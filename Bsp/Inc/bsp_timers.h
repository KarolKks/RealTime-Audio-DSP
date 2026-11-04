#ifndef BSP_TIMERS_H_
#define BSP_TIMERS_H_

#include <stdint.h>

typedef enum {
    BSP_TIMER_ENC_1 = 0, // TIM2 (32-bit counter)
    BSP_TIMER_ENC_2      // TIM3 (16-bit counter)
} bsp_timer_enc_id_t;

/**
 * @brief  Initializes hardware timers TIM2 and TIM3 in quadrature encoder interface mode
 *         and starts counter operation.
 */
void bsp_timers_init(void);

/**
 * @brief  Gets current counter value of specified encoder timer.
 * @param  timer_id: BSP_TIMER_ENC_1 or BSP_TIMER_ENC_2.
 * @retval 32-bit counter value.
 */
uint32_t bsp_timers_get_counter(bsp_timer_enc_id_t timer_id);

/**
 * @brief  Sets current counter value of specified encoder timer.
 * @param  timer_id: BSP_TIMER_ENC_1 or BSP_TIMER_ENC_2.
 * @param  val: New counter value.
 */
void bsp_timers_set_counter(bsp_timer_enc_id_t timer_id, uint32_t val);

#endif /* BSP_TIMERS_H_ */

