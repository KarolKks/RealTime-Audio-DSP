#include "audio_stream.h"
#include "bsp_sai.h"
#include <string.h>

static int32_t s_rx_audio_buffer[AUDIO_STREAM_BUFFER_WORDS];
static int32_t s_tx_audio_buffer[AUDIO_STREAM_BUFFER_WORDS];
static audio_stream_callback_t s_half_complete_callback;
static audio_stream_callback_t s_full_complete_callback;
static void *s_callback_context;
static bool s_started;

static void on_audio_half_complete(void *context)
{
    (void)context;
    if (s_half_complete_callback != NULL)
    {
        s_half_complete_callback(s_callback_context);
    }
}

static void on_audio_full_complete(void *context)
{
    (void)context;
    if (s_full_complete_callback != NULL)
    {
        s_full_complete_callback(s_callback_context);
    }
}

audio_status_t audio_stream_start(audio_stream_callback_t half_complete,
                                  audio_stream_callback_t full_complete,
                                  void *context)
{
    if (half_complete == NULL || full_complete == NULL)
    {
        return AUDIO_STATUS_INVALID_ARGUMENT;
    }
    if (s_started)
    {
        return AUDIO_STATUS_ALREADY_STARTED;
    }

    memset(s_rx_audio_buffer, 0, sizeof(s_rx_audio_buffer));
    memset(s_tx_audio_buffer, 0, sizeof(s_tx_audio_buffer));

    s_half_complete_callback = half_complete;
    s_full_complete_callback = full_complete;
    s_callback_context = context;
    bsp_sai_register_callbacks(on_audio_half_complete, on_audio_full_complete, NULL);

    if (!bsp_sai_start_duplex_dma(s_rx_audio_buffer, s_tx_audio_buffer,
                                  AUDIO_STREAM_BUFFER_WORDS))
    {
        bsp_sai_register_callbacks(NULL, NULL, NULL);
        s_half_complete_callback = NULL;
        s_full_complete_callback = NULL;
        s_callback_context = NULL;
        return AUDIO_STATUS_HARDWARE_ERROR;
    }

    s_started = true;
    return AUDIO_STATUS_OK;
}

const int32_t *audio_stream_rx_buffer(void)
{
    return s_rx_audio_buffer;
}

int32_t *audio_stream_tx_buffer(void)
{
    return s_tx_audio_buffer;
}
