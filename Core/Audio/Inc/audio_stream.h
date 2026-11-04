#ifndef AUDIO_STREAM_H_
#define AUDIO_STREAM_H_

#include "cmsis_os.h"
#include <stdint.h>

#define AUDIO_STREAM_BUFFER_WORDS      512U
#define AUDIO_STREAM_HALF_BUFFER_WORDS (AUDIO_STREAM_BUFFER_WORDS / 2U)

#define AUDIO_STREAM_EVENT_HALF_COMPLETE 0x01U
#define AUDIO_STREAM_EVENT_FULL_COMPLETE 0x02U

typedef enum
{
    AUDIO_STATUS_OK = 0,
    AUDIO_STATUS_INVALID_ARGUMENT,
    AUDIO_STATUS_ALREADY_STARTED,
    AUDIO_STATUS_NOT_INITIALIZED,
    AUDIO_STATUS_HARDWARE_ERROR,
    AUDIO_STATUS_RTOS_ERROR
} audio_status_t;

/**
 * @brief Starts circular SAI DMA and routes completion events to the audio task.
 */
audio_status_t audio_stream_start(osThreadId_t processing_task);

/**
 * @brief Waits for one or more completed DMA buffer halves.
 */
audio_status_t audio_stream_wait(uint32_t *events);

const int32_t *audio_stream_rx_buffer(void);
int32_t *audio_stream_tx_buffer(void);

#endif /* AUDIO_STREAM_H_ */
