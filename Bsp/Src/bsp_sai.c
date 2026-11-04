#include "bsp_sai.h"
#include "bsp.h"
#include "stm32l4xx_hal.h"
#include <stddef.h>

/* Handles required by stm32l4xx_it.c and stm32l4xx_hal_msp.c */
SAI_HandleTypeDef hsai_BlockA1;
SAI_HandleTypeDef hsai_BlockB1;
DMA_HandleTypeDef hdma_sai1_a;
DMA_HandleTypeDef hdma_sai1_b;

/* User-registered callbacks */
static bsp_sai_callback_t s_half_cplt_cb = NULL;
static bsp_sai_callback_t s_full_cplt_cb = NULL;
static void              *s_cb_context   = NULL;

static void MX_DMA_Init(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA2_Channel1_IRQn interrupt configuration (SAI1 Block A) */
    HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);

    /* DMA2_Channel2_IRQn interrupt configuration (SAI1 Block B) */
    HAL_NVIC_SetPriority(DMA2_Channel2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);
}

static void MX_SAI1_Init(void)
{
    /* Block A: Master TX */
    hsai_BlockA1.Instance                    = SAI1_Block_A;
    hsai_BlockA1.Init.Protocol               = SAI_FREE_PROTOCOL;
    hsai_BlockA1.Init.AudioMode              = SAI_MODEMASTER_TX;
    hsai_BlockA1.Init.DataSize               = SAI_DATASIZE_32;
    hsai_BlockA1.Init.FirstBit               = SAI_FIRSTBIT_MSB;
    hsai_BlockA1.Init.ClockStrobing          = SAI_CLOCKSTROBING_FALLINGEDGE;
    hsai_BlockA1.Init.Synchro                = SAI_ASYNCHRONOUS;
    hsai_BlockA1.Init.OutputDrive            = SAI_OUTPUTDRIVE_DISABLE;
    hsai_BlockA1.Init.NoDivider              = SAI_MASTERDIVIDER_ENABLE;
    hsai_BlockA1.Init.FIFOThreshold          = SAI_FIFOTHRESHOLD_EMPTY;
    hsai_BlockA1.Init.AudioFrequency         = SAI_AUDIO_FREQUENCY_32K;
    hsai_BlockA1.Init.SynchroExt             = SAI_SYNCEXT_DISABLE;
    hsai_BlockA1.Init.MonoStereoMode         = SAI_STEREOMODE;
    hsai_BlockA1.Init.CompandingMode         = SAI_NOCOMPANDING;
    hsai_BlockA1.Init.TriState               = SAI_OUTPUT_NOTRELEASED;
    hsai_BlockA1.FrameInit.FrameLength       = 64;
    hsai_BlockA1.FrameInit.ActiveFrameLength = 32;
    hsai_BlockA1.FrameInit.FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION;
    hsai_BlockA1.FrameInit.FSPolarity        = SAI_FS_ACTIVE_LOW;
    hsai_BlockA1.FrameInit.FSOffset          = SAI_FS_BEFOREFIRSTBIT;
    hsai_BlockA1.SlotInit.FirstBitOffset     = 0;
    hsai_BlockA1.SlotInit.SlotSize           = SAI_SLOTSIZE_DATASIZE;
    hsai_BlockA1.SlotInit.SlotNumber         = 2;
    hsai_BlockA1.SlotInit.SlotActive         = 0x0000FFFF;
    if (HAL_SAI_Init(&hsai_BlockA1) != HAL_OK)
    {
        bsp_error_handler();
    }

    /* Block B: Synchronous Slave RX */
    hsai_BlockB1.Instance                    = SAI1_Block_B;
    hsai_BlockB1.Init.Protocol               = SAI_FREE_PROTOCOL;
    hsai_BlockB1.Init.AudioMode              = SAI_MODESLAVE_RX;
    hsai_BlockB1.Init.DataSize               = SAI_DATASIZE_32;
    hsai_BlockB1.Init.FirstBit               = SAI_FIRSTBIT_MSB;
    hsai_BlockB1.Init.ClockStrobing          = SAI_CLOCKSTROBING_FALLINGEDGE;
    hsai_BlockB1.Init.Synchro                = SAI_SYNCHRONOUS;
    hsai_BlockB1.Init.OutputDrive            = SAI_OUTPUTDRIVE_DISABLE;
    hsai_BlockB1.Init.FIFOThreshold          = SAI_FIFOTHRESHOLD_EMPTY;
    hsai_BlockB1.Init.SynchroExt             = SAI_SYNCEXT_DISABLE;
    hsai_BlockB1.Init.MonoStereoMode         = SAI_STEREOMODE;
    hsai_BlockB1.Init.CompandingMode         = SAI_NOCOMPANDING;
    hsai_BlockB1.Init.TriState               = SAI_OUTPUT_NOTRELEASED;
    hsai_BlockB1.FrameInit.FrameLength       = 64;
    hsai_BlockB1.FrameInit.ActiveFrameLength = 32;
    hsai_BlockB1.FrameInit.FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION;
    hsai_BlockB1.FrameInit.FSPolarity        = SAI_FS_ACTIVE_LOW;
    hsai_BlockB1.FrameInit.FSOffset          = SAI_FS_BEFOREFIRSTBIT;
    hsai_BlockB1.SlotInit.FirstBitOffset     = 0;
    hsai_BlockB1.SlotInit.SlotSize           = SAI_SLOTSIZE_DATASIZE;
    hsai_BlockB1.SlotInit.SlotNumber         = 2;
    hsai_BlockB1.SlotInit.SlotActive         = 0x0000FFFF;
    if (HAL_SAI_Init(&hsai_BlockB1) != HAL_OK)
    {
        bsp_error_handler();
    }
}

void bsp_sai_init(void)
{
    MX_DMA_Init();
    MX_SAI1_Init();
}

bool bsp_sai_start_duplex_dma(int32_t *rx_buf, int32_t *tx_buf, uint16_t size)
{
    if (rx_buf == NULL || tx_buf == NULL || size == 0)
    {
        return false;
    }

    if (HAL_SAI_Receive_DMA(&hsai_BlockB1, (uint8_t *)rx_buf, size) != HAL_OK)
    {
        return false;
    }

    if (HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t *)tx_buf, size) != HAL_OK)
    {
        if (HAL_SAI_DMAStop(&hsai_BlockB1) != HAL_OK)
        {
            bsp_error_handler();
        }
        return false;
    }

    return true;
}

void bsp_sai_register_callbacks(bsp_sai_callback_t half_cplt, bsp_sai_callback_t full_cplt, void *context)
{
    s_half_cplt_cb = half_cplt;
    s_full_cplt_cb = full_cplt;
    s_cb_context   = context;
}

/* HAL SAI Interrupt Callbacks */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (hsai->Instance == SAI1_Block_B && s_half_cplt_cb != NULL)
    {
        s_half_cplt_cb(s_cb_context);
    }
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (hsai->Instance == SAI1_Block_B && s_full_cplt_cb != NULL)
    {
        s_full_cplt_cb(s_cb_context);
    }
}
