#include "ui_task.h"
#include "hw_encoders.h"
#include "hw_oled_sh1106.h"
#include "dsp_effects.h"
#include "dsp_fft.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// Effect UI Descriptor Structure (Table-Driven Metadata & Callbacks)
typedef struct {
    const char *name;              // Display name (e.g. "ROBOT", "DIST")
    const char *param_label;       // Parameter label (e.g. "FREQ", "SAT")
    int16_t enc_min;               // Encoder minimum limit
    int16_t enc_max;               // Encoder maximum limit
    int32_t bar_min;               // Progress bar minimum value
    int32_t bar_max;               // Progress bar maximum value

    void    (*on_btn2_press)(void);       // Button 2 action (reset/toggle)
    int16_t (*get_encoder_val)(void);     // Retrieve current parameter mapped to encoder units
    void    (*set_param)(int16_t val);    // Apply encoder value to DSP parameter
    int32_t (*get_bar_val)(void);         // Get raw value for progress bar calculation
    void    (*format_val_str)(char *buf); // Format value with units for OLED display
} effect_ui_desc_t;

// Effect 0: Clean / Bypass
static void clean_btn2_press(void) { g_dsp_params.volume_gain = 4; }
static int16_t clean_get_enc(void) { return g_dsp_params.volume_gain; }
static void clean_set_param(int16_t val) { g_dsp_params.volume_gain = val; }
static int32_t clean_get_bar(void) { return g_dsp_params.volume_gain; }
static void clean_format_str(char *buf) { sprintf(buf, "%dx", g_dsp_params.volume_gain); }

// Effect 1: Robot (Ring Modulator)
static void robot_btn2_press(void) { g_dsp_params.ringmod_freq = 250; g_dsp_params.ringmod_wave_shape = !g_dsp_params.ringmod_wave_shape; }
static int16_t robot_get_enc(void) { return (g_dsp_params.ringmod_freq - 50) / 10; }
static void robot_set_param(int16_t val) { g_dsp_params.ringmod_freq = 50 + val * 10; }
static int32_t robot_get_bar(void) { return g_dsp_params.ringmod_freq; }
static void robot_format_str(char *buf) { sprintf(buf, "%dHz", g_dsp_params.ringmod_freq); }

// Effect 2: Distortion
static void dist_btn2_press(void) { g_dsp_params.dist_gain = 5; g_dsp_params.dist_type = !g_dsp_params.dist_type; }
static int16_t dist_get_enc(void) { return g_dsp_params.dist_gain; }
static void dist_set_param(int16_t val) { g_dsp_params.dist_gain = val; }
static int32_t dist_get_bar(void) { return g_dsp_params.dist_gain; }
static void dist_format_str(char *buf) { sprintf(buf, "%d", g_dsp_params.dist_gain); }

// Effect 3: Echo (Delay)
static void delay_btn2_press(void) { g_dsp_params.delay_time_ms = 150; g_dsp_params.delay_feedback_mode = !g_dsp_params.delay_feedback_mode; }
static int16_t delay_get_enc(void) { return (g_dsp_params.delay_time_ms - 50) / 10; }
static void delay_set_param(int16_t val) { g_dsp_params.delay_time_ms = 50 + val * 10; }
static int32_t delay_get_bar(void) { return g_dsp_params.delay_time_ms; }
static void delay_format_str(char *buf) { sprintf(buf, "%dms", g_dsp_params.delay_time_ms); }

// Effect 4: Pitch Shifter
static void pitch_btn2_press(void) { g_dsp_params.pitch_enc2_val = 10; g_dsp_params.pitch_S_q16 = DSP_Q16_ONE; }
static int16_t pitch_get_enc(void) { return g_dsp_params.pitch_enc2_val; }
static void pitch_set_param(int16_t val) {
    g_dsp_params.pitch_enc2_val = val;
    if (val <= 10) g_dsp_params.pitch_S_q16 = 32768 + val * 3276;
    else g_dsp_params.pitch_S_q16 = 65536 + (val - 10) * 3276;
}
static int32_t pitch_get_bar(void) { return g_dsp_params.pitch_enc2_val; }
static void pitch_format_str(char *buf) {
    sprintf(buf, "%d.%02dx", (int)(g_dsp_params.pitch_S_q16 >> 16), (int)(((g_dsp_params.pitch_S_q16 & 0xFFFF) * 100) >> 16));
}

// Effect 5: Tremolo / Auto-Pan
static void tremolo_btn2_press(void) { g_dsp_params.tremolo_freq = 5; g_dsp_params.auto_pan_mode = !g_dsp_params.auto_pan_mode; }
static int16_t tremolo_get_enc(void) { return g_dsp_params.tremolo_freq; }
static void tremolo_set_param(int16_t val) { g_dsp_params.tremolo_freq = val; }
static int32_t tremolo_get_bar(void) { return g_dsp_params.tremolo_freq; }
static void tremolo_format_str(char *buf) { sprintf(buf, "%dHz", g_dsp_params.tremolo_freq); }

