#ifndef AUDIO_MODEL_H_
#define AUDIO_MODEL_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    AUDIO_EFFECT_CLEAN = 0,
    AUDIO_EFFECT_ROBOT,
    AUDIO_EFFECT_DISTORTION,
    AUDIO_EFFECT_DELAY,
    AUDIO_EFFECT_PITCH,
    AUDIO_EFFECT_TREMOLO_PAN,
    AUDIO_EFFECT_FILTER,
    AUDIO_EFFECT_BITCRUSHER,
    AUDIO_EFFECT_GATE,
    AUDIO_EFFECT_COUNT
} audio_effect_id_t;

typedef struct
{
    audio_effect_id_t current_effect;
    int16_t volume_gain;
    bool is_muted;
    int16_t ringmod_freq;
    uint8_t ringmod_wave_shape;
    int16_t dist_gain;
    uint8_t dist_type;
    int16_t delay_time_ms;
    uint8_t delay_feedback_mode;
    uint8_t pitch_ratio_percent;
    int16_t tremolo_freq;
    uint8_t auto_pan_mode;
    int16_t filter_fc_idx;
    uint8_t filter_type;
    int16_t bc_downsample;
    uint8_t bc_bit_depth;
    int16_t gate_threshold_db;
    uint8_t gate_release_mode;
} audio_settings_t;

bool audio_settings_get_defaults(audio_settings_t *settings);

#endif /* AUDIO_MODEL_H_ */
