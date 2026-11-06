#ifndef BSP_GPIO_H_
#define BSP_GPIO_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Logical pin enumeration for the target board.
 */
typedef enum {
    BSP_PIN_OLED_CS = 0,
    BSP_PIN_OLED_DC,
    BSP_PIN_OLED_RES,
    BSP_PIN_ENC1_SW,
    BSP_PIN_ENC2_SW,
    BSP_PIN_COUNT
} bsp_gpio_pin_t;

typedef enum {
    BSP_PIN_RESET = 0,
    BSP_PIN_SET   = 1
} bsp_pin_state_t;

/**
 * @brief  Initializes all GPIO pins used by the application board.
 */
void bsp_gpio_init(void);

/**
 * @brief  Sets the output state of a logical GPIO pin.
 * @param  pin: Logical pin identifier.
 * @param  state: BSP_PIN_RESET (low) or BSP_PIN_SET (high).
 */
void bsp_gpio_write(bsp_gpio_pin_t pin, bsp_pin_state_t state);

/**
 * @brief  Reads the input state of a logical GPIO pin.
 * @param  pin: Logical pin identifier.
 * @retval BSP_PIN_RESET or BSP_PIN_SET.
 */
bsp_pin_state_t bsp_gpio_read(bsp_gpio_pin_t pin);

/**
 * @brief  Toggles the output state of a logical GPIO pin.
 * @param  pin: Logical pin identifier.
 */
void bsp_gpio_toggle(bsp_gpio_pin_t pin);

#endif /* BSP_GPIO_H_ */

