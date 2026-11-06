#include "ui_task.h"
#include "ui_controller.h"
#include "ui_view.h"
#include "task_messages.h"
#include "sh1106.h"
#include "rotary_encoder.h"
#include "gfx.h"
#include "audio_model.h"
#include "bsp.h"
#include "cmsis_os.h"

void ui_task_entry(void *argument)
{
    (void)argument;

    audio_settings_t settings;
    ui_visualization_t visualization = {0};
    ui_effect_display_t display;
    ui_controller_encoder_state_t encoder_state;

    if (!audio_settings_get_defaults(&settings))
    {
        bsp_error_handler();
    }

    rotary_encoder_init();
    if (!sh1106_init())
    {
        bsp_error_handler();
    }
    gfx_init();
    if (!ui_controller_init(&settings, &encoder_state))
    {
        bsp_error_handler();
    }
    rotary_encoder_set_value(ENCODER_1, encoder_state.encoder_1_position);
    rotary_encoder_set_value(ENCODER_2, encoder_state.encoder_2_position);

    for (;;)
    {
        rotary_encoder_update();

        ui_controller_input_t input = {
            .encoder_1_position = rotary_encoder_get_value(ENCODER_1),
            .encoder_2_position = rotary_encoder_get_value(ENCODER_2),
            .button_1_clicked = rotary_encoder_is_button_clicked(BUTTON_1),
            .button_2_clicked = rotary_encoder_is_button_clicked(BUTTON_2)
        };
        if (!ui_controller_update(&settings, &input, &encoder_state))
        {
            bsp_error_handler();
        }

        if (encoder_state.encoder_1_position != input.encoder_1_position)
        {
            rotary_encoder_set_value(ENCODER_1, encoder_state.encoder_1_position);
        }
        if (encoder_state.encoder_2_position != input.encoder_2_position)
        {
            rotary_encoder_set_value(ENCODER_2, encoder_state.encoder_2_position);
        }

        if (task_messages_publish_audio_settings(&settings) != TASK_MESSAGES_OK)
        {
            bsp_error_handler();
        }

        task_messages_status_t message_status =
            task_messages_receive_visualization(&visualization);
        if (message_status != TASK_MESSAGES_OK &&
            message_status != TASK_MESSAGES_EMPTY)
        {
            bsp_error_handler();
        }

        if (!ui_controller_get_display(&settings, &display))
        {
            bsp_error_handler();
        }
        ui_view_render(&display, &visualization);

        if (!sh1106_flush(gfx_get_framebuffer()))
        {
            bsp_error_handler();
        }

        osDelay(50U);
    }
}
