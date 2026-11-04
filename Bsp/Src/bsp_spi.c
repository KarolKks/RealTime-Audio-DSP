#include "bsp_spi.h"
#include "stm32l4xx_hal.h"
#include <stddef.h>

static SPI_HandleTypeDef hspi1;

void bsp_spi1_init(void)
{
    hspi1.Instance               = SPI1;
    hspi1.Init.Mode              = SPI_MODE_MASTER;
    hspi1.Init.Direction         = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize          = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity       = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase          = SPI_PHASE_1EDGE;
    hspi1.Init.NSS               = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi1.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial     = 7;
    hspi1.Init.CRCLength         = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode          = SPI_NSS_PULSE_ENABLE;

    HAL_SPI_Init(&hspi1);
}

bool bsp_spi1_transmit(const uint8_t *data, uint16_t size, uint32_t timeout_ms)
{
    if (data == NULL || size == 0)
    {
        return false;
    }
    return (HAL_SPI_Transmit(&hspi1, (uint8_t *)data, size, timeout_ms) == HAL_OK);
}

