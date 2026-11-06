#ifndef UI_TASK_H_
#define UI_TASK_H_

/**
 * @brief  RTOS task function for handling the user interface.
 *         Maintains UI parameter state, exchanges snapshots with the audio
 *         task, renders the display, and flushes the framebuffer to the OLED.
 * @param  argument Task argument required by CMSIS-RTOS; unused by this task.
 * @retval None
 */
void ui_task_entry(void *argument);

#endif /* UI_TASK_H_ */
