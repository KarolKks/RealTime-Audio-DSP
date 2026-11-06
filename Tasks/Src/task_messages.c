#include "task_messages.h"
#include "cmsis_os.h"

static osMessageQueueId_t s_audio_settings_queue;
static osMessageQueueId_t s_visualization_queue;

bool task_messages_init(void)
{
    const osMessageQueueAttr_t settings_attributes = {
        .name = "audioSettings"
    };
    const osMessageQueueAttr_t visualization_attributes = {
        .name = "visualization"
    };

    s_audio_settings_queue = osMessageQueueNew(1U, sizeof(audio_settings_t),
                                               &settings_attributes);
    if (s_audio_settings_queue == NULL)
    {
        return false;
    }

    s_visualization_queue = osMessageQueueNew(1U, sizeof(ui_visualization_t),
                                              &visualization_attributes);
    return s_visualization_queue != NULL;
}

static task_messages_status_t publish_latest(osMessageQueueId_t queue, const void *message)
{
    if (queue == NULL || message == NULL)
    {
        return TASK_MESSAGES_INVALID_ARGUMENT;
    }
    if (osMessageQueueReset(queue) != osOK)
    {
        return TASK_MESSAGES_ERROR;
    }
    return (osMessageQueuePut(queue, message, 0U, 0U) == osOK)
               ? TASK_MESSAGES_OK
               : TASK_MESSAGES_ERROR;
}

static task_messages_status_t receive_latest(osMessageQueueId_t queue, void *message)
{
    if (queue == NULL || message == NULL)
    {
        return TASK_MESSAGES_INVALID_ARGUMENT;
    }

    osStatus_t status = osMessageQueueGet(queue, message, NULL, 0U);
    if (status == osOK)
    {
        return TASK_MESSAGES_OK;
    }
    return (status == osErrorResource)
               ? TASK_MESSAGES_EMPTY
               : TASK_MESSAGES_ERROR;
}

task_messages_status_t task_messages_publish_audio_settings(
    const audio_settings_t *settings)
{
    return publish_latest(s_audio_settings_queue, settings);
}

task_messages_status_t task_messages_receive_audio_settings(
    audio_settings_t *settings)
{
    return receive_latest(s_audio_settings_queue, settings);
}

task_messages_status_t task_messages_publish_visualization(
    const ui_visualization_t *visualization)
{
    return publish_latest(s_visualization_queue, visualization);
}

task_messages_status_t task_messages_receive_visualization(
    ui_visualization_t *visualization)
{
    return receive_latest(s_visualization_queue, visualization);
}
