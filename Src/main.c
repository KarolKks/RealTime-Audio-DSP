/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Entry point for the Real-Time Audio DSP Application
  ******************************************************************************
  */

#include "main.h"


/* RTOS Thread Handles and Attributes ----------------------------------------*/
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name       = "defaultTask",
    .stack_size = 256 * 4,                         // 1024 bytes for audio DSP & FPU context
    .priority   = (osPriority_t)osPriorityRealtime // Hard real-time priority for DSP
};

osThreadId_t encoderTaskHandle;
const osThreadAttr_t encoderTask_attributes = {
    .name       = "encoderTask",
    .stack_size = 1024 * 4,
    .priority   = (osPriority_t)osPriorityLow       // Low priority for UI & graphics rendering
};

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* 1. Initialize Board Support Package (MCU Clocks, GPIO, SPI1, Timers, SAI1/DMA) */
    bsp_init();

    /* 2. Initialize Hardware Audio Stream (DMA circular buffers & callbacks) */
    hw_audio_init();

    /* 3. Initialize RTOS Kernel */
    osKernelInitialize();

    /* 4. Create Tasks */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
    encoderTaskHandle = osThreadNew(StartEncoderTask, NULL, &encoderTask_attributes);

    /* 5. Start Scheduler */
    osKernelStart();

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
