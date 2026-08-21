#ifndef DSP_EFFECTS_H_
#define DSP_EFFECTS_H_

#include "stm32l4xx_hal.h"

extern volatile int16_t volumeGain;
extern volatile int16_t currentEffect;
extern volatile int16_t ringmod_freq;
extern volatile uint8_t ringmod_wave_shape;
extern volatile int16_t dist_gain;
extern volatile uint8_t dist_type;
extern volatile int16_t delay_time_ms;
extern volatile uint8_t delay_feedback_mode;
extern volatile int16_t pitch_enc2_val;
extern volatile int32_t pitch_S_q16;
extern volatile uint8_t isMuted;

extern volatile int16_t tremolo_freq;
extern volatile uint8_t auto_pan_mode;
extern volatile int16_t filter_fc_idx;
extern volatile uint8_t filter_type;
extern volatile int16_t bc_downsample;
extern volatile uint8_t bc_bit_depth;
extern volatile int16_t gate_threshold_db;
extern volatile uint8_t gate_release_mode;
extern volatile int32_t adjustable_gate_threshold;

extern volatile float32_t bq_b0, bq_b1, bq_b2, bq_a1, bq_a2;

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