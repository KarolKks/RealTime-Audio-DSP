#ifndef HW_OLED_SH1106_H_
#define HW_OLED_SH1106_H_

#include "stm32l4xx_hal.h"

/**
 * @brief  Initializes the SH1106 OLED display using SPI interface.
 *         Clears the display RAM and powers it on.
 * @param  None
 * @retval None
 */
void SH1106_Init(void);

/**
 * @brief  Draws a single pixel in the display frame buffer.
 * @param  x: X coordinate (0 to 127).
 * @param  y: Y coordinate (0 to 63).
 * @param  color: 1 for pixel ON, 0 for pixel OFF.
 * @retval None
 */
void SH1106_DrawPixel(int16_t x, int16_t y, uint8_t color);

/**
 * @brief  Draws a single character using font 5x7 in the frame buffer.
 * @param  x: X coordinate of top-left corner.
 * @param  y: Y coordinate of top-left corner.
 * @param  c: Character to draw (ASCII).
 * @param  color: 1 for character ON, 0 for character OFF.
 * @retval None
 */
void SH1106_DrawChar(int16_t x, int16_t y, char c, uint8_t color);

/**
 * @brief  Draws a string in the frame buffer.
 * @param  x: X coordinate of start.
 * @param  y: Y coordinate of start.
 * @param  str: Null-terminated string.
 * @param  color: 1 for text ON, 0 for text OFF.
 * @retval None
 */
void SH1106_DrawStr(int16_t x, int16_t y, const char *str, uint8_t color);

/**
 * @brief  Clears the entire local RAM frame buffer (fills with 0).
 * @param  None
 * @retval None
 */
void SH1106_Clear(void);

/**
 * @brief  Flushes the local RAM frame buffer to the SH1106 OLED display controller over SPI.
 * @param  None
 * @retval None
 */
void SH1106_Update(void);

/**
 * @brief  Draws a formatted progress bar in the frame buffer.
 * @param  x: X coordinate of top-left corner.
 * @param  y: Y coordinate of top-left corner.
 * @param  width: Width of the progress bar.
 * @param  height: Height of the progress bar.
 * @param  val: Current parameter value.
 * @param  min: Minimum parameter value.
 * @param  max: Maximum parameter value.
 * @retval None
 */
void DrawProgressBar(int16_t x, int16_t y, int16_t width, int16_t height, int32_t val, int32_t min, int32_t max);

#endif /* HW_OLED_SH1106_H_ */
