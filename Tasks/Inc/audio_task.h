#ifndef AUDIO_TASK_H_
#define AUDIO_TASK_H_

#include "cmsis_os.h"

/**
 * @brief  RTOS task function for real-time DSP audio processing.
 *         Waits for DMA Half/Full Transfer flags, processes audio samples
 *         through active effects, calculates VU meter and triggers FFT visualizer
 * @param  argument: Task argument pointer
 * @retval None
 */
void audio_task_entry(void *argument);

#endif /* AUDIO_TASK_H_ */
