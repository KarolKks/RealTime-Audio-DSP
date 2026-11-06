#ifndef BSP_SPI_H_
#define BSP_SPI_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief  Initializes SPI1 peripheral in Master mode.
 */
void bsp_spi1_init(void);

/**
 * @brief  Transmits a block of data over SPI1 bus (blocking).
 * @param  data: Pointer to data buffer.
 * @param  size: Number of bytes to transmit.
 * @param  timeout_ms: Timeout in milliseconds.
 * @retval true on success, false on error or invalid parameters.
 */
bool bsp_spi1_transmit(const uint8_t *data, uint16_t size, uint32_t timeout_ms);

#endif /* BSP_SPI_H_ */

