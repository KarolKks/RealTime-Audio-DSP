#include "audio_model.h"
#include <stddef.h>

bool audio_settings_get_defaults(audio_settings_t *settings)
{
    if (settings == NULL)
    {
        return false;
    }

    *settings = (audio_settings_t) {
        .current_effect = AUDIO_EFFECT_CLEAN,
        .volume_gain = 4,
        .is_muted = false,
        .ringmod_freq = 250,
        .ringmod_wave_shape = 0,
        .dist_gain = 5,
        .dist_type = 0,
        .delay_time_ms = 150,
        .delay_feedback_mode = 1,
        .pitch_ratio_percent = 100,
        .tremolo_freq = 5,
        .auto_pan_mode = 0,
        .filter_fc_idx = 10,
        .filter_type = 0,
        .bc_downsample = 4,
        .bc_bit_depth = 0,
        .gate_threshold_db = -45,
        .gate_release_mode = 1
    };
    return true;
}
