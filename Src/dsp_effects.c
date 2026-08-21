#include "dsp_effects.h"
#include <string.h>
#include <math.h>

/* Parameter definitions and default values */
volatile int16_t volumeGain = 4;
volatile int16_t currentEffect = 0;
volatile int16_t ringmod_freq = 250;      // 50 Hz to 1500 Hz (default 250 Hz)
volatile uint8_t ringmod_wave_shape = 0;   // 0 = sine, 1 = square
volatile int16_t dist_gain = 5;            // Saturation level 1 to 25 (default 5)
volatile uint8_t dist_type = 0;            // 0 = Soft-Clipping, 1 = Hard-Clipping
volatile int16_t delay_time_ms = 150;     // Delay 50 ms to 600 ms (default 150 ms)
volatile uint8_t delay_feedback_mode = 1;  // 0 = single echo, 1 = multiple echo (feedback)
volatile int16_t pitch_enc2_val = 10;      // Encoder value for Pitch Shifter (default 10 = 1.0x)
volatile int32_t pitch_S_q16 = 65536;      // Pitch factor in Q16 (65536 = 1.0x)
volatile uint8_t isMuted = 0;              // Mute flag (0 = unmuted, 1 = muted)

volatile int16_t tremolo_freq = 5;            // 1 Hz to 20 Hz (default 5 Hz)
volatile uint8_t auto_pan_mode = 0;           // 0 = Tremolo, 1 = Auto-Pan
volatile int16_t filter_fc_idx = 10;          // Cutoff index 0 to 40 (default 10 = ~2100 Hz)
volatile uint8_t filter_type = 0;             // 0 = Low-Pass, 1 = High-Pass
volatile float32_t bq_b0 = 1.0f, bq_b1 = 0.0f, bq_b2 = 0.0f, bq_a1 = 0.0f, bq_a2 = 0.0f;
volatile int16_t bc_downsample = 4;           // Downsampling factor 1 to 30 (default 4)
volatile uint8_t bc_bit_depth = 0;            // 0 = 8-bit, 1 = 4-bit
volatile int16_t gate_threshold_db = -45;     // Threshold -60 dB to -20 dB (default -45 dB)
volatile uint8_t gate_release_mode = 1;       // 0 = Fast, 1 = Slow
volatile int32_t adjustable_gate_threshold = 26500; // Linear threshold value

/* Sine Look-Up Table (256 entries for half cycle) */
static const int16_t sine_table[256] = {
    0, 402, 804, 1206, 1607, 2009, 2410, 2811, 3211, 3611, 4011, 4410, 4807, 5205, 5601, 5997,
    6392, 6786, 7179, 7571, 7961, 8351, 8739, 9126, 9511, 9895, 10278, 10659, 11038, 11416, 11792, 12166,
    12539, 12909, 13278, 13645, 14009, 14372, 14732, 15090, 15446, 15799, 16150, 16499, 16845, 17189, 17530, 17868,
    18204, 18537, 18867, 19194, 19519, 19840, 20159, 20474, 20787, 21096, 21402, 21705, 22004, 22301, 22593, 22883,
    23169, 23452, 23731, 24007, 24279, 24547, 24811, 25072, 25329, 25582, 25831, 26077, 26318, 26556, 26789, 27019,
    27244, 27466, 27683, 27896, 28105, 28309, 28510, 28706, 28897, 29085, 29268, 29446, 29621, 29790, 29955, 30116,
    30272, 30424, 30571, 30713, 30851, 30984, 31113, 31236, 31356, 31470, 31580, 31684, 31785, 31880, 31970, 32056,
    32137, 32213, 32284, 32350, 32412, 32468, 32520, 32567, 32609, 32646, 32678, 32705, 32727, 32744, 32757, 32764,
    32767, 32764, 32757, 32744, 32727, 32705, 32678, 32646, 32609, 32567, 32520, 32468, 32412, 32350, 32284, 32213,
    32137, 32056, 31970, 31880, 31785, 31684, 31580, 31470, 31356, 31236, 31113, 30984, 30851, 30713, 30571, 30424,
    30272, 30116, 29955, 29790, 29621, 29446, 29268, 29085, 28897, 28706, 28510, 28309, 28105, 27896, 27683, 27466,
    27244, 27019, 26789, 26556, 26318, 26077, 25831, 25582, 25329, 25072, 24811, 24547, 24279, 24007, 23731, 23452,
    23169, 22883, 22593, 22301, 22004, 21705, 21402, 21096, 20787, 20474, 20159, 19840, 19519, 19194, 18867, 18537,
    18204, 17868, 17530, 17189, 16845, 16499, 16150, 15799, 15446, 15090, 14732, 14372, 14009, 13645, 13278, 12909,
    12539, 12166, 11792, 11416, 11038, 10659, 10278, 9895, 9511, 9126, 8739, 8351, 7961, 7571, 7179, 6786,
    6392, 5997, 5601, 5205, 4807, 4410, 4011, 3611, 3211, 2811, 2410, 2009, 1607, 1206, 804, 402
};

