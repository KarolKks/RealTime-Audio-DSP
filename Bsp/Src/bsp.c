#include "bsp.h"
#include "stm32l4xx_hal.h"

void bsp_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void bsp_error_handler(void)
{
    __disable_irq();

#if defined(DEBUG) || !defined(NDEBUG)
    /* Trigger hardware breakpoint to halt execution in debugger (ST-Link / GDB) */
    __BKPT(0);
#endif

    /* Fail-Stop loop */
    while (1)
    {
    }
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        bsp_error_handler();
    }

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM            = 1;
    RCC_OscInitStruct.PLL.PLLN            = 40;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        bsp_error_handler();
    }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                       RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        bsp_error_handler();
    }
}

static void PeriphCommonClock_Config(void)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    PeriphClkInit.PeriphClockSelection       = RCC_PERIPHCLK_SAI1;
    PeriphClkInit.Sai1ClockSelection         = RCC_SAI1CLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source      = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M           = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N           = 35;
    PeriphClkInit.PLLSAI1.PLLSAI1P           = RCC_PLLP_DIV17;
    PeriphClkInit.PLLSAI1.PLLSAI1Q           = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R           = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut    = RCC_PLLSAI1_SAI1CLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        bsp_error_handler();
    }
}

void bsp_init(void)
{
    /* Reset of all peripherals, Initializes Flash interface and Systick */
    HAL_Init();

    /* Configure main MCU clocks */
    SystemClock_Config();

    /* Configure peripheral clocks (SAI1) */
    PeriphCommonClock_Config();

    /* Configure MCU & PCB peripheral interfaces */
    bsp_gpio_init();
    bsp_spi1_init();
    bsp_timers_init();
    bsp_sai_init();
}
