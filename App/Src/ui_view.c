#include "ui_view.h"
#include "gfx.h"
#include "ui_widgets.h"
#include <stdio.h>
#include <string.h>

void ui_view_render(const ui_effect_display_t *display,
                    const ui_visualization_t *visualization)
{
    if (display == NULL || visualization == NULL)
    {
        return;
    }

    static int16_t peak_heights[16];
    static uint8_t peak_hold_count[16];
    static float fft_max_envelope = 0.2f;

    gfx_clear();

    char status_line[32];
    if (display->muted)
    {
        (void)snprintf(status_line, sizeof(status_line), "EFEKT: MUTE");
    }
    else
    {
        (void)snprintf(status_line, sizeof(status_line), "EFEKT: %s",
                       display->effect_name);
    }
    gfx_draw_str(0, 0, status_line, 1);

    int16_t value_x = (int16_t)(127 - (strlen(display->parameter_value) * 6U));
    gfx_draw_str(value_x, 0, display->parameter_value, 1);
    gfx_draw_str(0, 8, display->parameter_label, 1);
    ui_widget_progress_bar(32, 9, 64, 6, display->parameter_value_raw, display->parameter_min, display->parameter_max);
    gfx_draw_line_h(0, 15, 128, 1);

    float magnitudes[UI_SPECTRUM_BAND_COUNT];
    float local_max = 0.0f;
    for (uint8_t band = 0; band < UI_SPECTRUM_BAND_COUNT; band++)
    {
        float magnitude = visualization->band_magnitudes[band];
        if (!(magnitude >= 0.0f && magnitude <= 1000.0f))
        {
            magnitude = 0.0f;
        }
        magnitudes[band] = magnitude;
        if (magnitude > local_max)
        {
            local_max = magnitude;
        }
    }

    if (local_max > fft_max_envelope)
    {
        fft_max_envelope = local_max;
    }
    else
    {
        fft_max_envelope = fft_max_envelope * 0.98f + local_max * 0.02f;
    }
    if (fft_max_envelope < 0.2f)
    {
        fft_max_envelope = 0.2f;
    }

    for (uint8_t band = 0; band < UI_SPECTRUM_BAND_COUNT; band++)
    {
        int16_t height = (int16_t)((magnitudes[band] / fft_max_envelope) * 35.0f);
        if (height > 35)
        {
            height = 35;
        }
        if (height < 0)
        {
            height = 0;
        }

        if (height >= peak_heights[band])
        {
            peak_heights[band] = height;
            peak_hold_count[band] = 8U;
        }
        else if (peak_hold_count[band] > 0U)
        {
            peak_hold_count[band]--;
        }
        else if (peak_heights[band] > 0)
        {
            peak_heights[band]--;
        }

        int16_t x_start;
        if (band < 5U)
        {
            x_start = (int16_t)(band * 6U);
        }
        else if (band < 11U)
        {
            x_start = (int16_t)(39 + (band - 5U) * 6U);
        }
        else
        {
            x_start = (int16_t)(84 + (band - 11U) * 6U);
        }

        for (int16_t x = 0; x < 4; x++)
        {
            for (int16_t y = (int16_t)(51 - height); y <= 51; y++)
            {
                gfx_draw_pixel(x_start + x, y, 1);
            }
            gfx_draw_pixel(x_start + x,
                           (int16_t)(51 - peak_heights[band]), 1);
        }
    }

    int16_t vu_height =
        (int16_t)(((uint32_t)visualization->peak_level_per_mille * 35U) / 1000U);
    if (vu_height > 35)
    {
        vu_height = 35;
    }
    if (vu_height < 0)
    {
        vu_height = 0;
    }

    for (int16_t y = 16; y <= 51; y++)
    {
        gfx_draw_pixel(116, y, 1);
        gfx_draw_pixel(123, y, 1);
    }
    for (int16_t x = 116; x <= 123; x++)
    {
        gfx_draw_pixel(x, 16, 1);
        gfx_draw_pixel(x, 51, 1);
    }

    for (int16_t y = (int16_t)(51 - vu_height); y < 51; y++)
    {
        for (int16_t x = 118; x <= 121; x++)
        {
            gfx_draw_pixel(x, y, 1);
        }
    }

    if (vu_height >= 34)
    {
        gfx_draw_pixel(119, 18, 1);
        gfx_draw_pixel(120, 18, 1);
        gfx_draw_pixel(119, 19, 1);
        gfx_draw_pixel(120, 19, 1);
    }

    gfx_draw_str(2, 54, "Bas", 1);
    gfx_draw_str(42, 54, "Sred", 1);
    gfx_draw_str(84, 54, "Sopr", 1);
    gfx_draw_str(116, 54, "VU", 1);
}