/* Effect state variables */
static uint32_t ringmod_phase = 0;

#define DELAY_BUFFER_SIZE 20000
static int16_t delayBuffer[DELAY_BUFFER_SIZE];
static uint16_t delayWriteIdx = 0;

#define PITCH_BUF_SIZE 8192
static int16_t pitchBuffer[PITCH_BUF_SIZE];
static uint16_t pitchWriteIdx = 0;
static int32_t pitchPhase = 0;

/* DC Block and Noise Gate state */
static int32_t dc_block_state = 0;
static int32_t prev_in = 0;
static int32_t gate_envelope = 0;

/* LFO, IIR Filters and Bitcrusher state */
static uint32_t tremolo_phase = 0;
static float32_t bq_x1 = 0.0f, bq_x2 = 0.0f, bq_y1 = 0.0f, bq_y2 = 0.0f;
static float32_t bq_x1_2 = 0.0f, bq_x2_2 = 0.0f, bq_y1_2 = 0.0f, bq_y2_2 = 0.0f;
static int32_t bc_held_sample = 0;
static uint16_t bc_counter = 0;

void dsp_effects_init(void)
{
    memset(delayBuffer, 0, sizeof(delayBuffer));
    delayWriteIdx = 0;

    memset(pitchBuffer, 0, sizeof(pitchBuffer));
    pitchWriteIdx = 0;
    pitchPhase = 0;

    ringmod_phase = 0;
    tremolo_phase = 0;

    dc_block_state = 0;
    prev_in = 0;
    gate_envelope = 0;

    bq_x1 = 0.0f; bq_x2 = 0.0f; bq_y1 = 0.0f; bq_y2 = 0.0f;
    bq_x1_2 = 0.0f; bq_x2_2 = 0.0f; bq_y1_2 = 0.0f; bq_y2_2 = 0.0f;

    bc_held_sample = 0;
    bc_counter = 0;

    /* Initialize filter and gate parameters */
    dsp_effects_update_params();
}

void dsp_effects_update_params(void)
{
    /*Recalculate biquad filter coefficients */
    float32_t fs = 32170.0f;
    float32_t fc = 100.0f * powf(1.127f, (float32_t)filter_fc_idx);
    if (fc > 15000.0f) fc = 15000.0f;
  
    float32_t w0 = 2.0f * 3.14159265f * fc / fs;
    float32_t alpha = sinf(w0) / (2.0f * 0.707f); // Q = 0.707
    float32_t cos_w0 = cosf(w0);
  
    float32_t b0, b1, b2, a0, a1, a2;
    if (filter_type == 0) // Low-Pass
    {
        b0 = (1.0f - cos_w0) / 2.0f;
        b1 = 1.0f - cos_w0;
        b2 = (1.0f - cos_w0) / 2.0f;
        a0 = 1.0f + alpha;
        a1 = -2.0f * cos_w0;
        a2 = 1.0f - alpha;
    }
    else // High-Pass
    {
        b0 = (1.0f + cos_w0) / 2.0f;
        b1 = -(1.0f + cos_w0);
        b2 = (1.0f + cos_w0) / 2.0f;
        a0 = 1.0f + alpha;
        a1 = -2.0f * cos_w0;
        a2 = 1.0f - alpha;
    }
  
    bq_b0 = b0 / a0;
    bq_b1 = b1 / a0;
    bq_b2 = b2 / a0;
    bq_a1 = a1 / a0;
    bq_a2 = a2 / a0;

    /* 2. Recalculate noise gate threshold */
    float32_t ratio = powf(10.0f, (float32_t)gate_threshold_db / 20.0f);
    adjustable_gate_threshold = (int32_t)(8388608.0f * ratio);
}

