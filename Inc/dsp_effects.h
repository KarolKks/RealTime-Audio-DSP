#ifndef DSP_EFFECTS_H_
#define DSP_EFFECTS_H_

#include <stdint.h>
#include <stdbool.h>
#include "arm_math.h"

// Effect IDs enumeration
typedef enum {
    DSP_EFFECT_CLEAN = 0,
    DSP_EFFECT_ROBOT,
    DSP_EFFECT_DISTORTION,
    DSP_EFFECT_DELAY,
    DSP_EFFECT_PITCH,
    DSP_EFFECT_TREMOLO_PAN,
    DSP_EFFECT_FILTER,
    DSP_EFFECT_BITCRUSHER,
    DSP_EFFECT_GATE,
    DSP_EFFECT_COUNT
} dsp_effect_id_t;

// DSP System Constants
#define DSP_SAMPLE_RATE_HZ          32170.0f
#define DSP_SAMPLE_RATE_INT         32170
#define DSP_MAX_INT24_F             8388608.0f
#define DSP_MAX_INT24               8388608
#define DSP_Q15_ONE                 32768
#define DSP_Q16_ONE                 65536
#define DSP_INT32_MAX_LIMIT         2147483647LL
#define DSP_INT32_MIN_LIMIT        -2147483648LL
#define DSP_LFO_PHASE_STEP_FACTOR   133508UL
#define DSP_DELAY_BUFFER_SIZE       20000
#define DSP_PITCH_BUFFER_SIZE       8192

// BiQuad filter coefficients structure
typedef struct {
    float32_t b0;
    float32_t b1;
    float32_t b2;
    float32_t a1;
    float32_t a2;
} dsp_biquad_coeffs_t;

// Unified DSP Parameters structure
typedef struct {
    dsp_effect_id_t current_effect;
    int16_t volume_gain;
    bool is_muted;

    // Robot (Ring Modulator)
    int16_t ringmod_freq;
    uint8_t ringmod_wave_shape;

    // Distortion
    int16_t dist_gain;
    uint8_t dist_type;

    // Delay / Echo
    int16_t delay_time_ms;
    uint8_t delay_feedback_mode;

    // Pitch Shifter
    int16_t pitch_enc2_val;
    int32_t pitch_S_q16;

    // Tremolo / Auto-Pan
    int16_t tremolo_freq;
    uint8_t auto_pan_mode;

    // BiQuad IIR Filter
    int16_t filter_fc_idx;
    uint8_t filter_type;

    // Bitcrusher
    int16_t bc_downsample;
    uint8_t bc_bit_depth;

    // Noise Gate
    int16_t gate_threshold_db;
    uint8_t gate_release_mode;
} dsp_params_t;

// Global runtime settings structure
extern volatile dsp_params_t g_dsp_params;

/**
 * @brief  Initializes state variables and buffers for all DSP effects.
 * @param  None
 * @retval None
 */
void dsp_effects_init(void);

/**
 * @brief  Recalculates filter coefficients and noise gate thresholds.
 *         Should be called in the background when parameters change.
 * @param  None
 * @retval None
 */
void dsp_effects_update_params(void);

/**
 * @brief  Processes a single mono input sample through the active DSP effect
 *         and outputs stereo samples (left and right).
 * @param  input_sample: 24-bit input audio sample.
 * @param  output_l: Pointer to store the processed left channel sample.
 * @param  output_r: Pointer to store the processed right channel sample.
 * @retval None
 */
void dsp_process_sample(int32_t input_sample, int32_t *output_l, int32_t *output_r);

#endif /* DSP_EFFECTS_H_ */