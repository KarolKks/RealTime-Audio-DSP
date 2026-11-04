/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Entry point for the Real-Time Audio DSP Application
  ******************************************************************************
  */

#include "main.h"
#include "tasks.h"

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* 1. Initialize Board Support Package (MCU Clocks, GPIO, SPI1, Timers, SAI1/DMA) */
    bsp_init();

    /* Initialize the RTOS kernel and create application tasks. */
    if (osKernelInitialize() != osOK)
    {
        Error_Handler();
    }

    if (!tasks_create())
    {
        Error_Handler();
    }

    if (osKernelStart() != osOK)
    {
        Error_Handler();
    }

    /* We should never get here as control is now taken by the scheduler */
    while (1)
    {
    }
}

/**
  * @brief  Period elapsed callback in non-blocking mode.
  * @note   This function is called when TIM6 interrupt took place (HAL tick timebase).
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        HAL_IncTick();
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    bsp_error_handler();
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
}
#endif /* USE_FULL_ASSERT */
