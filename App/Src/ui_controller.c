#include "ui_controller.h"
#include <math.h>
#include <stdio.h>

typedef struct
{
    const char *name;
    const char *parameter_label;
    int16_t encoder_min;
    int16_t encoder_max;
    int32_t bar_min;
    int32_t bar_max;
    void (*on_button)(audio_settings_t *settings);
    int16_t (*get_encoder_value)(const audio_settings_t *settings);
    void (*set_parameter)(audio_settings_t *settings, int16_t value);
    int32_t (*get_bar_value)(const audio_settings_t *settings);
    void (*format_value)(const audio_settings_t *settings, char *buffer, size_t size);
} effect_ui_descriptor_t;

static void clean_button(audio_settings_t *settings) { settings->volume_gain = 4; }
static int16_t clean_encoder(const audio_settings_t *settings) { return settings->volume_gain; }
static void clean_set(audio_settings_t *settings, int16_t value) { settings->volume_gain = value; }
static int32_t clean_bar(const audio_settings_t *settings) { return settings->volume_gain; }
static void clean_format(const audio_settings_t *settings, char *buffer, size_t size)
{
    (void)snprintf(buffer, size, "%dx", settings->volume_gain);
}

static void robot_button(audio_settings_t *settings)
{
    settings->ringmod_freq = 250;
    settings->ringmod_wave_shape = !settings->ringmod_wave_shape;
}
static int16_t robot_encoder(const audio_settings_t *settings) { return (settings->ringmod_freq - 50) / 10; }
static void robot_set(audio_settings_t *settings, int16_t value) { settings->ringmod_freq = 50 + value * 10; }
static int32_t robot_bar(const audio_settings_t *settings) { return settings->ringmod_freq; }
static void robot_format(const audio_settings_t *settings, char *buffer, size_t size)
{
    (void)snprintf(buffer, size, "%dHz", settings->ringmod_freq);
}

static void distortion_button(audio_settings_t *settings)
{
    settings->dist_gain = 5;
    settings->dist_type = !settings->dist_type;
}
static int16_t distortion_encoder(const audio_settings_t *settings) { return settings->dist_gain; }
static void distortion_set(audio_settings_t *settings, int16_t value) { settings->dist_gain = value; }
static int32_t distortion_bar(const audio_settings_t *settings) { return settings->dist_gain; }
static void distortion_format(const audio_settings_t *settings, char *buffer, size_t size)
{
    (void)snprintf(buffer, size, "%d", settings->dist_gain);
}

static void delay_button(audio_settings_t *settings)
{
    settings->delay_time_ms = 150;
    settings->delay_feedback_mode = !settings->delay_feedback_mode;
}
static int16_t delay_encoder(const audio_settings_t *settings) { return (settings->delay_time_ms - 50) / 10; }
static void delay_set(audio_settings_t *settings, int16_t value) { settings->delay_time_ms = 50 + value * 10; }
static int32_t delay_bar(const audio_settings_t *settings) { return settings->delay_time_ms; }
static void delay_format(const audio_settings_t *settings, char *buffer, size_t size)
{
    (void)snprintf(buffer, size, "%dms", settings->delay_time_ms);
}

static void pitch_button(audio_settings_t *settings)
{
    settings->pitch_ratio_percent = 100U;
}
static int16_t pitch_encoder(const audio_settings_t *settings)
{
    return (int16_t)((settings->pitch_ratio_percent - 50U) / 5U);
}
static void pitch_set(audio_settings_t *settings, int16_t value)
{
    settings->pitch_ratio_percent = (uint8_t)(50 + value * 5);
}
static int32_t pitch_bar(const audio_settings_t *settings)
{
    return (settings->pitch_ratio_percent - 50U) / 5U;
}
static void pitch_format(const audio_settings_t *settings, char *buffer, size_t size)
{
    (void)snprintf(buffer, size, "%u.%02ux",
                   settings->pitch_ratio_percent / 100U,
                   settings->pitch_ratio_percent % 100U);
}

