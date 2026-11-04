#include "audio_task.h"
#include "hw_audio.h"
#include "dsp_effects.h"
#include "dsp_fft.h"

void StartDefaultTask(void *argument)
{
    (void)argument;
    uint32_t flags;
    static int32_t mono_samples[HALF_BUFFER_SIZE / 2];

    // Initialize DSP effect buffers and LFO states
    dsp_effects_init();
    
    // Initialize FFT precomputations
    dsp_fft_init();

    for(;;)
    {
        // Wait until DMA fills either the first half or the second half of rxAudioBuffer
        flags = osThreadFlagsWait(AUDIO_FLAG_HALF_COMPLETE | AUDIO_FLAG_FULL_COMPLETE, osFlagsWaitAny, osWaitForever);

        // Assign input and output pointers based on which half of the circular DMA buffer is ready
        int32_t *pIn  = (flags & AUDIO_FLAG_HALF_COMPLETE) ? &rxAudioBuffer[0] : &rxAudioBuffer[HALF_BUFFER_SIZE];
        int32_t *pOut = (flags & AUDIO_FLAG_HALF_COMPLETE) ? &txAudioBuffer[0] : &txAudioBuffer[HALF_BUFFER_SIZE];

        int32_t max_block_val = 0;

        // Process stereo frame block (each stereo block contains Left and Right samples consecutively)
        for (uint16_t i = 0; i < HALF_BUFFER_SIZE; i += 2)
        {
            int32_t sample = pIn[i]; // Read 24-bit Left channel sample from microphone
            mono_samples[i / 2] = sample; // Store in temporary array for FFT visualizer

            // Track block peak value for the VU Meter visualizer
            int32_t abs_vu = sample;
            if (abs_vu < 0) abs_vu = -abs_vu;
            if (abs_vu > max_block_val) max_block_val = abs_vu;

            // Apply DSP processing on Left sample and write out to stereo channels
            int32_t out_l = 0;
            int32_t out_r = 0;
            dsp_process_sample(sample, &out_l, &out_r);

            pOut[i]     = out_l; // Left channel output
            pOut[i + 1] = out_r; // Right channel output
        }

        // Update global VU level for UI visualizer task
        global_vu_level = max_block_val;
        
        // CPU Optimization: Calculate FFT only once every 8 frames (~32 ms),
        // since OLED refreshes at 50 ms. This prevents CPU starvation
        // when compiling with no optimizations (-O0).
        static uint16_t fft_throttle_cnt = 0;
        fft_throttle_cnt++;
        if (fft_throttle_cnt >= 8)
        {
            fft_throttle_cnt = 0;
            dsp_fft_process(mono_samples);
        }
    }
}
