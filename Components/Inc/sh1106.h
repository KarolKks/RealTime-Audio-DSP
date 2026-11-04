#ifndef SH1106_H_
#define SH1106_H_

#include <stdint.h>
#include <stdbool.h>

#define SH1106_WIDTH       128
#define SH1106_HEIGHT      64
#define SH1106_PAGE_COUNT  8
#define SH1106_BUFFER_SIZE (SH1106_WIDTH * SH1106_PAGE_COUNT) // 1024 bytes

/**
 * @brief  Initializes the SH1106 OLED display controller via BSP SPI/GPIO interface.
 * @retval true on success, false on failure.
 */
bool sh1106_init(void);

/**
 * @brief  Turns the display panel ON or OFF.
 * @param  enable: true to turn ON (0xAF), false to turn OFF (0xAE).
 */
void sh1106_display_on(bool enable);

/**
 * @brief  Sets display contrast.
 * @param  contrast: Value from 0 to 255.
 */
void sh1106_set_contrast(uint8_t contrast);

/**
 * @brief  Flushes an external 1024-byte framebuffer to the SH1106 controller RAM.
 * @param  framebuffer: Pointer to 1024-byte display buffer (128 columns * 8 pages).
 * @retval true on success, false on failure.
 */
bool sh1106_flush(const uint8_t *framebuffer);

#endif /* SH1106_H_ */

