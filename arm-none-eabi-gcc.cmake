set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)


set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")


set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -Wextra -Os -ffunction-sections -fdata-sections" CACHE INTERNAL "Opcje kompilatora C")
set(CMAKE_CXX_FLAGS "${MCU_FLAGS} -Wall -Wextra -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti" CACHE INTERNAL "Opcje kompilatora C++")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "Opcje kompilatora ASM")

set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -Wl,--gc-sections" CACHE INTERNAL "Opcje linkera")


set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)