#ifndef BSP_SAI_H_
#define BSP_SAI_H_

#include <stdint.h>
#include <stdbool.h>

typedef void (*bsp_sai_callback_t)(void *context);

/**
 * @brief  Initializes DMA controller and SAI1 peripheral (Block A TX, Block B RX).
 */
void bsp_sai_init(void);

/**
 * @brief  Starts simultaneous circular DMA reception (Block B) and transmission (Block A).
 * @param  rx_buf: Pointer to input stereo 32-bit audio buffer.
 * @param  tx_buf: Pointer to output stereo 32-bit audio buffer.
 * @param  size: Total number of 32-bit words in the circular buffer.
 * @retval true on success, false on error or invalid arguments.
 */
bool bsp_sai_start_duplex_dma(int32_t *rx_buf, int32_t *tx_buf, uint16_t size);

/**
 * @brief  Registers user callbacks for DMA half-transfer and transfer-complete events.
 * @param  half_cplt: Function called when first half of rx buffer is ready.
 * @param  full_cplt: Function called when second half of rx buffer is ready.
 * @param  context: User context pointer passed to callback functions.
 */
void bsp_sai_register_callbacks(bsp_sai_callback_t half_cplt, bsp_sai_callback_t full_cplt, void *context);

#endif /* BSP_SAI_H_ */