static void tremolo_button(audio_settings_t *settings)
{
    settings->tremolo_freq = 5;
    settings->auto_pan_mode = !settings->auto_pan_mode;
}
static int16_t tremolo_encoder(const audio_settings_t *settings) { return settings->tremolo_freq; }
static void tremolo_set(audio_settings_t *settings, int16_t value) { settings->tremolo_freq = value; }
static int32_t tremolo_bar(const audio_settings_t *settings) { return settings->tremolo_freq; }
static void tremolo_format(const audio_settings_t *settings, char *buffer, size_t size)
{
    (void)snprintf(buffer, size, "%dHz", settings->tremolo_freq);
}

static void filter_button(audio_settings_t *settings)
{
    settings->filter_fc_idx = 10;
    settings->filter_type = !settings->filter_type;
}
static int16_t filter_encoder(const audio_settings_t *settings) { return settings->filter_fc_idx; }
static void filter_set(audio_settings_t *settings, int16_t value) { settings->filter_fc_idx = value; }
static int32_t filter_bar(const audio_settings_t *settings) { return settings->filter_fc_idx; }
static void filter_format(const audio_settings_t *settings, char *buffer, size_t size)
{
    int32_t frequency = (int32_t)(100.0f * powf(1.127f, (float)settings->filter_fc_idx));
    if (frequency >= 1000)
    {
        (void)snprintf(buffer, size, "%d.%dkHz", (int)(frequency / 1000),
                       (int)((frequency % 1000) / 100));
    }
    else
    {
        (void)snprintf(buffer, size, "%dHz", (int)frequency);
    }
}

static void bitcrusher_button(audio_settings_t *settings)
{
    settings->bc_downsample = 4;
    settings->bc_bit_depth = !settings->bc_bit_depth;
}
static int16_t bitcrusher_encoder(const audio_settings_t *settings) { return settings->bc_downsample; }
static void bitcrusher_set(audio_settings_t *settings, int16_t value) { settings->bc_downsample = value; }
static int32_t bitcrusher_bar(const audio_settings_t *settings) { return settings->bc_downsample; }
static void bitcrusher_format(const audio_settings_t *settings, char *buffer, size_t size)
{
    (void)snprintf(buffer, size, "%dx", settings->bc_downsample);
}

static void gate_button(audio_settings_t *settings)
{
    settings->gate_threshold_db = -45;
    settings->gate_release_mode = !settings->gate_release_mode;
}
static int16_t gate_encoder(const audio_settings_t *settings) { return -settings->gate_threshold_db; }
static void gate_set(audio_settings_t *settings, int16_t value) { settings->gate_threshold_db = -value; }
static int32_t gate_bar(const audio_settings_t *settings) { return -settings->gate_threshold_db; }
static void gate_format(const audio_settings_t *settings, char *buffer, size_t size)
{
    (void)snprintf(buffer, size, "%ddB", settings->gate_threshold_db);
}

static const effect_ui_descriptor_t s_effects[AUDIO_EFFECT_COUNT] = {
    [AUDIO_EFFECT_CLEAN] = {
        "CLEAN", "VOL", 1, 32, 1, 32,
        clean_button, clean_encoder, clean_set, clean_bar, clean_format
    },
    [AUDIO_EFFECT_ROBOT] = {
        "ROBOT", "FREQ", 0, 145, 50, 1500,
        robot_button, robot_encoder, robot_set, robot_bar, robot_format
    },
    [AUDIO_EFFECT_DISTORTION] = {
        "DIST", "SAT", 1, 25, 1, 25,
        distortion_button, distortion_encoder, distortion_set, distortion_bar, distortion_format
    },
    [AUDIO_EFFECT_DELAY] = {
        "ECHO", "DEL", 0, 55, 50, 600,
        delay_button, delay_encoder, delay_set, delay_bar, delay_format
    },
    [AUDIO_EFFECT_PITCH] = {
        "PITCH", "PTCH", 0, 30, 0, 30,
        pitch_button, pitch_encoder, pitch_set, pitch_bar, pitch_format
    },
    [AUDIO_EFFECT_TREMOLO_PAN] = {
        "TREM", "RATE", 1, 20, 1, 20,
        tremolo_button, tremolo_encoder, tremolo_set, tremolo_bar, tremolo_format
    },
    [AUDIO_EFFECT_FILTER] = {
        "FILTR", "FREQ", 0, 40, 0, 40,
        filter_button, filter_encoder, filter_set, filter_bar, filter_format
    },
    [AUDIO_EFFECT_BITCRUSHER] = {
        "CRUSH", "CRSH", 1, 30, 1, 30,
        bitcrusher_button, bitcrusher_encoder, bitcrusher_set, bitcrusher_bar, bitcrusher_format
    },
    [AUDIO_EFFECT_GATE] = {
        "BRAMK", "THR", 20, 60, 20, 60,
        gate_button, gate_encoder, gate_set, gate_bar, gate_format
    }
};

