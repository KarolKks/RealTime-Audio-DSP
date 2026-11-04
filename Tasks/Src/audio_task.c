#include "audio_task.h"
#include "audio_stream.h"
#include "bsp.h"
#include "dsp_effects.h"
#include "dsp_fft.h"

static int32_t mono_samples[AUDIO_STREAM_HALF_BUFFER_WORDS / 2U];

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

    global_vu_level = max_block_value;

    static uint16_t fft_throttle_count;
    if (++fft_throttle_count >= 8U)
    {
        fft_throttle_count = 0U;
        dsp_fft_process(mono_samples);
    }
}

void audio_task_entry(void *argument)
{
    (void)argument;

    dsp_effects_init();
    dsp_fft_init();

    if (audio_stream_start(osThreadGetId()) != AUDIO_STATUS_OK)
    {
        bsp_error_handler();
    }

    for (;;)
    {
        uint32_t events = 0U;
        if (audio_stream_wait(&events) != AUDIO_STATUS_OK)
        {
            bsp_error_handler();
        }

        if (events == (AUDIO_STREAM_EVENT_HALF_COMPLETE |
                       AUDIO_STREAM_EVENT_FULL_COMPLETE))
        {
            bsp_error_handler();
        }

        if ((events & AUDIO_STREAM_EVENT_HALF_COMPLETE) != 0U)
        {
            process_audio_half(0U);
        }
        if ((events & AUDIO_STREAM_EVENT_FULL_COMPLETE) != 0U)
        {
            process_audio_half(AUDIO_STREAM_HALF_BUFFER_WORDS);
        }
    }
}
