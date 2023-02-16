/*
 * Copyright (c) 2022 LAAS-CNRS
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
 * SPDX-License-Identifier: LGLPV2.1
 */

/**
 * @date   2022
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef SCHEDULING_COMMON_HPP_
#define SCHEDULING_COMMON_HPP_


// Stdlib
#include <stdint.h>

// Zephyr
#include <zephyr.h>

// OwnTech Power API
#include "Scheduling.h"


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

void scheduling_common_start_task(task_information_t& task_info, k_thread_entry_t entry_point);
void scheduling_common_suspend_task(task_information_t& task_info);
void scheduling_common_resume_task(task_information_t& task_info);


#endif // SCHEDULING_COMMON_HPP_
