#ifndef HW_ENCODERS_H_
#define HW_ENCODERS_H_

#include "stm32l4xx_hal.h"

/**
 * @brief  Initializes the hardware timers in encoder mode.
 * @param  None
 * @retval None
 */
void hw_encoders_init(void);

/**
 * @brief  Updates the state of encoders and buttons. Performs debouncing on switches.
 *         Should be called periodically (e.g., every 10-50 ms) in the UI task.
 * @param  None
 * @retval None
 */
void hw_encoders_update(void);

/**
 * @brief  Gets the current counter value of Encoder 1 (TIM2).
 * @param  None
 * @retval Current count.
 */
int32_t hw_encoders_get_count1(void);

/**
 * @brief  Gets the current counter value of Encoder 2 (TIM3).
 * @param  None
 * @retval Current count.
 */
int16_t hw_encoders_get_count2(void);

/**
 * @brief  Sets the counter value of Encoder 1 (TIM2).
 * @param  val: New counter value.
 * @retval None
 */
void hw_encoders_set_count1(int32_t val);

/**
 * @brief  Sets the counter value of Encoder 2 (TIM3).
 * @param  val: New counter value.
 * @retval None
 */
void hw_encoders_set_count2(int16_t val);

/**
 * @brief  Checks if Button 1 (Switch 1) was pressed (falling edge detected).
 *         Clears the flag upon reading.
 * @param  None
 * @retval 1 if pressed, 0 otherwise.
 */
uint8_t hw_encoders_is_btn1_pressed(void);

/**
 * @brief  Checks if Button 2 (Switch 2) was pressed (falling edge detected).
 *         Clears the flag upon reading.
 * @param  None
 * @retval 1 if pressed, 0 otherwise.
 */
uint8_t hw_encoders_is_btn2_pressed(void);

#endif /* HW_ENCODERS_H_ */
