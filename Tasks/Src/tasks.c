#include "tasks.h"
#include "audio_task.h"
#include "ui_task.h"
#include "task_messages.h"
#include "cmsis_os.h"


bool tasks_create(void)
{
    if (!task_messages_init())
    {
        return false;
    }

    const osThreadAttr_t audio_task_attributes = {
        .name = "audioTask",
        .stack_size = 256U * 4U,
        .priority = osPriorityRealtime
    };
    const osThreadAttr_t ui_task_attributes = {
        .name = "uiTask",
        .stack_size = 1024U * 4U,
        .priority = osPriorityLow
    };

    osThreadId_t audio_task = osThreadNew(audio_task_entry, NULL, &audio_task_attributes);
    if (audio_task == NULL)
    {
        return false;
    }

    osThreadId_t ui_task = osThreadNew(ui_task_entry, NULL, &ui_task_attributes);
    return ui_task != NULL;
}
