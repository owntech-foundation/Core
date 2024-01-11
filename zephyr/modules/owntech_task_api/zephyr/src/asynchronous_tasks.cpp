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

#ifdef CONFIG_OWNTECH_SCHEDULING_ENABLE_ASYNCHRONOUS_TASKS

#include "asynchronous_tasks.hpp"
#include "scheduling_common.hpp"


static K_THREAD_STACK_ARRAY_DEFINE(asynchronous_thread_stack, CONFIG_OWNTECH_SCHEDULING_MAX_ASYNCHRONOUS_TASKS, CONFIG_OWNTECH_SCHEDULING_ASYNCHRONOUS_TASKS_STACK_SIZE);


static task_information_t tasks_information[CONFIG_OWNTECH_SCHEDULING_MAX_ASYNCHRONOUS_TASKS];
static uint8_t task_count = 0;

static const int ASYNCHRONOUS_THREADS_PRIORITY = 14;


void _scheduling_user_asynchronous_task_entry_point(void* thread_function_p, void*, void*)
{
	while(1)
	{
		((task_function_t)thread_function_p)();
		k_yield();
	}
}

int8_t scheduling_define_asynchronous_task(task_function_t routine)
{
	if (task_count < CONFIG_OWNTECH_SCHEDULING_MAX_ASYNCHRONOUS_TASKS)
	{
		uint8_t task_number = task_count;
		task_count++;

		tasks_information[task_number].routine     = routine;
		tasks_information[task_number].priority    = ASYNCHRONOUS_THREADS_PRIORITY;
		tasks_information[task_number].task_number = task_number;
		tasks_information[task_number].stack       = asynchronous_thread_stack[task_number];
		tasks_information[task_number].stack_size  = K_THREAD_STACK_SIZEOF(asynchronous_thread_stack[task_number]);
		tasks_information[task_number].status      = task_status_t::defined;

		return task_number;
	}
	else
	{
		return -1;
	}
}

void scheduling_start_asynchronous_task(uint8_t task_number)
{
	if (task_number < task_count)
	{
		if (tasks_information[task_number].status == task_status_t::defined)
		{
			scheduling_common_start_task(tasks_information[task_number], _scheduling_user_asynchronous_task_entry_point);
			tasks_information[task_number].status = task_status_t::running;
		}
		else if (tasks_information[task_number].status == task_status_t::suspended)
		{
			scheduling_common_resume_task(tasks_information[task_number]);
			tasks_information[task_number].status = task_status_t::running;
		}
	}
}

void scheduling_stop_asynchronous_task(uint8_t task_number)
{
	if (task_number < task_count)
	{
		if (tasks_information[task_number].status == task_status_t::running)
		{
			scheduling_common_suspend_task(tasks_information[task_number]);
			tasks_information[task_number].status = task_status_t::suspended;
		}
	}
}


#endif // CONFIG_OWNTECH_SCHEDULING_ENABLE_ASYNCHRONOUS_TASKS
