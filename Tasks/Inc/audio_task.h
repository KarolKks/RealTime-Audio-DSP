#ifndef AUDIO_TASK_H_
#define AUDIO_TASK_H_

/**
 * @brief  RTOS task function for real-time DSP audio processing.
 *         Waits for DMA Half/Full Transfer flags, processes audio samples
 *         through active effects, calculates visualization data, and publishes
 *         it for the UI task.
 * @param  argument Task argument required by CMSIS-RTOS; unused by this task.
 * @retval None
 */
void audio_task_entry(void *argument);

#endif /* AUDIO_TASK_H_ */
