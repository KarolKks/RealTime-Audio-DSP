#ifndef DSP_FFT_H_
#define DSP_FFT_H_

#include <stdint.h>
#include "arm_math.h"

#define FFT_SIZE 256

typedef struct
{
    float32_t band_magnitudes[16];
} dsp_spectrum_t;

/**
 * @brief  Initializes bit reversal tables and twiddle factors for local FFT computation.
 * @param  None
 * @retval None
 */
void dsp_fft_init(void);

/**
 * @brief  Calculates a 256-point complex FFT on mono input samples and aggregates
 *         the output into 16 logarithmic frequency bands.
 * @param  pInMono: Pointer to 128 new 24-bit input samples.
 * @param  spectrum: Destination for the calculated frequency-band magnitudes.
 * @retval None
 */
void dsp_fft_process(const int32_t *pInMono, dsp_spectrum_t *spectrum);

#endif /* DSP_FFT_H_ */
