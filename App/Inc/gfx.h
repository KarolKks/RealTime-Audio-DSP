#ifndef GFX_H_
#define GFX_H_

#include <stdint.h>
#include <stdbool.h>

#define GFX_WIDTH       128
#define GFX_HEIGHT      64
#define GFX_BUFFER_SIZE (GFX_WIDTH * GFX_HEIGHT / 8) // 1024 bytes

/**
 * @brief  Initializes the graphics buffer.
 */
void gfx_init(void);

/**
 * @brief  Clears the entire 1024-byte RAM frame buffer (fills with 0).
 */
void gfx_clear(void);

/**
 * @brief  Gets a pointer to the internal RAM frame buffer.
 * @retval Pointer to 1024-byte buffer.
 */
const uint8_t* gfx_get_framebuffer(void);

/**
 * @brief  Draws a single pixel in the RAM frame buffer.
 * @param  x: X coordinate (0 to 127).
 * @param  y: Y coordinate (0 to 63).
 * @param  color: 1 for pixel ON, 0 for pixel OFF.
 */
void gfx_draw_pixel(int16_t x, int16_t y, uint8_t color);

/**
 * @brief  Draws a single character using font 5x7 in the frame buffer.
 * @param  x: X coordinate of top-left corner.
 * @param  y: Y coordinate of top-left corner.
 * @param  c: Character to draw (ASCII).
 * @param  color: 1 for character ON, 0 for character OFF.
 */
void gfx_draw_char(int16_t x, int16_t y, char c, uint8_t color);

/**
 * @brief  Draws a string in the frame buffer.
 * @param  x: X coordinate of start.
 * @param  y: Y coordinate of start.
 * @param  str: Null-terminated string.
 * @param  color: 1 for text ON, 0 for text OFF.
 */
void gfx_draw_str(int16_t x, int16_t y, const char *str, uint8_t color);

/**
 * @brief  Draws a horizontal line.
 * @param  x: Start X coordinate.
 * @param  y: Y coordinate.
 * @param  width: Line width in pixels.
 * @param  color: 1 for line ON, 0 for line OFF.
 */
void gfx_draw_line_h(int16_t x, int16_t y, int16_t width, uint8_t color);

#endif /* GFX_H_ */

