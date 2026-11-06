#include "dsp_fft.h"
#include <string.h>
#include <math.h>

// Radix-2 FFT internal structures
static uint16_t bit_rev_256[FFT_SIZE];
static float32_t twiddle_cos_256[FFT_SIZE / 2];
static float32_t twiddle_sin_256[FFT_SIZE / 2];
static uint8_t my_fft_initialized = 0;

void dsp_fft_init(void)
{
    // 1. Generate bit-reversal table for 256 points
    for (int i = 0; i < FFT_SIZE; i++)
    {
        int rev = 0;
        int temp = i;
        for (int bit = 0; bit < 8; bit++)
        {
            rev = (rev << 1) | (temp & 1);
            temp >>= 1;
        }
        bit_rev_256[i] = rev;
    }

    // 2. Generate twiddle factor table (cos and sin)
    float32_t angle;
    for (int i = 0; i < FFT_SIZE / 2; i++)
    {
        angle = -2.0f * 3.141592653589793f * (float32_t)i / (float32_t)FFT_SIZE;
        twiddle_cos_256[i] = cosf(angle);
        twiddle_sin_256[i] = sinf(angle);
    }
    
    my_fft_initialized = 1;
}

static void my_fft_compute(const float32_t *input, float32_t *output_mag)
{
    if (!my_fft_initialized)
    {
        dsp_fft_init();
    }

    // Align variables to 8-byte boundary for optimal FPU memory access
    static float32_t re[FFT_SIZE] __attribute__((aligned(8)));
    static float32_t im[FFT_SIZE] __attribute__((aligned(8)));

    // Re-order input values using precomputed bit-reversal table
    for (int i = 0; i < FFT_SIZE; i++)
    {
        re[bit_rev_256[i]] = input[i];
        im[i] = 0.0f;
    }

    // Run FFT Butterfly stages (Radix-2 DIT FFT)
    for (int len = 2; len <= FFT_SIZE; len <<= 1)
    {
        int half_len = len >> 1;
        int step = FFT_SIZE / len;
        for (int i = 0; i < FFT_SIZE; i += len)
        {
            for (int j = 0; j < half_len; j++)
            {
                int u = i + j;
                int v = i + j + half_len;
                
                float32_t t_cos = twiddle_cos_256[j * step];
                float32_t t_sin = twiddle_sin_256[j * step];
                
                float32_t t_re = re[v] * t_cos - im[v] * t_sin;
                float32_t t_im = re[v] * t_sin + im[v] * t_cos;
                
                re[v] = re[u] - t_re;
                im[v] = im[u] - t_im;
                re[u] += t_re;
                im[u] += t_im;
            }
        }
    }

    // Compute magnitude for the first 128 bins (DC to fs/2)
    output_mag[0] = re[0] < 0.0f ? -re[0] : re[0];
    for (int k = 1; k < 128; k++)
    {
        float32_t r = re[k];
        float32_t i = im[k];
        output_mag[k] = sqrtf(r * r + i * i);
    }
}

void dsp_fft_process(const int32_t *pInMono, dsp_spectrum_t *spectrum)
{
    if (pInMono == NULL || spectrum == NULL)
    {
        return;
    }

    static float32_t fft_in[256] __attribute__((aligned(8))) = {0};
    static float32_t mag[128] __attribute__((aligned(8)));

    // Shift buffer by 128 samples (sliding window)
    memmove(&fft_in[0], &fft_in[128], 128 * sizeof(float32_t));
    
    // Normalize and copy new 128 mono samples into the buffer
    for (int j = 0; j < 128; j++)
    {
        // scale from 24-bit range (max 8388608) to float [-1.0f, 1.0f]
        fft_in[128 + j] = (float32_t)pInMono[j] / 8388608.0f;
    }

    //Calculate FFT magnitude spectrum
    my_fft_compute(fft_in, mag);

    //Logarithmic grouping of 128 bins into 16 OLED spectrum bars
    const uint8_t bin_groups[16][2] = {
        {1, 1},   // 125 Hz
        {2, 2},   // 250 Hz
        {3, 3},   // 375 Hz
        {4, 4},   // 500 Hz
        {5, 6},   // 625 - 750 Hz
        {7, 8},   // 875 - 1000 Hz
        {9, 11},  // 1.1k - 1.3k
        {12, 15}, // 1.5k - 1.8k
        {16, 20}, // 2.0k - 2.5k
        {21, 26}, // 2.6k - 3.2k
        {27, 34}, // 3.3k - 4.2k
        {35, 44}, // 4.4k - 5.5k
        {45, 56}, // 5.6k - 7.0k
        {57, 72}, // 7.1k - 9.0k
        {73, 93}, // 9.1k - 11.6k
        {94, 120} // 11.7k - 15.0k
    };

    float32_t temp_mags[16] = {0};
    for (int bar = 0; bar < 16; bar++)
    {
        float32_t sum = 0;
        int count = 0;
        for (int bin = bin_groups[bar][0]; bin <= bin_groups[bar][1]; bin++)
        {
            sum += mag[bin];
            count++;
        }
        temp_mags[bar] = sum / count;
    }

    for (int bar = 0; bar < 16; bar++)
    {
        spectrum->band_magnitudes[bar] = temp_mags[bar];
    }
}