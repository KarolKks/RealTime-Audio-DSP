#ifndef UI_TASK_H_
#define UI_TASK_H_

#include "cmsis_os.h"

/**
 * @brief  RTOS task function for handling the user interface.
 *         Polls encoders and buttons, updates DSP parameters,
 *         and draws the menu, VU meter, and FFT spectrum on OLED.
 * @param  argument: Task argument pointer (required by CMSIS-RTOS V2 API).
 * @retval None
 */
void ui_task_entry(void *argument);

#endif /* UI_TASK_H_ */
