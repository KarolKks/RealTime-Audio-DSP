#include "hw_encoders.h"
#include "bsp_timers.h"
#include "bsp_gpio.h"

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
    // Hardware timers are initialized and started in bsp_timers_init()
    bsp_timers_set_counter(BSP_TIMER_ENC_1, 0);
    bsp_timers_set_counter(BSP_TIMER_ENC_2, 0);
}

void hw_encoders_update(void)
{
    // Read raw GPIO states of encoder pushbuttons via BSP abstraction
    uint8_t btn1_raw = (uint8_t)bsp_gpio_read(BSP_PIN_ENC1_SW);
    uint8_t btn2_raw = (uint8_t)bsp_gpio_read(BSP_PIN_ENC2_SW);

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
    return (int32_t)bsp_timers_get_counter(BSP_TIMER_ENC_1);
}

int16_t hw_encoders_get_count2(void)
{
    return (int16_t)bsp_timers_get_counter(BSP_TIMER_ENC_2);
}

void hw_encoders_set_count1(int32_t val)
{
    bsp_timers_set_counter(BSP_TIMER_ENC_1, (uint32_t)val);
}

void hw_encoders_set_count2(int16_t val)
{
    bsp_timers_set_counter(BSP_TIMER_ENC_2, (uint32_t)val);
}

uint8_t hw_encoders_is_btn1_pressed(void)
{
    uint8_t is_pressed = btn1_pressed_flag;
    btn1_pressed_flag = 0;
    return is_pressed;
}

uint8_t hw_encoders_is_btn2_pressed(void)
{
    uint8_t is_pressed = btn2_pressed_flag;
    btn2_pressed_flag = 0;
    return is_pressed;
}
