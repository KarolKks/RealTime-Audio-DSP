# 1. Określenie systemu docelowego (Generic oznacza "bare-metal", czyli brak systemu operacyjnego jak Linux)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 2. Wskazanie narzędzi kompilatora
# Zakładamy tutaj, że masz dodany folder z kompilatorem 'arm-none-eabi-gcc' do zmiennej środowiskowej PATH.
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# 3. Flagi specyficzne dla rdzenia Twojego mikrokontrolera (STM32L476 to Cortex-M4 z jednostką FPU)
set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

# 4. Podstawowe flagi kompilacji dla języka C, C++ i Assemblera
# -Os optymalizuje pod kątem rozmiaru kodu
# -ffunction-sections -fdata-sections pomaga linkerowi usunąć nieużywany kod (martwe sekcje)
set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -Wextra -Os -ffunction-sections -fdata-sections" CACHE INTERNAL "Opcje kompilatora C")
set(CMAKE_CXX_FLAGS "${MCU_FLAGS} -Wall -Wextra -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti" CACHE INTERNAL "Opcje kompilatora C++")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "Opcje kompilatora ASM")

# 5. Flagi linkera
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -Wl,--gc-sections" CACHE INTERNAL "Opcje linkera")

# 6. Zabezpieczenie przed testem kompilatora CMake
# Domyślnie CMake próbuje skompilować prosty program .exe żeby sprawdzić kompilator. 
# Na systemach wbudowanych to się nie uda bez pełnej konfiguracji i skryptu linkera, więc mówimy mu, żeby zbudował tylko bibliotekę statyczną jako test.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)