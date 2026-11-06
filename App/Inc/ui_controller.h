#ifndef UI_CONTROLLER_H_
#define UI_CONTROLLER_H_

#include "audio_model.h"
#include "ui_model.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    int32_t encoder_1_position;
    int32_t encoder_2_position;
    bool button_1_clicked;
    bool button_2_clicked;
} ui_controller_input_t;

typedef struct
{
    int32_t encoder_1_position;
    int32_t encoder_2_position;
} ui_controller_encoder_state_t;

/**
 * @brief Initializes controller output positions from the current parameters.
 * @param settings Current audio settings used to initialize the controls.
 * @param encoder_state Destination for the initial encoder positions.
 * @retval true if initialized, otherwise false for invalid input.
 */
bool ui_controller_init(const audio_settings_t *settings,
                        ui_controller_encoder_state_t *encoder_state);

/**
 * @brief Applies a user-input snapshot to the local audio-settings model.
 * @param settings Parameter state owned by the UI task; updated in place.
 * @param input Encoder positions and button events read by the UI task.
 * @param encoder_state Destination for normalized encoder positions.
 * @retval true if input was processed, otherwise false for invalid input.
 */
bool ui_controller_update(audio_settings_t *settings,
                          const ui_controller_input_t *input,
                          ui_controller_encoder_state_t *encoder_state);

/**
 * @brief Creates a display-ready snapshot from the current audio settings.
 * @param settings Current audio settings.
 * @param display Destination for the formatted effect and parameter information.
 * @retval true if the snapshot was created, otherwise false for invalid input.
 */
bool ui_controller_get_display(const audio_settings_t *settings, ui_effect_display_t *display);

#endif /* UI_CONTROLLER_H_ */
