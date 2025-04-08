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

/**
 * @date   2022
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef SCHEDULING_COMMON_H_
#define SCHEDULING_COMMON_H_


/* Stdlib */
#include <stdint.h>

/* Zephyr */
#include <zephyr/kernel.h>

/* OwnTech Power API */
#include "TaskAPI.h"


enum class task_status_t
{
	inexistent,
	defined,
	running,
	suspended
};

typedef struct
{
	task_function_t routine;
	int priority;
	uint8_t task_number;
	k_thread_stack_t* stack;
	size_t stack_size;
	k_tid_t thread_id;
	k_thread thread_data;
	task_status_t status;
} task_information_t;

/**
 * @brief Start a new Zephyr thread for an asynchronous task.
 *
 * This function creates a thread for the given task using its stack,
 * priority, and entry point. The entry point will receive the task routine
 * as its first argument.
 *
 * @param task_info   Reference to the task information structure.
 *                    Must contain valid stack, size, priority, and routine.
 * @param entry_point Function to run as the thread entry (typically a loop dispatcher).
 */
void scheduling_common_start_task(task_information_t& task_info,
								  k_thread_entry_t entry_point);

/**
 * @brief Suspend an active Zephyr thread.
 *
 * Suspends the task associated with the provided task info. The task
 * must be in a running state.
 *
 * @param task_info Reference to the task information structure for the task to suspend.
 */
void scheduling_common_suspend_task(task_information_t& task_info);


/**
 * @brief Resume a previously suspended Zephyr thread.
 *
 * Resumes a task that was suspended using `scheduling_common_suspend_task()`.
 *
 * @param task_info Reference to the task information structure for the task to resume.
 */
void scheduling_common_resume_task(task_information_t& task_info);


#endif /* SCHEDULING_COMMON_H_ */