void dsp_process_sample(int32_t input_sample, int32_t *output_l, int32_t *output_r)
{
    int32_t sample = input_sample;

    /* DC Block Filter (constant offset removal) */
    int32_t current_in = sample;
    dc_block_state = current_in - prev_in + (int32_t)(((int64_t)dc_block_state * 32737) >> 15);
    prev_in = current_in;
    sample = dc_block_state;

    /* Soft Noise Gate / Expander */
    int32_t abs_sample = sample;
    if (abs_sample < 0) abs_sample = -abs_sample;

    if (abs_sample > gate_envelope) {
        gate_envelope = abs_sample; // fast attack
    } else {
        gate_envelope = (int32_t)(((int64_t)gate_envelope * 32737) >> 15); // slow release
    }

    int32_t gate_gain = 32768; // Q15 format (1.0 = 32768)
    if (currentEffect == 8) // Adjustable Noise Gate
    {
        int32_t th = adjustable_gate_threshold;
        if (gate_envelope < th) {
            gate_gain = 0;
        } else if (gate_envelope < th * 2) {
            gate_gain = (int32_t)(((int64_t)(gate_envelope - th) * 32768) / th);
        }
    }
    else // Standard Noise Gate
    {
        if (gate_envelope < 60000) {
            gate_gain = 0;
        } else if (gate_envelope < 160000) {
            gate_gain = (int32_t)(((int64_t)(gate_envelope - 60000) * 10737) >> 15);
        }
    }

    sample = (int32_t)(((int64_t)sample * gate_gain) >> 15);

    /* 1. Apply Gain or Mute */
    int64_t processed = 0;
    if (!isMuted)
    {
        processed = (int64_t)sample * volumeGain;
    }

    /* Panning gains (L/R) for Auto-Pan */
    float32_t pan_gain_l = 1.0f;
    float32_t pan_gain_r = 1.0f;
    uint8_t has_pan = 0;

    /* 2. Process active DSP effect */
    switch (currentEffect)
    {
        case 1: // Robot (Ring Modulator) - modulation with sine or square wave
        {
            uint32_t phase_step = (uint32_t)ringmod_freq * 133508;
            ringmod_phase += phase_step;

            int32_t mod_val = 0;
            if (ringmod_wave_shape == 0) // Sine from LUT
            {
                uint32_t idx = ringmod_phase >> 23; 
                if (idx < 256) {
                    mod_val = (int32_t)sine_table[idx] << 16; // scale to Q31
                } else {
                    mod_val = -(int32_t)sine_table[idx - 256] << 16;
                }
            }
            else // Square wave
            {
                mod_val = (ringmod_phase < 0x80000000) ? 2147483647 : -2147483648;
            }

            processed = (processed * mod_val) >> 31;
            break;
        }

        case 2: // Distortion / Overdrive
        {
            int64_t dist_signal = processed * dist_gain;

            if (dist_type == 1) // Hard-Clipping
            {
                if (dist_signal > 1073741823LL) dist_signal = 1073741823LL;
                else if (dist_signal < -1073741824LL) dist_signal = -1073741824LL;
            }
            else // Soft-Clipping
            {
                if (dist_signal > 1431655765LL) {
                    dist_signal = 1073741823LL + ((dist_signal - 1431655765LL) >> 2);
                } else if (dist_signal < -1431655765LL) {
                    dist_signal = -1073741823LL + ((dist_signal + 1431655765LL) >> 2);
                } else if (dist_signal > 715827882LL) {
                    dist_signal = 715827882LL + ((dist_signal - 715827882LL) >> 1);
                } else if (dist_signal < -715827882LL) {
                    dist_signal = -715827882LL + ((dist_signal + 715827882LL) >> 1);
                }
            }
            processed = dist_signal;
            break;
        }

        case 3: // Echo (Delay)
        {
            int32_t delay_samples = (delay_time_ms * 32170) / 1000;
            if (delay_samples >= DELAY_BUFFER_SIZE) delay_samples = DELAY_BUFFER_SIZE - 1;
            else if (delay_samples < 1) delay_samples = 1;

            int32_t readIdx = (int32_t)delayWriteIdx - delay_samples;
            if (readIdx < 0) readIdx += DELAY_BUFFER_SIZE;
            int32_t delayedSample = ((int32_t)delayBuffer[readIdx]) << 16;

            int64_t mixed = processed + (delayedSample >> 1);

            if (mixed > 2147483647LL) mixed = 2147483647LL;
            else if (mixed < -2147483648LL) mixed = -2147483648LL;

            if (delay_feedback_mode == 0) // single echo
            {
                delayBuffer[delayWriteIdx] = (int16_t)(processed >> 16);
            }
            else // feedback echo
            {
                delayBuffer[delayWriteIdx] = (int16_t)(mixed >> 16);
            }

            delayWriteIdx++;
            if (delayWriteIdx >= DELAY_BUFFER_SIZE) delayWriteIdx = 0;

            processed = mixed;
            break;
        }

        case 4: // Pitch Shifter
        {
            pitchBuffer[pitchWriteIdx] = (int16_t)(processed >> 16);

            pitchPhase += (65536 - pitch_S_q16);
            if (pitchPhase >= (4096 << 16)) pitchPhase -= (4096 << 16);
            else if (pitchPhase < 0) pitchPhase += (4096 << 16);

            int32_t p1 = pitchPhase >> 16;
            int32_t p2 = p1 + 2048;
            if (p2 >= 4096) p2 -= 4096;

            int32_t r1 = ((int32_t)pitchWriteIdx - p1 - 10) & (PITCH_BUF_SIZE - 1);
            int32_t r2 = ((int32_t)pitchWriteIdx - p2 - 10) & (PITCH_BUF_SIZE - 1);

            int32_t s1 = pitchBuffer[r1];
            int32_t s2 = pitchBuffer[r2];

            int32_t w1 = 0;
            if (p1 < 2048) w1 = p1 << 4;
            else w1 = (4096 - p1) << 4;
            int32_t w2 = 32768 - w1;

            int64_t mixed_val = ((int64_t)s1 * w1 + (int64_t)s2 * w2) >> 15;
            processed = mixed_val << 16;

            pitchWriteIdx = (pitchWriteIdx + 1) & (PITCH_BUF_SIZE - 1);
            break;
        }

        case 5: // Tremolo / Auto-Pan
        {
            uint32_t phase_step = (uint32_t)tremolo_freq * 133508;
            tremolo_phase += phase_step;

            uint32_t idx = tremolo_phase >> 23;
            float32_t lfo = 0.0f;
            if (idx < 256) {
                lfo = (float32_t)sine_table[idx] / 32768.0f;
            } else {
                lfo = -(float32_t)sine_table[idx - 256] / 32768.0f;
            }

            if (auto_pan_mode == 0) // Tremolo
            {
                pan_gain_l = 0.6f + 0.4f * lfo;
                pan_gain_r = pan_gain_l;
            }
            else // Auto-Pan
            {
                pan_gain_l = 0.5f + 0.5f * lfo;
                pan_gain_r = 0.5f - 0.5f * lfo;
            }
            has_pan = 1;
            break;
        }

        case 6: // IIR Biquad 4th Order Cascade (24 dB/oct)
        {
            float32_t x0 = (float32_t)processed;
          
            // Cascade 1
            float32_t y0 = bq_b0 * x0 + bq_b1 * bq_x1 + bq_b2 * bq_x2 - bq_a1 * bq_y1 - bq_a2 * bq_y2;
            bq_x2 = bq_x1;
            bq_x1 = x0;
            bq_y2 = bq_y1;
            bq_y1 = y0;
          
            // Cascade 2
            float32_t y0_2 = bq_b0 * y0 + bq_b1 * bq_x1_2 + bq_b2 * bq_x2_2 - bq_a1 * bq_y1_2 - bq_a2 * bq_y2_2;
            bq_x2_2 = bq_x1_2;
            bq_x1_2 = y0;
            bq_y2_2 = bq_y1_2;
            bq_y1_2 = y0_2;
          
            processed = (int64_t)y0_2;
            break;
        }

        case 7: // Bitcrusher
        {
            bc_counter++;
            if (bc_counter >= bc_downsample)
            {
                bc_counter = 0;
                int32_t val = (int32_t)processed;
                int32_t bits = (bc_bit_depth == 0) ? 8 : 4;
                int32_t shift = 32 - bits;
                val = (val >> shift) << shift;
                bc_held_sample = val;
            }
            processed = bc_held_sample;
            break;
        }

        default:
            break;
    }

    /* 3. Hard Limiter - protects against digital overflow clipping */
    if (processed > 2147483647LL)  processed = 2147483647LL;
    if (processed < -2147483648LL) processed = -2147483648LL;

    if (has_pan)
    {
        *output_l = (int32_t)(processed * pan_gain_l);
        *output_r = (int32_t)(processed * pan_gain_r);
    }
    else
    {
        *output_l = (int32_t)processed;
        *output_r = (int32_t)processed;
    }
}
