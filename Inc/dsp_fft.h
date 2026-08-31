#ifndef DSP_FFT_H_
#define DSP_FFT_H_

#include <stdint.h>
#include <stdbool.h>
#include "arm_math.h"

#define FFT_SIZE 256

// Visualisation data shared between DSP processing and OLED task
extern volatile float32_t global_fft_mags[16];
extern volatile int32_t global_vu_level;
extern volatile int32_t fft_init_status;

/**
 * @brief  Initializes bit reversal tables and twiddle factors for local FFT computation.
 * @param  None
 * @retval None
 */
void dsp_fft_init(void);

/**
 * @brief  Calculates a 256-point complex FFT on mono input samples and aggregates
 *         the output into 16 logarithmic frequency bins for OLED visualizer.
 * @param  pInMono: Pointer to 128 new 24-bit input samples.
 * @retval None
 */
void dsp_fft_process(const int32_t *pInMono);

#endif /* DSP_FFT_H_ */
