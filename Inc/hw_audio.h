#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

#ifndef HW_AUDIO_H_
#define HW_AUDIO_H_

#define BUFFER_SIZE 512
#define HALF_BUFFER_SIZE 256

#define AUDIO_FLAG_HALF_COMPLETE   0x01
#define AUDIO_FLAG_FULL_COMPLETE   0x02


extern int32_t rxAudioBuffer[BUFFER_SIZE];
extern int32_t txAudioBuffer[BUFFER_SIZE];

/**
 * @brief  Initializes audio peripherals (SAI, DMA) and starts DMA streams.
 * @param  None
 * @retval HAL status (HAL_OK on success, HAL_ERROR or other on failure)
 */
HAL_StatusTypeDef hw_audio_init(void);



#endif /* HW_AUDIO_H_ */
