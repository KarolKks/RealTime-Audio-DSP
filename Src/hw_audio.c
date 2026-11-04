#include "hw_audio.h"
#include "bsp_sai.h"
#include <string.h>

extern osThreadId_t defaultTaskHandle;

int32_t rxAudioBuffer[BUFFER_SIZE];
int32_t txAudioBuffer[BUFFER_SIZE];

static void on_audio_half_complete(void *context)
{
    (void)context;
    if (defaultTaskHandle != NULL)
    {
        osThreadFlagsSet(defaultTaskHandle, AUDIO_FLAG_HALF_COMPLETE);
    }
}

static void on_audio_full_complete(void *context)
{
    (void)context;
    if (defaultTaskHandle != NULL)
    {
        osThreadFlagsSet(defaultTaskHandle, AUDIO_FLAG_FULL_COMPLETE);
    }
}

HAL_StatusTypeDef hw_audio_init(void)
{
    memset(rxAudioBuffer, 0, sizeof(rxAudioBuffer));
    memset(txAudioBuffer, 0, sizeof(txAudioBuffer));

    bsp_sai_register_callbacks(on_audio_half_complete, on_audio_full_complete, NULL);

    if (!bsp_sai_start_duplex_dma(rxAudioBuffer, txAudioBuffer, BUFFER_SIZE))
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}