static const effect_ui_descriptor_t *get_effect(const audio_settings_t *params)
{
    if (params == NULL || params->current_effect >= AUDIO_EFFECT_COUNT)
    {
        return NULL;
    }
    return &s_effects[params->current_effect];
}

bool ui_controller_init(const audio_settings_t *params,
                        ui_controller_encoder_state_t *encoder_state)
{
    if (encoder_state == NULL || get_effect(params) == NULL)
    {
        return false;
    }

    encoder_state->encoder_1_position = params->current_effect * 2;
    encoder_state->encoder_2_position =
        s_effects[params->current_effect].get_encoder_value(params);
    return true;
}

bool ui_controller_update(audio_settings_t *params,
                          const ui_controller_input_t *input,
                          ui_controller_encoder_state_t *encoder_state)
{
    if (input == NULL || encoder_state == NULL)
    {
        return false;
    }

    const effect_ui_descriptor_t *effect = get_effect(params);
    if (effect == NULL)
    {
        return false;
    }

    encoder_state->encoder_1_position = input->encoder_1_position;
    encoder_state->encoder_2_position = input->encoder_2_position;

    if (input->button_1_clicked)
    {
        if (params->current_effect != AUDIO_EFFECT_CLEAN)
        {
            params->current_effect = AUDIO_EFFECT_CLEAN;
            encoder_state->encoder_1_position = 0;
            encoder_state->encoder_2_position =
                s_effects[AUDIO_EFFECT_CLEAN].get_encoder_value(params);
        }
        else
        {
            params->is_muted = !params->is_muted;
        }
    }

    if (input->button_2_clicked)
    {
        effect = get_effect(params);
        effect->on_button(params);
        encoder_state->encoder_2_position = effect->get_encoder_value(params);
    }

    if (encoder_state->encoder_1_position < 0)
    {
        encoder_state->encoder_1_position = 0;
    }
    else if (encoder_state->encoder_1_position > (AUDIO_EFFECT_COUNT - 1) * 2)
    {
        encoder_state->encoder_1_position = (AUDIO_EFFECT_COUNT - 1) * 2;
    }

    audio_effect_id_t selected_effect =
        (audio_effect_id_t)(encoder_state->encoder_1_position / 2);
    if (selected_effect != params->current_effect)
    {
        params->current_effect = selected_effect;
        effect = get_effect(params);
        encoder_state->encoder_2_position = effect->get_encoder_value(params);
    }

    effect = get_effect(params);
    if (encoder_state->encoder_2_position < effect->encoder_min)
    {
        encoder_state->encoder_2_position = effect->encoder_min;
    }
    else if (encoder_state->encoder_2_position > effect->encoder_max)
    {
        encoder_state->encoder_2_position = effect->encoder_max;
    }

    if (encoder_state->encoder_2_position != effect->get_encoder_value(params))
    {
        effect->set_parameter(params, (int16_t)encoder_state->encoder_2_position);
    }
    return true;
}

bool ui_controller_get_display(const audio_settings_t *params, ui_effect_display_t *display)
{
    if (display == NULL)
    {
        return false;
    }

    const effect_ui_descriptor_t *effect = get_effect(params);
    if (effect == NULL)
    {
        return false;
    }

    (void)snprintf(display->effect_name, sizeof(display->effect_name), "%s", effect->name);
    (void)snprintf(display->parameter_label, sizeof(display->parameter_label), "%s", effect->parameter_label);
    effect->format_value(params, display->parameter_value,sizeof(display->parameter_value));
    
    display->parameter_value_raw = effect->get_bar_value(params);
    display->parameter_min = effect->bar_min;
    display->parameter_max = effect->bar_max;
    display->muted = params->is_muted;
    
    return true;
}
