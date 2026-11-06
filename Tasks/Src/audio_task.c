#include "audio_task.h"
#include "audio_stream.h"
#include "bsp.h"
#include "dsp_effects.h"
#include "dsp_fft.h"
#include "task_messages.h"
#include "cmsis_os.h"
#include <string.h>

#define AUDIO_TASK_EVENT_HALF_COMPLETE 0x01U
#define AUDIO_TASK_EVENT_FULL_COMPLETE 0x02U
#define AUDIO_TASK_EVENT_MASK (AUDIO_TASK_EVENT_HALF_COMPLETE | \
                               AUDIO_TASK_EVENT_FULL_COMPLETE)

static int32_t mono_samples[AUDIO_STREAM_HALF_BUFFER_WORDS / 2U];
static dsp_spectrum_t s_spectrum;
static ui_visualization_t s_visualization;
static osThreadId_t s_audio_task_id;

static void notify_audio_task(uint32_t event)
{
    if (s_audio_task_id == NULL ||
        (osThreadFlagsSet(s_audio_task_id, event) & osFlagsError) != 0U)
    {
        bsp_error_handler();
    }
}

static void on_audio_half_complete(void *context)
{
    (void)context;
    notify_audio_task(AUDIO_TASK_EVENT_HALF_COMPLETE);
}

static void on_audio_full_complete(void *context)
{
    (void)context;
    notify_audio_task(AUDIO_TASK_EVENT_FULL_COMPLETE);
}

static void process_audio_half(uint16_t offset)
{
    const int32_t *input = audio_stream_rx_buffer() + offset;
    int32_t *output = audio_stream_tx_buffer() + offset;
    int32_t max_block_value = 0;

    for (uint16_t i = 0; i < AUDIO_STREAM_HALF_BUFFER_WORDS; i += 2U)
    {
        int32_t sample = input[i];
        mono_samples[i / 2U] = sample;

        int32_t absolute_value = (sample < 0) ? -sample : sample;
        if (absolute_value > max_block_value)
        {
            max_block_value = absolute_value;
        }

        int32_t output_left = 0;
        int32_t output_right = 0;
        dsp_process_sample(sample, &output_left, &output_right);
        output[i] = output_left;
        output[i + 1U] = output_right;
    }

    uint32_t level_per_mille =
        (uint32_t)(((uint64_t)max_block_value * 1000U) / DSP_MAX_INT24);
    if (level_per_mille > 1000U)
    {
        level_per_mille = 1000U;
    }
    s_visualization.peak_level_per_mille = (uint16_t)level_per_mille;

    static uint16_t fft_throttle_count;
    if (++fft_throttle_count >= 8U)
    {
        fft_throttle_count = 0U;
        dsp_fft_process(mono_samples, &s_spectrum);
        for (uint8_t band = 0U; band < UI_SPECTRUM_BAND_COUNT; band++)
        {
            s_visualization.band_magnitudes[band] =
                s_spectrum.band_magnitudes[band];
        }
    }

    if (task_messages_publish_visualization(&s_visualization) != TASK_MESSAGES_OK)
    {
        bsp_error_handler();
    }
}

void audio_task_entry(void *argument)
{
    (void)argument;

    dsp_effects_init();
    dsp_fft_init();

    s_audio_task_id = osThreadGetId();
    if (audio_stream_start(on_audio_half_complete, on_audio_full_complete, NULL)
        != AUDIO_STATUS_OK)
    {
        bsp_error_handler();
    }

    for (;;)
    {
        uint32_t events = osThreadFlagsWait(AUDIO_TASK_EVENT_MASK,
                                            osFlagsWaitAny,
                                            osWaitForever);
        if ((events & osFlagsError) != 0U)
        {
            bsp_error_handler();
        }

        audio_settings_t settings;
        task_messages_status_t message_status =
            task_messages_receive_audio_settings(&settings);
        if (message_status == TASK_MESSAGES_OK)
        {
            if (!dsp_effects_apply_params(&settings))
            {
                bsp_error_handler();
            }
        }
        else if (message_status != TASK_MESSAGES_EMPTY)
        {
            bsp_error_handler();
        }

        if (events == AUDIO_TASK_EVENT_MASK)
        {
            bsp_error_handler();
        }
        if ((events & AUDIO_TASK_EVENT_HALF_COMPLETE) != 0U)
        {
            process_audio_half(0U);
        }
        if ((events & AUDIO_TASK_EVENT_FULL_COMPLETE) != 0U)
        {
            process_audio_half(AUDIO_STREAM_HALF_BUFFER_WORDS);
        }
    }
}
