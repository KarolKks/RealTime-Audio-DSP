#include "hw_audio.h"
#include <string.h>

extern SAI_HandleTypeDef hsai_BlockA1;
extern SAI_HandleTypeDef hsai_BlockB1;

extern osThreadId_t defaultTaskHandle;

int32_t rxAudioBuffer[BUFFER_SIZE];
int32_t txAudioBuffer[BUFFER_SIZE];


HAL_StatusTypeDef hw_audio_init(void){
    memset(rxAudioBuffer, 0, sizeof(rxAudioBuffer));
    memset(txAudioBuffer, 0, sizeof(txAudioBuffer));    

    if(HAL_SAI_Receive_DMA(&hsai_BlockB1, (uint8_t *)rxAudioBuffer, BUFFER_SIZE) != HAL_OK){
        return HAL_ERROR;
    }

    if(HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t *)txAudioBuffer, BUFFER_SIZE) != HAL_OK){
        return HAL_ERROR;
    }
    return HAL_OK;
}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (hsai->Instance == SAI1_Block_B && defaultTaskHandle != NULL)
    {
        osThreadFlagsSet(defaultTaskHandle, AUDIO_FLAG_HALF_COMPLETE);
    }
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (hsai->Instance == SAI1_Block_B && defaultTaskHandle != NULL)
    {
        osThreadFlagsSet(defaultTaskHandle, AUDIO_FLAG_FULL_COMPLETE);
    }
}