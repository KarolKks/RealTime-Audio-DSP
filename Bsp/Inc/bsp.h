#ifndef BSP_H_
#define BSP_H_

#include "bsp_gpio.h"
#include "bsp_spi.h"
#include "bsp_timers.h"
#include "bsp_sai.h"

/**
 * @brief  Initializes the complete Board Support Package:
 *         - Core System Clocks (MSI, PLL up to 80 MHz, PLLSAI1 for Audio)
 *         - GPIO Ports & Pin Configuration
 *         - SPI1 Bus
 *         - Encoder Timers TIM2 and TIM3
 *         - Audio SAI1 & DMA Streams
 */
void bsp_init(void);

/**
 * @brief  BSP-level error handler invoked on critical peripheral failure.
 */
void bsp_error_handler(void);

#endif /* BSP_H_ */

