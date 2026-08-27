#include "hw_encoders.h"
#include "main.h"

// Timer handles defined in main.c
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

// Debounce state variables for switches (1 = released, 0 = pressed due to PULLUP)
static uint8_t btn1_stable = 1;
static uint8_t btn2_stable = 1;
static uint8_t btn1_cnt = 0;
static uint8_t btn2_cnt = 0;

// Edge detection flags (1 = click detected and pending consumption)
static uint8_t btn1_pressed_flag = 0;
static uint8_t btn2_pressed_flag = 0;

void hw_encoders_init(void)
{
    // Start hardware quadrature encoder interfaces
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
}

void hw_encoders_update(void)
{
    // Read raw GPIO states of encoder pushbuttons
    uint8_t btn1_raw = HAL_GPIO_ReadPin(GPIOB, EncoderSwitch1_Pin);
    uint8_t btn2_raw = HAL_GPIO_ReadPin(GPIOB, EncoderSwitch2_Pin);

    // Debounce Button 1 (Switch 1)
    if (btn1_raw == btn1_stable)
    {
        btn1_cnt = 0;
    }
    else
    {
        btn1_cnt++;
        if (btn1_cnt >= 3) // Active/stable state change after ~150ms of polling
        {
            uint8_t prev_stable = btn1_stable;
            btn1_stable = btn1_raw;
            btn1_cnt = 0;

            // Check for falling edge (transition from 1 -> 0, representing button press)
            if (btn1_stable == 0 && prev_stable == 1)
            {
                btn1_pressed_flag = 1;
            }
        }
    }

    // Debounce Button 2 (Switch 2)
    if (btn2_raw == btn2_stable)
    {
        btn2_cnt = 0;
    }
    else
    {
        btn2_cnt++;
        if (btn2_cnt >= 3)
        {
            uint8_t prev_stable = btn2_stable;
            btn2_stable = btn2_raw;
            btn2_cnt = 0;

            // Check for falling edge (transition from 1 -> 0, representing button press)
            if (btn2_stable == 0 && prev_stable == 1)
            {
                btn2_pressed_flag = 1;
            }
        }
    }
}

int32_t hw_encoders_get_count1(void)
{
    return (int32_t)__HAL_TIM_GET_COUNTER(&htim2);
}

int16_t hw_encoders_get_count2(void)
{
    return (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
}

void hw_encoders_set_count1(int32_t val)
{
    __HAL_TIM_SET_COUNTER(&htim2, val);
}

void hw_encoders_set_count2(int16_t val)
{
    __HAL_TIM_SET_COUNTER(&htim3, val);
}

uint8_t hw_encoders_is_btn1_pressed(void)
{
    // Clear-on-read latch implementation
    uint8_t is_pressed = btn1_pressed_flag;
    btn1_pressed_flag = 0;
    return is_pressed;
}

uint8_t hw_encoders_is_btn2_pressed(void)
{
    // Clear-on-read latch implementation
    uint8_t is_pressed = btn2_pressed_flag;
    btn2_pressed_flag = 0;
    return is_pressed;
}
