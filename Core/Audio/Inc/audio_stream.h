#ifndef AUDIO_STREAM_H_
#define AUDIO_STREAM_H_

#include <stdbool.h>
#include <stdint.h>

#define AUDIO_STREAM_BUFFER_WORDS      512U
#define AUDIO_STREAM_HALF_BUFFER_WORDS (AUDIO_STREAM_BUFFER_WORDS / 2U)

typedef enum
{
    AUDIO_STATUS_OK = 0,
    AUDIO_STATUS_INVALID_ARGUMENT,
    AUDIO_STATUS_ALREADY_STARTED,
    AUDIO_STATUS_HARDWARE_ERROR
} audio_status_t;

typedef void (*audio_stream_callback_t)(void *context);

/**
 * @brief Starts circular SAI DMA and registers callbacks for completed halves.
 *        Callbacks execute in DMA interrupt context.
 */
audio_status_t audio_stream_start(audio_stream_callback_t half_complete,
                                  audio_stream_callback_t full_complete,
                                  void *context);

const int32_t *audio_stream_rx_buffer(void);
int32_t *audio_stream_tx_buffer(void);

#endif /* AUDIO_STREAM_H_ */
