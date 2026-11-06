#include "gfx.h"
#include <string.h>

// External prototype from font5x7.c
const uint8_t* font5x7_get_char_bitmap(char c);

// Local RAM display frame buffer (128 columns * 8 pages = 1024 bytes)
static uint8_t s_framebuffer[GFX_BUFFER_SIZE];

void gfx_init(void)
{
    gfx_clear();
}

void gfx_clear(void)
{
    memset(s_framebuffer, 0, sizeof(s_framebuffer));
}

const uint8_t* gfx_get_framebuffer(void)
{
    return s_framebuffer;
}

void gfx_draw_pixel(int16_t x, int16_t y, uint8_t color)
{
    if (x < 0 || x >= GFX_WIDTH || y < 0 || y >= GFX_HEIGHT)
    {
        return;
    }

    if (color)
    {
        s_framebuffer[x + (y / 8) * GFX_WIDTH] |= (1 << (y % 8));
    }
    else
    {
        s_framebuffer[x + (y / 8) * GFX_WIDTH] &= ~(1 << (y % 8));
    }
}

void gfx_draw_char(int16_t x, int16_t y, char c, uint8_t color)
{
    const uint8_t *bitmap = font5x7_get_char_bitmap(c);

    for (uint8_t col = 0; col < 5; col++)
    {
        uint8_t line = bitmap[col];
        for (uint8_t row = 0; row < 8; row++)
        {
            if (line & (1 << row))
            {
                gfx_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

void gfx_draw_str(int16_t x, int16_t y, const char *str, uint8_t color)
{
    if (str == NULL)
    {
        return;
    }

    while (*str)
    {
        gfx_draw_char(x, y, *str++, color);
        x += 6; // 5 pixels width + 1 pixel spacing
    }
}

void gfx_draw_line_h(int16_t x, int16_t y, int16_t width, uint8_t color)
{
    for (int16_t i = 0; i < width; i++)
    {
        gfx_draw_pixel(x + i, y, color);
    }
}

