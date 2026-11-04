#include "bsp_gpio.h"
#include "stm32l4xx_hal.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
} bsp_gpio_map_t;

static const bsp_gpio_map_t pin_map[BSP_PIN_COUNT] = {
    [BSP_PIN_OLED_CS]  = { GPIOB, GPIO_PIN_12 },
    [BSP_PIN_OLED_DC]  = { GPIOC, GPIO_PIN_8  },
    [BSP_PIN_OLED_RES] = { GPIOC, GPIO_PIN_6  },
    [BSP_PIN_ENC1_SW]  = { GPIOB, GPIO_PIN_14 },
    [BSP_PIN_ENC2_SW]  = { GPIOB, GPIO_PIN_13 }
};

void bsp_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Configure GPIO pin Output Levels */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_8, GPIO_PIN_SET);

    /* Configure GPIO pin : OLED_CS_Pin (PB12) */
    GPIO_InitStruct.Pin   = GPIO_PIN_12;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Configure GPIO pins : OLED_RES_Pin (PC6), OLED_DC_Pin (PC8) */
    GPIO_InitStruct.Pin   = GPIO_PIN_6 | GPIO_PIN_8;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Configure GPIO pins : EncoderSwitch1_Pin (PB14), EncoderSwitch2_Pin (PB13) */
    GPIO_InitStruct.Pin  = GPIO_PIN_14 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void bsp_gpio_write(bsp_gpio_pin_t pin, bsp_pin_state_t state)
{
    if (pin < BSP_PIN_COUNT)
    {
        HAL_GPIO_WritePin(pin_map[pin].port, pin_map[pin].pin, (state == BSP_PIN_SET) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

bsp_pin_state_t bsp_gpio_read(bsp_gpio_pin_t pin)
{
    if (pin < BSP_PIN_COUNT)
    {
        return (HAL_GPIO_ReadPin(pin_map[pin].port, pin_map[pin].pin) == GPIO_PIN_SET) ? BSP_PIN_SET : BSP_PIN_RESET;
    }
    return BSP_PIN_RESET;
}

void bsp_gpio_toggle(bsp_gpio_pin_t pin)
{
    if (pin < BSP_PIN_COUNT)
    {
        HAL_GPIO_TogglePin(pin_map[pin].port, pin_map[pin].pin);
    }
}

