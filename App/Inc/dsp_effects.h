#ifndef DSP_EFFECTS_H_
#define DSP_EFFECTS_H_

#include <stdint.h>
#include <stdbool.h>
#include "arm_math.h"
#include "audio_model.h"

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

/**
 * @brief  Initializes state variables and buffers for all DSP effects.
 * @param  None
 * @retval None
 */
void dsp_effects_init(void);

/**
 * @brief Applies a complete parameter snapshot in the DSP-owning task.
 * @param settings Application audio settings to apply.
 * @retval true if the settings are valid and were applied.
 */
bool dsp_effects_apply_params(const audio_settings_t *settings);

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