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
 * @date   2023
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


/* OwnTech Power API */
#include "../src/uninterruptible_synchronous_task.h"
#include "../src/asynchronous_tasks.h"


/* Current class header */
#include "TaskAPI.h"

/**
 *  Static variables
 */

const int TaskAPI::DEFAULT_PRIORITY = 5;

/**
 *  Public object to interact with the class
 */

TaskAPI task;

/* Public API */

/* Non-interruptable control task */
int8_t TaskAPI::createCritical(task_function_t periodic_task,
							   uint32_t task_period_us,
							   scheduling_interrupt_source_t int_source)
{
	scheduling_set_uninterruptible_synchronous_task_interrupt_source(int_source);

	return scheduling_define_uninterruptible_synchronous_task(periodic_task,
															  task_period_us);
}

void TaskAPI::startCritical(bool manage_data_acquisition)
{
	scheduling_start_uninterruptible_synchronous_task(manage_data_acquisition);
}

void TaskAPI::stopCritical()
{
	scheduling_stop_uninterruptible_synchronous_task();
}


/* Asynchronous tasks */

#ifdef CONFIG_OWNTECH_TASK_ENABLE_ASYNCHRONOUS_TASKS

int8_t TaskAPI::createBackground(task_function_t routine)
{
	return scheduling_define_asynchronous_task(routine);
}

void TaskAPI::startBackground(uint8_t task_number)
{
	scheduling_start_asynchronous_task(task_number);
}

void TaskAPI::stopBackground(uint8_t task_number)
{
	scheduling_stop_asynchronous_task(task_number);
}

/* Suspend asynchronous tasks */

void TaskAPI::suspendBackgroundMs(uint32_t duration_ms)
{
	k_sleep(K_MSEC(duration_ms));
}

void TaskAPI::suspendBackgroundUs(uint32_t duration_us)
{
	k_sleep(K_USEC(duration_us));
}

#endif /* CONFIG_OWNTECH_TASK_ENABLE_ASYNCHRONOUS_TASKS */
