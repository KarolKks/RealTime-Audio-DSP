#include "ui_widgets.h"
#include "gfx.h"

void ui_widget_progress_bar(int16_t x, int16_t y, int16_t width, int16_t height, 
                           int32_t val, int32_t min, int32_t max)
{
    // Draw outer boundary
    for (int16_t i = 0; i < width; i++)
    {
        gfx_draw_pixel(x + i, y, 1);
        gfx_draw_pixel(x + i, y + height - 1, 1);
    }
    for (int16_t j = 0; j < height; j++)
    {
        gfx_draw_pixel(x, y + j, 1);
        gfx_draw_pixel(x + width - 1, y + j, 1);
    }

    // Return if range is zero
    if (max == min)
    {
        return;
    }

    // Draw fill area
    int32_t fill_width = ((val - min) * (width - 4)) / (max - min);
    if (fill_width < 0)
    {
        fill_width = 0;
    }
    if (fill_width > width - 4)
    {
        fill_width = width - 4;
    }

    for (int16_t i = 0; i < fill_width; i++)
    {
        for (int16_t j = 2; j < height - 2; j++)
        {
            gfx_draw_pixel(x + 2 + i, y + j, 1);
        }
    }
}

