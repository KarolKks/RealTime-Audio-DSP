#include "rotary_encoder.h"
#include "bsp_timers.h"
#include "bsp_gpio.h"

typedef struct {
    uint8_t stable_state;
    uint8_t debounce_cnt;
    bool    clicked_flag;
    bsp_gpio_pin_t bsp_pin;
} button_state_t;

static button_state_t s_buttons[BUTTON_COUNT] = {
    [BUTTON_1] = { .stable_state = 1, .debounce_cnt = 0, .clicked_flag = false, .bsp_pin = BSP_PIN_ENC1_SW },
    [BUTTON_2] = { .stable_state = 1, .debounce_cnt = 0, .clicked_flag = false, .bsp_pin = BSP_PIN_ENC2_SW }
};

void rotary_encoder_init(void)
{
    bsp_timers_set_counter(BSP_TIMER_ENC_1, 0);
    bsp_timers_set_counter(BSP_TIMER_ENC_2, 0);

    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        s_buttons[i].stable_state = 1;
        s_buttons[i].debounce_cnt = 0;
        s_buttons[i].clicked_flag = false;
    }
}

void rotary_encoder_update(void)
{
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        uint8_t raw = (uint8_t)bsp_gpio_read(s_buttons[i].bsp_pin);

        if (raw == s_buttons[i].stable_state)
        {
            s_buttons[i].debounce_cnt = 0;
        }
        else
        {
            s_buttons[i].debounce_cnt++;
            if (s_buttons[i].debounce_cnt >= 3)
            {
                uint8_t prev = s_buttons[i].stable_state;
                s_buttons[i].stable_state = raw;
                s_buttons[i].debounce_cnt = 0;

                // Detect falling edge (1 -> 0 with pull-up resistor)
                if (s_buttons[i].stable_state == 0 && prev == 1)
                {
                    s_buttons[i].clicked_flag = true;
                }
            }
        }
    }
}

int32_t rotary_encoder_get_value(encoder_id_t enc)
{
    if (enc == ENCODER_1)
    {
        return (int32_t)bsp_timers_get_counter(BSP_TIMER_ENC_1);
    }
    else if (enc == ENCODER_2)
    {
        return (int16_t)bsp_timers_get_counter(BSP_TIMER_ENC_2);
    }
    return 0;
}

void rotary_encoder_set_value(encoder_id_t enc, int32_t val)
{
    if (enc == ENCODER_1)
    {
        bsp_timers_set_counter(BSP_TIMER_ENC_1, (uint32_t)val);
    }
    else if (enc == ENCODER_2)
    {
        bsp_timers_set_counter(BSP_TIMER_ENC_2, (uint32_t)val);
    }
}

bool rotary_encoder_is_button_clicked(button_id_t btn)
{
    if (btn < BUTTON_COUNT)
    {
        bool clicked = s_buttons[btn].clicked_flag;
        s_buttons[btn].clicked_flag = false;
        return clicked;
    }
    return false;
}

