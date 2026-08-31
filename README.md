# RealTime-Audio-DSP: Real-Time Audio Processor on STM32L476RG

A modular, high-performance real-time digital signal processing (DSP) audio application built on the **STM32L476RG** microcontroller (ARM Cortex-M4 core with hardware FPU) using the **FreeRTOS** real-time operating system, **STM32 HAL** driver library, and optimized fixed-point and floating-point algorithms.

The project implements a standalone digital multi-effects unit (9 active DSP algorithms) featuring an integrated 16-band graphic spectrum analyzer on an OLED display and a real-time signal level meter (VU-Meter).

---

## Operating Principle and User Interface

The device functions as an autonomous, hardware-based real-time audio processor. Audio captured by a 24-bit digital MEMS microphone (INMP441) is processed in real time by the microcontroller and output directly through a 32-bit stereo digital-to-analog converter (PCM5102A).

The user controls the system via two rotary encoders with integrated pushbuttons (KY-040), with real-time graphical feedback presented on an OLED display (SH1106):

*   **Encoder 1 (Left Knob - Effect Selection)**:
    *   *Rotation*: Switches between the 9 available DSP algorithms in real time (Clean/Bypass, Robot, Distortion, Delay, Pitch Shifter, Tremolo, IIR Filter, Bitcrusher, Noise Gate).
    *   *Pushbutton*: Quickly returns to direct passthrough (Bypass) or toggles master audio output muting (Mute).
*   **Encoder 2 (Right Knob - Parameter Adjustment)**:
    *   *Rotation*: Modulates the primary parameter of the currently active effect in real time (e.g., modulation frequency in Robot mode, delay time in Echo mode, saturation gain in Distortion mode, or filter cutoff frequency).
    *   *Pushbutton*: Resets the active parameter to its default value or toggles effect sub-modes (e.g., LFO sine/square wave shape, Soft/Hard-clipping distortion curves, Low-Pass/High-Pass filter topologies).
*   **OLED Display 128x64 px (Real-Time Visualizer)**:
    *   *Top Status Bar*: Displays the active effect name (e.g., `EFEKT: ROBOT`), parameter label, precise numerical value with units (e.g., `250Hz`, `150ms`, `4x`), and a graphical progress bar.
    *   *Main Screen Area*: Renders an animated 16-band FFT frequency spectrum analyzer grouped into Bass, Mid, and Treble ranges, complete with dynamic automatic gain control (AGC) and falling peak hold indicators.
    *   *Right Edge*: Displays a vertical VU-Meter column with a dedicated hardware clipping detection marker.

---

## Key Features

*   **9 Real-Time DSP Audio Effects**:
    1.  *Clean / Bypass*: Low-noise passive passthrough with master gain control.
    2.  *Robot*: Amplitude modulation (Ring Modulator) with selectable sine/square LFO generator.
    3.  *Distortion*: Multi-segment piecewise-linear analog-style saturation (Soft-Clipping and Hard-Clipping modes).
    4.  *Echo (Delay)*: Digital feedback delay line (up to 600 ms) with adjustable delay time and feedback decay.
    5.  *Pitch Shifter*: Dual-tap circular delay line with triangular crossfading and variable pitch transposition rate.
    6.  *Tremolo / Auto-Pan*: Volume and stereo pan modulation based on a precomputed sine look-up table (LUT).
    7.  *IIR Filter*: Dual-stage 4th-order biquad filter (Linkwitz-Riley 24 dB/octave Low-Pass and High-Pass).
    8.  *Bitcrusher*: Sample rate downsampling and bit-depth decimation (retro 8-bit / 4-bit sound).
    9.  *Adjustable Noise Gate*: Linear envelope tracker with rapid attack and slow release.
*   **16-Band Spectrum Analyzer (FFT)**: Local, custom implementation of a 256-point complex Radix-2 DIT Fast Fourier Transform with logarithmic frequency bin grouping.
*   **Dynamic UI Graphics**: Real-time automatic scaling (Dynamic AGC), peak hold markers, and VU meter with digital clipping detection.

---

## Hardware Specifications and Pinout

The project is designed for the **STM32L476RG Nucleo-64** development board connected to the following peripheral modules:

| Module | Interface | STM32 Pin | Functional Description |
| :--- | :--- | :--- | :--- |
| **INMP441 Microphone** | I2S / SAI | `PB12` (SCK), `PB13` (WS), `PB15` (SD) | 24-bit MEMS digital microphone (Audio Input) |
| **PCM5102A DAC** | I2S / SAI | `PA8` (MCLK), `PA5` (SCK), `PA4` (WS), `PA7` (SD) | 32-bit stereo audio DAC (Audio Output) |
| **SH1106 OLED Display** | 4-Wire SPI | `PB12` (CS), `PC6` (RES), `PC8` (DC), `PA5` (SCK), `PA7` (MOSI) | 0.96" 128x64 px graphical display (Visualizer & Menu) |
| **Encoder 1 (Select)** | Counter TIM2 | `PA0` (CH1), `PA1` (CH2), `PB14` (Button) | KY-040 rotary encoder (Effect selection / Bypass) |
| **Encoder 2 (Adjust)** | Counter TIM3 | `PA6` (CH1), `PA7` (CH2), `PB13` (Button) | KY-040 rotary encoder (Parameter adjustment / Reset) |

---

## System Architecture and Operating Principle