// Effect 6: IIR BiQuad Filter
static void filter_btn2_press(void) { g_dsp_params.filter_fc_idx = 10; g_dsp_params.filter_type = !g_dsp_params.filter_type; dsp_effects_update_params(); }
static int16_t filter_get_enc(void) { return g_dsp_params.filter_fc_idx; }
static void filter_set_param(int16_t val) { g_dsp_params.filter_fc_idx = val; dsp_effects_update_params(); }
static int32_t filter_get_bar(void) { return g_dsp_params.filter_fc_idx; }
static void filter_format_str(char *buf) {
    int32_t disp_fc = (int32_t)(100.0f * powf(1.127f, (float32_t)g_dsp_params.filter_fc_idx));
    if (disp_fc >= 1000) {
        sprintf(buf, "%d.%dkHz", (int)(disp_fc / 1000), (int)((disp_fc % 1000) / 100));
    } else {
        sprintf(buf, "%dHz", (int)disp_fc);
    }
}

// Effect 7: Bitcrusher
static void crusher_btn2_press(void) { g_dsp_params.bc_downsample = 4; g_dsp_params.bc_bit_depth = !g_dsp_params.bc_bit_depth; }
static int16_t crusher_get_enc(void) { return g_dsp_params.bc_downsample; }
static void crusher_set_param(int16_t val) { g_dsp_params.bc_downsample = val; }
static int32_t crusher_get_bar(void) { return g_dsp_params.bc_downsample; }
static void crusher_format_str(char *buf) { sprintf(buf, "%dx", g_dsp_params.bc_downsample); }

// Effect 8: Noise Gate
static void gate_btn2_press(void) { g_dsp_params.gate_threshold_db = -45; g_dsp_params.gate_release_mode = !g_dsp_params.gate_release_mode; dsp_effects_update_params(); }
static int16_t gate_get_enc(void) { return -g_dsp_params.gate_threshold_db; }
static void gate_set_param(int16_t val) { g_dsp_params.gate_threshold_db = -val; dsp_effects_update_params(); }
static int32_t gate_get_bar(void) { return -g_dsp_params.gate_threshold_db; }
static void gate_format_str(char *buf) { sprintf(buf, "%ddB", g_dsp_params.gate_threshold_db); }

// Unified Table of Effect UI Descriptors
static const effect_ui_desc_t effects_desc[DSP_EFFECT_COUNT] = {
    [DSP_EFFECT_CLEAN]       = { "CLEAN", "VOL",  1,  32,  1,  32,   clean_btn2_press,   clean_get_enc,   clean_set_param,   clean_get_bar,   clean_format_str   },
    [DSP_EFFECT_ROBOT]       = { "ROBOT", "FREQ", 0,  145, 50, 1500, robot_btn2_press,   robot_get_enc,   robot_set_param,   robot_get_bar,   robot_format_str   },
    [DSP_EFFECT_DISTORTION]  = { "DIST",  "SAT",  1,  25,  1,  25,   dist_btn2_press,    dist_get_enc,    dist_set_param,    dist_get_bar,    dist_format_str    },
    [DSP_EFFECT_DELAY]       = { "ECHO",  "DEL",  0,  55,  50, 600,  delay_btn2_press,   delay_get_enc,   delay_set_param,   delay_get_bar,   delay_format_str   },
    [DSP_EFFECT_PITCH]       = { "PITCH", "PTCH", 0,  30,  0,  30,   pitch_btn2_press,   pitch_get_enc,   pitch_set_param,   pitch_get_bar,   pitch_format_str   },
    [DSP_EFFECT_TREMOLO_PAN] = { "TREM",  "RATE", 1,  20,  1,  20,   tremolo_btn2_press, tremolo_get_enc, tremolo_set_param, tremolo_get_bar, tremolo_format_str },
    [DSP_EFFECT_FILTER]      = { "FILTR", "FREQ", 0,  40,  0,  40,   filter_btn2_press,  filter_get_enc,  filter_set_param,  filter_get_bar,  filter_format_str  },
    [DSP_EFFECT_BITCRUSHER]  = { "CRUSH", "CRSH", 1,  30,  1,  30,   crusher_btn2_press, crusher_get_enc, crusher_set_param, crusher_get_bar, crusher_format_str },
    [DSP_EFFECT_GATE]        = { "BRAMK", "THR",  20, 60,  20, 60,   gate_btn2_press,    gate_get_enc,    gate_set_param,    gate_get_bar,    gate_format_str    }
};

