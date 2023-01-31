/*
 * Copyright (c) 2022-2023 LAAS-CNRS
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
 * @date   2023
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


// OwnTech Power API
#include "../src/uninterruptible_synchronous_task.hpp"
#include "../src/asynchronous_tasks.hpp"


// Current class header
#include "Scheduling.h"


/////
// Static variables

const int Scheduling::DEFAULT_PRIORITY = 5;


/////
// Public object to interact with the class

Scheduling scheduling;


/////
// Public API

// Non-interruptible control task

int8_t Scheduling::defineUninterruptibleSynchronousTask(task_function_t periodic_task, uint32_t task_period_us, scheduling_interrupt_source_t int_source)
{
	scheduling_set_uninterruptible_synchronous_task_interrupt_source(int_source);
	return scheduling_define_uninterruptible_synchronous_task(periodic_task, task_period_us);
}

void Scheduling::startUninterruptibleSynchronousTask()
{
	scheduling_start_uninterruptible_synchronous_task();
}

void Scheduling::stopUninterruptibleSynchronousTask()
{
	scheduling_stop_uninterruptible_synchronous_task();
}


// Asynchronous tasks

#ifdef CONFIG_OWNTECH_SCHEDULING_ENABLE_ASYNCHRONOUS_TASKS

int8_t Scheduling::defineAsynchronousTask(task_function_t routine)
{
	return scheduling_define_asynchronous_task(routine);
}

void Scheduling::startAsynchronousTask(uint8_t task_number)
{
	scheduling_start_asynchronous_task(task_number);
}

void Scheduling::stopAsynchronousTask(uint8_t task_number)
{
	scheduling_stop_asynchronous_task(task_number);
}

// Suspend asynchronous tasks

void Scheduling::suspendCurrentTaskMs(uint32_t duration_ms)
{
	k_sleep(K_MSEC(duration_ms));
}

void Scheduling::suspendCurrentTaskUs(uint32_t duration_us)
{
	k_sleep(K_USEC(duration_us));
}

#endif // CONFIG_OWNTECH_SCHEDULING_ENABLE_ASYNCHRONOUS_TASKS