The software is structured into three decoupled layers: low-level hardware drivers, DSP digital mathematical processing, and real-time task scheduling via FreeRTOS.

### 1. Hardware Audio Streaming (Circular Double-Buffered DMA)
The audio stream operates using Direct Memory Access (DMA) controllers integrated with the Serial Audio Interface (SAI) peripheral:
*   **Input (Capture)**: The DMA controller automatically captures 24-bit samples from the INMP441 microphone and transfers them into a circular buffer (`rxAudioBuffer`) of 512 words without CPU intervention.
*   **Double Buffering (Ping-Pong)**: When the DMA fills the first half of the buffer (256 words = 128 audio frame samples), a `Half-Transfer Complete` hardware interrupt is generated. While the CPU processes this first half, the DMA continues streaming input into the second half in the background. Once the second half is filled, a `Transfer Complete` interrupt is triggered.
*   **Output (Playback)**: Processed output samples in `txAudioBuffer` are simultaneously transmitted via a second DMA channel directly to the PCM5102A DAC.
*   This approach completely offloads sample transfer from the CPU, guaranteeing glitch-free, continuous audio streaming.

### 2. FreeRTOS Multitasking
System execution is coordinated by the FreeRTOS real-time kernel across two primary tasks:
*   **Audio DSP Task (`defaultTask` - Priority `osPriorityRealtime`)**: 
    The highest-priority task in the system. It remains suspended until awakened by a DMA hardware interrupt flag (half or full buffer). It immediately preempts the UI task, processes the 128-sample block (DC-block filtering, noise gate, active DSP effect dispatch, hard limiting), and writes the resulting samples to the DMA output buffer. Every 8 frames, it executes the FFT spectrum analysis routine and returns to sleep.
*   **UI and Display Task (`encoderTask` - Priority `osPriorityNormal`)**: 
    A periodic task running every 50 ms (20 Hz refresh rate). It polls the TIM2 and TIM3 quadrature encoder hardware counters, executes software switch debouncing, updates DSP parameters via descriptor callbacks, renders graphic frames in SRAM, and flushes the display buffer to the SH1106 OLED controller over SPI.

---

## Project Structure

```
├── Inc/
│   ├── main.h               # CubeMX pin mappings and hardware constants
│   ├── FreeRTOSConfig.h     # FreeRTOS scheduler configuration and heap settings
│   ├── hw_audio.h           # SAI audio bus interface, DMA buffers, and event flags
│   ├── hw_encoders.h        # Rotary encoder timers and digital debounce interface
│   ├── hw_oled_sh1106.h     # OLED SPI driver and drawing routines declarations
│   ├── dsp_effects.h        # DSP parameter declarations and effects structures
│   ├── dsp_fft.h            # Fast Fourier Transform structures and API
│   ├── audio_task.h         # Audio processing thread entry prototype
│   └── ui_task.h            # User interface thread entry prototype
├── Src/
│   ├── main.c               # System clocks, peripheral initialization, and RTOS start
│   ├── hw_audio.c           # Circular double-buffered DMA callbacks and SAI management
│   ├── hw_encoders.c        # Hardware encoder timers and debounce state machine
│   ├── hw_oled_sh1106.c     # Flash-optimized OLED SPI graphics driver (51-character font)
│   ├── dsp_effects.c        # Function pointer dispatch table and DSP effect algorithms
│   ├── dsp_fft.c            # Radix-2 DIT FFT implementation and logarithmic bin grouping
│   ├── audio_task.c         # Real-time audio frame processing loop
│   └── ui_task.c            # UI controller, table-driven descriptors, and OLED rendering
├── CMakeLists.txt           # CMake build target configuration and compiler flags
└── arm-none-eabi-gcc.cmake  # GNU ARM cross-compilation toolchain setup
```

---

## Build and Flashing Guide

The project is built using **CMake** and the **ARM GNU Toolchain** cross-compiler.

### Prerequisites
Ensure the following tools are installed and available in your system `PATH`:
*   **Arm GNU Toolchain** (`arm-none-eabi-gcc`)
*   **CMake** (version >= 3.16)
*   **Ninja** (recommended) or MinGW Makefiles

### 1. Generate Build Files
Run the following command in the project root directory:
```powershell
cmake -B build -G "Ninja"
```

### 2. Compile the Firmware
Compile the project using:
```powershell
cmake --build build
```

### 3. Flash to the STM32 Board (Nucleo)
1.  Connect the **STM32 Nucleo-L476RG** board to your PC via a USB cable. The board will appear as a USB mass storage drive (e.g., `NODE_L476RG`).
2.  Navigate to the `build/` directory and copy the binary file **`DSP_Audio_Project.bin`**.
3.  Paste the file directly onto the `NODE_L476RG` drive.
4.  The ST-Link programmer LED will flash, after which the microcontroller will automatically reset and begin executing the firmware.

---

## Project in Action

This section documents the physical hardware assembly, breadboard prototype testing setup, and live OLED graphical interface during active audio processing.
<img width="1072" height="898" alt="image" src="https://github.com/user-attachments/assets/21ce9b99-1935-4ed5-8b61-3588db4be7b1" />

<img width="570" height="676" alt="image" src="https://github.com/user-attachments/assets/bc1a7ee8-f85a-441d-a155-10e6a2a5d79e" />

<img width="558" height="664" alt="image" src="https://github.com/user-attachments/assets/8a846931-b36d-4510-aaa1-bf215e823dbf" />


