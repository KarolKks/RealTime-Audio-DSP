# CMake Project Configuration for STM32 Real-Time Audio DSP
# Architecture: ARM Cortex-M4 (STM32L476RG)
# RTOS: FreeRTOS | DSP: CMSIS-DSP


cmake_minimum_required(VERSION 3.16)

# Define the toolchain file for cross-compilation (ARM GCC)
# MUST be set before the project() command
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-none-eabi-gcc.cmake)

project(DSP_Audio_Project C ASM)

# Create the main executable target
add_executable(${PROJECT_NAME})

# Build Configuration & Preprocessor Macros

target_compile_definitions(${PROJECT_NAME} PRIVATE
    USE_HAL_DRIVER       # Enable STM32 Hardware Abstraction Layer
    STM32L476xx          # Target MCU definition for HAL and CMSIS
    ARM_MATH_CM4         # Enable specific optimizations for Cortex-M4 in CMSIS-DSP
)

# Include Directories (Header files)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L4xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
    
    # FreeRTOS Includes
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
)

# Source Files Configuration

target_sources(${PROJECT_NAME} PRIVATE
    # --- Core Application & Modular Code ---
    Src/main.c
    Src/audio_task.c          # Audio processing thread (High Priority)
    Src/ui_task.c             # OLED & Encoders thread (Low Priority)
    Src/dsp_effects.c         # Audio effects algorithms (RingMod, Delay, etc.)
    Src/dsp_fft.c             # Fast Fourier Transform for spectrum analysis
    Src/hw_audio.c            # SAI and DMA hardware configuration
    Src/hw_encoders.c         # Hardware timers setup for rotary encoders
    Src/hw_oled_sh1106.c      # SH1106 OLED display SPI driver

    # --- System & Interrupts ---
    Src/freertos.c
    Src/stm32l4xx_it.c
    Src/stm32l4xx_hal_msp.c
    Src/stm32l4xx_hal_timebase_tim.c
    Src/syscalls.c
    Src/sysmem.c
    Src/system_stm32l4xx.c
    
    # --- Assembly Startup Code ---
    Src/startup_stm32l476rgtx.s

    # --- FreeRTOS Kernel ---
    Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    Middlewares/Third_Party/FreeRTOS/Source/list.c
    Middlewares/Third_Party/FreeRTOS/Source/queue.c
    Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    Middlewares/Third_Party/FreeRTOS/Source/timers.c
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c

    # --- STM32 HAL Drivers ---
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sai.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c
)


# Libraries and Linker Configuration

# Provide the path to the precompiled CMSIS-DSP static library
target_link_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Lib/GCC
)

# Link the required libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE
    arm_cortexM4lf_math  # Precompiled CMSIS-DSP library (libarm_cortexM4lf_math.a)
    m                    # Standard math library (libm) for powf, cosf, sinf, sqrtf
)

# Compiler & Linker flags for the final executable
target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/STM32L476RGTX_FLASH.ld   # Linker script path
    -Wl,-Map=${PROJECT_NAME}.map                   # Generate memory map file
    --specs=nano.specs                             # Use Newlib-nano to reduce code size
    -Wl,--gc-sections                              # Garbage collect unused sections
    -u _printf_float                               # Enable floating-point support in printf()
)

# Automatically generate .bin and .hex files for flashing, and display memory usage
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}
    COMMENT "Building raw binary and hex files, printing memory usage..."
)