void StartEncoderTask(void *argument)
{
    (void)argument;
    // Initialize OLED display and Encoders timer interfaces
    hw_encoders_init();
    SH1106_Init();

    // Force recalculation of biquads and noise gate parameters to defaults
    dsp_effects_update_params();

    // Synchronize encoder hardware timer counters with initial parameters
    hw_encoders_set_count1(g_dsp_params.current_effect * 2);
    hw_encoders_set_count2(g_dsp_params.volume_gain);

    int32_t lastEnc1 = g_dsp_params.current_effect * 2;
    int16_t lastEnc2 = g_dsp_params.volume_gain;

    // Static variables for FFT peak hold and dynamic scaling envelope
    static int16_t peak_heights[16] = {0};
    static uint8_t peak_hold_cnt[16] = {0};
    static float32_t fft_max_env = 0.2f;

    for (;;)
    {
        // 1. Read and debounce buttons and encoder hardware counters
        hw_encoders_update();

        // Handle Button 1 (Switch 1) - Bypass / Mute logic
        if (hw_encoders_is_btn1_pressed())
        {
            if (g_dsp_params.current_effect != DSP_EFFECT_CLEAN)
            {
                g_dsp_params.current_effect = DSP_EFFECT_CLEAN;
                hw_encoders_set_count1(0);
                hw_encoders_set_count2(g_dsp_params.volume_gain);
                lastEnc1 = 0;
                lastEnc2 = g_dsp_params.volume_gain;
            }
            else
            {
                g_dsp_params.is_muted = !g_dsp_params.is_muted;
            }
        }

        // Handle Button 2 (Switch 2) - Reset active parameter / Toggle submodes
        if (hw_encoders_is_btn2_pressed())
        {
            if (g_dsp_params.current_effect < DSP_EFFECT_COUNT)
            {
                const effect_ui_desc_t *fx = &effects_desc[g_dsp_params.current_effect];
                fx->on_btn2_press();
                int16_t new_enc = fx->get_encoder_val();
                hw_encoders_set_count2(new_enc);
                lastEnc2 = new_enc;
            }
        }

        // 2. Process Encoder 1 - Effect Selection
        int32_t enc1 = hw_encoders_get_count1();
        if (enc1 < 0)
        {
            enc1 = 0;
            hw_encoders_set_count1(0);
        }
        else if (enc1 > (DSP_EFFECT_COUNT - 1) * 2)
        {
            enc1 = (DSP_EFFECT_COUNT - 1) * 2;
            hw_encoders_set_count1(enc1);
        }

        if (enc1 != lastEnc1)
        {
            dsp_effect_id_t effect = (dsp_effect_id_t)(enc1 / 2);
            if (effect < DSP_EFFECT_COUNT && effect != g_dsp_params.current_effect)
            {
                g_dsp_params.current_effect = effect;
                const effect_ui_desc_t *fx = &effects_desc[g_dsp_params.current_effect];
                int16_t new_enc = fx->get_encoder_val();
                hw_encoders_set_count2(new_enc);
                lastEnc2 = new_enc;
            }
            lastEnc1 = enc1;
        }

        // 3. Process Encoder 2 - Parameter Modification
        if (g_dsp_params.current_effect < DSP_EFFECT_COUNT)
        {
            const effect_ui_desc_t *fx = &effects_desc[g_dsp_params.current_effect];
            int16_t enc2 = hw_encoders_get_count2();

            if (enc2 < fx->enc_min)
            {
                enc2 = fx->enc_min;
                hw_encoders_set_count2(fx->enc_min);
            }
            else if (enc2 > fx->enc_max)
            {
                enc2 = fx->enc_max;
                hw_encoders_set_count2(fx->enc_max);
            }

            if (enc2 != lastEnc2)
            {
                fx->set_param(enc2);
                lastEnc2 = enc2;
            }
        }

        // 4. OLED Rendering
        SH1106_Clear();

        if (g_dsp_params.current_effect < DSP_EFFECT_COUNT)
        {
            const effect_ui_desc_t *fx = &effects_desc[g_dsp_params.current_effect];
            char line1_buf[32];
            char val_buf[16];

            // Render Row 1: Effect name and parameter value text
            if (g_dsp_params.is_muted)
            {
                sprintf(line1_buf, "EFEKT: MUTE");
            }
            else
            {
                sprintf(line1_buf, "EFEKT: %s", fx->name);
            }
            SH1106_DrawStr(0, 0, line1_buf, 1);

            // Format parameter string and right-align at X = 127
            fx->format_val_str(val_buf);
            uint8_t val_len = strlen(val_buf);
            int16_t x_pos = 127 - (val_len * 6);
            SH1106_DrawStr(x_pos, 0, val_buf, 1);

            // Render Row 2: Parameter label and progress bar
            SH1106_DrawStr(0, 8, fx->param_label, 1);
            DrawProgressBar(32, 9, 64, 6, fx->get_bar_val(), fx->bar_min, fx->bar_max);
        }

        // Draw horizontal divider line at Y = 15
        for (int16_t x = 0; x < 128; x++)
        {
            SH1106_DrawPixel(x, 15, 1);
        }

        // Render Row 3: FFT Spectrum Analyzer (X = 0 to 111, Y = 16 to 51)
        float32_t local_max = 0.0f;
        for (int b = 0; b < 16; b++)
        {
            float32_t val = global_fft_mags[b];
            if (!(val >= 0.0f && val <= 1000.0f))
            {
                val = 0.0f;
                global_fft_mags[b] = 0.0f;
            }
            if (val > local_max)
            {
                local_max = val;
            }
        }
        
        if (local_max > fft_max_env)
        {
            fft_max_env = local_max;
        }
        else
        {
            fft_max_env = fft_max_env * 0.98f + local_max * 0.02f;
        }
        
        float32_t current_scale = fft_max_env;
        if (current_scale < 0.2f)
        {
            current_scale = 0.2f;
        }

        for (int b = 0; b < 16; b++)
        {
            float32_t magnitude = global_fft_mags[b];
            int16_t bar_height = (int16_t)((magnitude / current_scale) * 35.0f);
            if (bar_height > 35) bar_height = 35;
            if (bar_height < 0)  bar_height = 0;

            // Peak hold logic
            if (bar_height >= peak_heights[b])
            {
                peak_heights[b] = bar_height;
                peak_hold_cnt[b] = 8; // Hold peak marker for 8 cycles (~400 ms)
            }
            else
            {
                if (peak_hold_cnt[b] > 0)
                {
                    peak_hold_cnt[b]--;
                }
                else
                {
                    if (peak_heights[b] > 0)
                    {
                        peak_heights[b]--;
                    }
                }
            }

            // Spatial grouping gaps between bands
            int16_t x_start = 0;
            if (b < 5)
            {
                x_start = b * 6; // Bass
            }
            else if (b < 11)
            {
                x_start = 39 + (b - 5) * 6; // Mid
            }
            else
            {
                x_start = 84 + (b - 11) * 6; // Treble
            }

            // Draw filled bars
            int16_t y_top = 51 - bar_height;
            for (int16_t px = 0; px < 4; px++)
            {
                for (int16_t py = y_top; py <= 51; py++)
                {
                    SH1106_DrawPixel(x_start + px, py, 1);
                }
            }

            // Draw peak hold markers
            int16_t y_peak = 51 - peak_heights[b];
            for (int16_t px = 0; px < 4; px++)
            {
                SH1106_DrawPixel(x_start + px, y_peak, 1);
            }
        }

        // Render Row 4: Vertical VU Meter on the right (X = 116 to 123, Y = 16 to 51)
        int32_t vu = global_vu_level;
        int16_t vu_height = (int16_t)(((int64_t)vu * 35) / DSP_MAX_INT24);
        if (vu_height > 35) vu_height = 35;
        if (vu_height < 0)  vu_height = 0;

        // Draw VU box border
        for (int16_t py = 16; py <= 51; py++)
        {
            SH1106_DrawPixel(116, py, 1);
            SH1106_DrawPixel(123, py, 1);
        }
        for (int16_t px = 116; px <= 123; px++)
        {
            SH1106_DrawPixel(px, 16, 1);
            SH1106_DrawPixel(px, 51, 1);
        }

        // Draw filled VU column
        int16_t vu_y_top = 51 - vu_height;
        for (int16_t py = vu_y_top; py < 51; py++)
        {
            for (int16_t px = 118; px <= 121; px++)
            {
                SH1106_DrawPixel(px, py, 1);
            }
        }

        // Render clipping peak warning indicator
        if (vu_height >= 34)
        {
            SH1106_DrawPixel(119, 18, 1);
            SH1106_DrawPixel(120, 18, 1);
            SH1106_DrawPixel(119, 19, 1);
            SH1106_DrawPixel(120, 19, 1);
        }

        // Render Row 5: Bottom frequency labels (Y = 54)
        SH1106_DrawStr(2, 54, "Bas", 1);
        SH1106_DrawStr(42, 54, "Sred", 1);
        SH1106_DrawStr(84, 54, "Sopr", 1);
        SH1106_DrawStr(116, 54, "VU", 1);

        // Send memory frame buffer to display panel
        SH1106_Update();

        osDelay(50);
    }
}
