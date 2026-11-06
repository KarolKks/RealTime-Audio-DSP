#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ENCODER_1 = 0,
    ENCODER_2,
    ENCODER_COUNT
} encoder_id_t;

typedef enum {
    BUTTON_1 = 0,
    BUTTON_2,
    BUTTON_COUNT
} button_id_t;

/**
 * @brief  Initializes encoder counters and button states.
 */
void rotary_encoder_init(void);

/**
 * @brief  Updates debounce states for push-buttons. Call periodically (e.g. every 10-50 ms).
 */
void rotary_encoder_update(void);

/**
 * @brief  Gets the current counter value of the specified encoder.
 * @param  enc: ENCODER_1 or ENCODER_2.
 * @retval 32-bit counter value.
 */
int32_t rotary_encoder_get_value(encoder_id_t enc);

/**
 * @brief  Sets the counter value of the specified encoder.
 * @param  enc: ENCODER_1 or ENCODER_2.
 * @param  val: New counter value.
 */
void rotary_encoder_set_value(encoder_id_t enc, int32_t val);

/**
 * @brief  Checks if a button press event occurred (falling edge). Clears the flag on read.
 * @param  btn: BUTTON_1 or BUTTON_2.
 * @retval true if button was clicked since last read, false otherwise.
 */
bool rotary_encoder_is_button_clicked(button_id_t btn);

#endif /* ROTARY_ENCODER_H_ */

