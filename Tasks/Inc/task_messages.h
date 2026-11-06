#ifndef TASK_MESSAGES_H_
#define TASK_MESSAGES_H_

#include "audio_model.h"
#include "ui_model.h"
#include <stdbool.h>

typedef enum
{
    TASK_MESSAGES_OK = 0,
    TASK_MESSAGES_EMPTY,
    TASK_MESSAGES_INVALID_ARGUMENT,
    TASK_MESSAGES_ERROR
} task_messages_status_t;

/**
 * @brief Creates the queues used to exchange snapshots between the UI and audio tasks.
 * @retval true if both queues were created, otherwise false.
 */
bool task_messages_init(void);

/**
 * @brief Publishes the latest application audio-settings snapshot.
 *        The queue stores a copy; a pending older snapshot may be replaced.
 * @param settings Settings to copy into the queue.
 * @retval TASK_MESSAGES_OK if published, otherwise the relevant error status.
 */
task_messages_status_t task_messages_publish_audio_settings(const audio_settings_t *settings);

/**
 * @brief Receives the most recently published audio-settings snapshot.
 * @param settings Destination for the copied settings.
 * @retval TASK_MESSAGES_OK if a snapshot was received, TASK_MESSAGES_EMPTY if
 *         none is waiting, otherwise an error status.
 */
task_messages_status_t task_messages_receive_audio_settings(audio_settings_t *settings);

/**
 * @brief Publishes the latest audio visualization snapshot for the UI.
 *        The queue stores a copy; a pending older snapshot may be replaced.
 * @param visualization Visualization data to copy into the queue.
 * @retval TASK_MESSAGES_OK if published, otherwise the relevant error status.
 */
task_messages_status_t task_messages_publish_visualization(const ui_visualization_t *visualization);

/**
 * @brief Receives the most recently published audio visualization snapshot.
 * @param visualization Destination for the copied visualization data.
 * @retval TASK_MESSAGES_OK if a snapshot was received, TASK_MESSAGES_EMPTY if
 *         none is waiting, otherwise an error status.
 */
task_messages_status_t task_messages_receive_visualization(ui_visualization_t *visualization);

#endif /* TASK_MESSAGES_H_ */
