#include "audio_stream.h"
#include "bsp.h"
#include "bsp_sai.h"
#include <string.h>

static int32_t s_rx_audio_buffer[AUDIO_STREAM_BUFFER_WORDS];
static int32_t s_tx_audio_buffer[AUDIO_STREAM_BUFFER_WORDS];
static osThreadId_t s_processing_task;
static bool s_started;

static void notify_audio_task(uint32_t event)
{
    if (s_processing_task == NULL ||
        (osThreadFlagsSet(s_processing_task, event) & osFlagsError) != 0U)
    {
        bsp_error_handler();
    }
}

static void on_audio_half_complete(void *context)
{
    (void)context;
    notify_audio_task(AUDIO_STREAM_EVENT_HALF_COMPLETE);
}

static void on_audio_full_complete(void *context)
{
    (void)context;
    notify_audio_task(AUDIO_STREAM_EVENT_FULL_COMPLETE);
}

audio_status_t audio_stream_start(osThreadId_t processing_task)
{
    if (processing_task == NULL)
    {
        return AUDIO_STATUS_INVALID_ARGUMENT;
    }
    if (s_started)
    {
        return AUDIO_STATUS_ALREADY_STARTED;
    }

    memset(s_rx_audio_buffer, 0, sizeof(s_rx_audio_buffer));
    memset(s_tx_audio_buffer, 0, sizeof(s_tx_audio_buffer));

    s_processing_task = processing_task;
    bsp_sai_register_callbacks(on_audio_half_complete, on_audio_full_complete, NULL);

    if (!bsp_sai_start_duplex_dma(s_rx_audio_buffer, s_tx_audio_buffer,
                                  AUDIO_STREAM_BUFFER_WORDS))
    {
        bsp_sai_register_callbacks(NULL, NULL, NULL);
        s_processing_task = NULL;
        return AUDIO_STATUS_HARDWARE_ERROR;
    }

    s_started = true;
    return AUDIO_STATUS_OK;
}

audio_status_t audio_stream_wait(uint32_t *events)
{
    if (events == NULL)
    {
        return AUDIO_STATUS_INVALID_ARGUMENT;
    }
    *events = 0U;
    if (!s_started)
    {
        return AUDIO_STATUS_NOT_INITIALIZED;
    }

    uint32_t flags = osThreadFlagsWait(
        AUDIO_STREAM_EVENT_HALF_COMPLETE | AUDIO_STREAM_EVENT_FULL_COMPLETE,
        osFlagsWaitAny,
        osWaitForever);
    if ((flags & osFlagsError) != 0U)
    {
        return AUDIO_STATUS_RTOS_ERROR;
    }

    *events = flags & (AUDIO_STREAM_EVENT_HALF_COMPLETE |
                       AUDIO_STREAM_EVENT_FULL_COMPLETE);
    return (*events != 0U) ? AUDIO_STATUS_OK : AUDIO_STATUS_RTOS_ERROR;
}

const int32_t *audio_stream_rx_buffer(void)
{
    return s_rx_audio_buffer;
}

int32_t *audio_stream_tx_buffer(void)
{
    return s_tx_audio_buffer;
}
