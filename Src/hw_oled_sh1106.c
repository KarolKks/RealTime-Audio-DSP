#include "hw_oled_sh1106.h"
#include "main.h"
#include <string.h>

// SPI handle defined in main.c
extern SPI_HandleTypeDef hspi1;

// Font character mapping string containing only used characters
static const char font_map[] = " :.-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZadekmoprsxz";

// Shortened font table (5x7 pixels) for 51 characters
static const uint8_t font5x7[51][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // ' ' (space)
    {0x00, 0x36, 0x36, 0x00, 0x00}, // ':'
    {0x00, 0x60, 0x60, 0x00, 0x00}, // '.'
    {0x08, 0x08, 0x08, 0x08, 0x08}, // '-'
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // '0'
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1'
    {0x42, 0x61, 0x51, 0x49, 0x46}, // '2'
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // '3'
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // '4'
    {0x27, 0x45, 0x45, 0x45, 0x39}, // '5'
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // '6'
    {0x01, 0x71, 0x09, 0x05, 0x03}, // '7'
    {0x36, 0x49, 0x49, 0x49, 0x36}, // '8'
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // '9'
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 'A'
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 'D'
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 'E'
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // 'F'
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // 'G'
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 'H'
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 'I'
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 'J'
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 'K'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 'L'
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 'M'
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 'N'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 'O'
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 'P'
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 'Q'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 'R'
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 'S'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 'T'
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 'U'
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 'V'
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // 'W'
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 'X'
    {0x03, 0x04, 0x78, 0x04, 0x03}, // 'Y'
    {0x61, 0x51, 0x49, 0x45, 0x43}, // 'Z'
    {0x20, 0x54, 0x54, 0x54, 0x78}, // 'a'
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // 'd'
    {0x38, 0x54, 0x54, 0x54, 0x18}, // 'e'
    {0x00, 0x7F, 0x10, 0x28, 0x44}, // 'k'
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // 'm'
    {0x38, 0x44, 0x44, 0x44, 0x38}, // 'o'
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // 'p'
    {0x7C, 0x08, 0x04, 0x01, 0x00}, // 'r'
    {0x48, 0x54, 0x54, 0x54, 0x20}, // 's'
    {0x44, 0x28, 0x10, 0x28, 0x44}, // 'x'
    {0x44, 0x64, 0x54, 0x4C, 0x44}  // 'z'
};

// Local RAM display frame buffer (128 columns * 8 pages = 1024 bytes)
static uint8_t oled_buffer[1024];

static void SH1106_WriteCommand(uint8_t cmd)
{
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
    HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
}

static void SH1106_WriteData(uint8_t *data, uint16_t size)
{
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, data, size, 100);
    HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
}

// Looks up the index of a character in the shortened font_map
static int16_t get_char_index(char c)
{
    for (int16_t i = 0; i < 51; i++)
    {
        if (font_map[i] == c)
        {
            return i;
        }
    }
    return 0; // Default to ' ' (space) if character is not found
}

void SH1106_Init(void)
{
    // Hardware Reset
    HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(10);

    // Initialize SH1106 controller
    SH1106_WriteCommand(0xAE); // Display OFF
    SH1106_WriteCommand(0x02); // Set lower column address
    SH1106_WriteCommand(0x10); // Set higher column address
    SH1106_WriteCommand(0x40); // Set display start line
    SH1106_WriteCommand(0x81); // Set contrast control
    SH1106_WriteCommand(0xCF); // Contrast value
    SH1106_WriteCommand(0xA1); // Segment re-map
    SH1106_WriteCommand(0xC8); // COM scan direction
    SH1106_WriteCommand(0xA6); // Normal display
    SH1106_WriteCommand(0xA8); // Set multiplex ratio
    SH1106_WriteCommand(0x3F); // 1/64 duty
    SH1106_WriteCommand(0xD3); // Set display offset
    SH1106_WriteCommand(0x00); // No offset
    SH1106_WriteCommand(0xD5); // Set osc frequency
    SH1106_WriteCommand(0x80);
    SH1106_WriteCommand(0xD9); // Set pre-charge period
    SH1106_WriteCommand(0xF1); // Pre-charge period
    SH1106_WriteCommand(0xDA); // Set COM pins config
    SH1106_WriteCommand(0x12);
    SH1106_WriteCommand(0xDB); // Set VCOMH level
    SH1106_WriteCommand(0x40);
    SH1106_WriteCommand(0xAD); // DC-DC mode (SH1106)
    SH1106_WriteCommand(0x8B); // DC-DC ON
    SH1106_WriteCommand(0x8D); // Charge Pump (SSD1306)
    SH1106_WriteCommand(0x14); // Charge Pump ON
    SH1106_WriteCommand(0xA4); // Normal RAM content
    SH1106_WriteCommand(0xAF); // Display ON

    // Clear frame buffer and flush to screen
    SH1106_Clear();
    SH1106_Update();

    HAL_Delay(100);
}

void SH1106_DrawPixel(int16_t x, int16_t y, uint8_t color)
{
    if (x < 0 || x >= 128 || y < 0 || y >= 64)
    {
        return;
    }
    
    if (color)
    {
        oled_buffer[x + (y / 8) * 128] |= (1 << (y % 8));
    }
    else
    {
        oled_buffer[x + (y / 8) * 128] &= ~(1 << (y % 8));
    }
}

void SH1106_DrawChar(int16_t x, int16_t y, char c, uint8_t color)
{
    int16_t idx = get_char_index(c);
    
    for (uint8_t col = 0; col < 5; col++)
    {
        uint8_t line = font5x7[idx][col];
        for (uint8_t row = 0; row < 8; row++)
        {
            if (line & (1 << row))
            {
                SH1106_DrawPixel(x + col, y + row, color);
            }
        }
    }
}

void SH1106_DrawStr(int16_t x, int16_t y, const char *str, uint8_t color)
{
    while (*str)
    {
        SH1106_DrawChar(x, y, *str++, color);
        x += 6;
    }
}

void SH1106_Clear(void)
{
    memset(oled_buffer, 0, sizeof(oled_buffer));
}

void SH1106_Update(void)
{
    for (uint8_t page = 0; page < 8; page++)
    {
        SH1106_WriteCommand(0xB0 + page);
        SH1106_WriteCommand(0x02); // Column address offset for SH1106 is 2 pixels
        SH1106_WriteCommand(0x10);
        SH1106_WriteData(&oled_buffer[page * 128], 128);
    }
}

void DrawProgressBar(int16_t x, int16_t y, int16_t width, int16_t height, int32_t val, int32_t min, int32_t max)
{
    // Draw outer boundary
    for (int16_t i = 0; i < width; i++)
    {
        SH1106_DrawPixel(x + i, y, 1);
        SH1106_DrawPixel(x + i, y + height - 1, 1);
    }
    for (int16_t j = 0; j < height; j++)
    {
        SH1106_DrawPixel(x, y + j, 1);
        SH1106_DrawPixel(x + width - 1, y + j, 1);
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
            SH1106_DrawPixel(x + 2 + i, y + j, 1);
        }
    }
}
