#ifndef TASKS_H_
#define TASKS_H_

#include <stdbool.h>

/**
 * @brief Creates the application RTOS tasks.
 * @retval true if every task was created, otherwise false.
 */
bool tasks_create(void);

#endif /* TASKS_H_ */
