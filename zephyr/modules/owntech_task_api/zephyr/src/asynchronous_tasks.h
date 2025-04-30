/*
 * Copyright (c) 2022-present LAAS-CNRS
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-2.1
 */

/*
 * @date   2022
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef ASYNCHRONOUSTASKS_H_
#define ASYNCHRONOUSTASKS_H_


/* Stdlib */
#include <stdint.h>

/* OwnTech Power API */
#include "TaskAPI.h"


#ifdef CONFIG_OWNTECH_TASK_ENABLE_ASYNCHRONOUS_TASKS


/**
 * @brief Define a new asynchronous task.
 *
 * Registers a user-defined task routine that can be run asynchronously
 * using a dedicated thread. Each task is assigned a unique task number.
 * The number of definable tasks is limited by 
 * `CONFIG_OWNTECH_TASK_MAX_ASYNCHRONOUS_TASKS`.
 *
 * @param routine Pointer to the function that defines the asynchronous task 
 *                behavior. The function should be of type `task_function_t` 
 *                and run an infinite loop or yield regularly via `k_yield()`.
 *
 * @return The task number (`>= 0`) on success, 
 *          or `-1` if the task limit has been reached.
 */
int8_t scheduling_define_asynchronous_task(task_function_t routine);

/**
 * @brief Start or resume an asynchronous task.
 *
 * If the task is in the `defined` state, it will be started with its own thread.
 * If the task is `suspended`, it will be resumed.
 * Calling this on a `running` task has no effect.
 *
 * @param task_number Index of the task to start, as returned by
 *        `scheduling_define_asynchronous_task()`.
 */
void scheduling_start_asynchronous_task(uint8_t task_number);

/**
 * @brief Suspend a running asynchronous task.
 *
 * If the task is currently running, it will be suspended and can later
 * be resumed with `scheduling_start_asynchronous_task()`.
 *
 * @param task_number Index of the task to stop.
 */
void scheduling_stop_asynchronous_task(uint8_t task_number);


#endif /* CONFIG_OWNTECH_TASK_ENABLE_ASYNCHRONOUS_TASKS */

#endif /* ASYNCHRONOUSTASKS_H_ */
