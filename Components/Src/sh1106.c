#include "sh1106.h"
#include "bsp_gpio.h"
#include "bsp_spi.h"
#include "bsp.h"
#include <stddef.h>

static void sh1106_write_command(uint8_t cmd)
{
    bsp_gpio_write(BSP_PIN_OLED_DC, BSP_PIN_RESET);
    bsp_gpio_write(BSP_PIN_OLED_CS, BSP_PIN_RESET);
    bsp_spi1_transmit(&cmd, 1, 10);
    bsp_gpio_write(BSP_PIN_OLED_CS, BSP_PIN_SET);
}

static void sh1106_write_data(const uint8_t *data, uint16_t size)
{
    bsp_gpio_write(BSP_PIN_OLED_DC, BSP_PIN_SET);
    bsp_gpio_write(BSP_PIN_OLED_CS, BSP_PIN_RESET);
    bsp_spi1_transmit(data, size, 100);
    bsp_gpio_write(BSP_PIN_OLED_CS, BSP_PIN_SET);
}

bool sh1106_init(void)
{
    // Hardware Reset via BSP
    bsp_gpio_write(BSP_PIN_OLED_RES, BSP_PIN_RESET);
    bsp_delay_ms(10);
    bsp_gpio_write(BSP_PIN_OLED_RES, BSP_PIN_SET);
    bsp_delay_ms(10);

    // Initialization command sequence for SH1106 OLED Controller
    sh1106_write_command(0xAE); // Display OFF
    sh1106_write_command(0x02); // Set lower column address
    sh1106_write_command(0x10); // Set higher column address
    sh1106_write_command(0x40); // Set display start line
    sh1106_write_command(0x81); // Set contrast control
    sh1106_write_command(0xCF); // Contrast value
    sh1106_write_command(0xA1); // Segment re-map
    sh1106_write_command(0xC8); // COM scan direction
    sh1106_write_command(0xA6); // Normal display
    sh1106_write_command(0xA8); // Set multiplex ratio
    sh1106_write_command(0x3F); // 1/64 duty
    sh1106_write_command(0xD3); // Set display offset
    sh1106_write_command(0x00); // No offset
    sh1106_write_command(0xD5); // Set osc frequency
    sh1106_write_command(0x80);
    sh1106_write_command(0xD9); // Set pre-charge period
    sh1106_write_command(0xF1); // Pre-charge period
    sh1106_write_command(0xDA); // Set COM pins config
    sh1106_write_command(0x12);
    sh1106_write_command(0xDB); // Set VCOMH level
    sh1106_write_command(0x40);
    sh1106_write_command(0xAD); // DC-DC mode (SH1106)
    sh1106_write_command(0x8B); // DC-DC ON
    sh1106_write_command(0x8D); // Charge Pump (SSD1306 compatibility)
    sh1106_write_command(0x14); // Charge Pump ON
    sh1106_write_command(0xA4); // Normal RAM content
    sh1106_write_command(0xAF); // Display ON

    bsp_delay_ms(100);
    return true;
}

void sh1106_display_on(bool enable)
{
    sh1106_write_command(enable ? 0xAF : 0xAE);
}

void sh1106_set_contrast(uint8_t contrast)
{
    sh1106_write_command(0x81);
    sh1106_write_command(contrast);
}

bool sh1106_flush(const uint8_t *framebuffer)
{
    if (framebuffer == NULL)
    {
        return false;
    }

    for (uint8_t page = 0; page < SH1106_PAGE_COUNT; page++)
    {
        sh1106_write_command(0xB0 + page);
        sh1106_write_command(0x02); // Column address offset for SH1106 is 2 pixels
        sh1106_write_command(0x10);
        sh1106_write_data(&framebuffer[page * SH1106_WIDTH], SH1106_WIDTH);
    }

    return true;
}

