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


// OwnTech Power API
#include "timer.h"

// Current class header
#include "Scheduling.h"


/////
// Static variables

const struct device* Scheduling::timer6 = DEVICE_DT_GET(TIMER6_DEVICE);

const int Scheduling::DEFAULT_PRIORITY = 5;

k_tid_t Scheduling::communicationThreadTid;
k_tid_t Scheduling::applicationThreadTid;

k_thread Scheduling::communicationThreadData;
k_thread Scheduling::applicationThreadData;

K_THREAD_STACK_DEFINE(communication_thread_stack, 512);
K_THREAD_STACK_DEFINE(application_thread_stack,   512);


/////
// Public object to interact with the class

Scheduling scheduling;


/////
// Private API

void Scheduling::threadEntryPoint(void* thread_function_p, void*, void*)
{
	thread_function_t thread_function = (thread_function_t)thread_function_p;
	thread_function();
}


/////
// Public API

void Scheduling::startControlTask(void (*periodic_task)(), uint32_t task_period_us)
{
	if ( (periodic_task != NULL) && (device_is_ready(timer6) == true) )
	{
		// Configure and start timer

		struct timer_config_t timer_cfg = {0};
		timer_cfg.timer_enable_irq   = 1;
		timer_cfg.timer_irq_callback = periodic_task;
		timer_cfg.timer_irq_t_usec   = task_period_us;

		timer_config(timer6, &timer_cfg);
		timer_start(timer6);
	}
}

void Scheduling::startCommunicationTask(thread_function_t routine, int priority)
{
	communicationThreadTid = k_thread_create(&communicationThreadData,
                                             communication_thread_stack,
                                             K_THREAD_STACK_SIZEOF(communication_thread_stack),
                                             threadEntryPoint,
                                             (void*)routine, NULL, NULL,
                                             priority,
                                             0,
                                             K_NO_WAIT);
}

void Scheduling::startApplicationTask(thread_function_t routine, int priority)
{
	applicationThreadTid = k_thread_create(&applicationThreadData,
                                           application_thread_stack,
                                           K_THREAD_STACK_SIZEOF(application_thread_stack),
                                           threadEntryPoint,
                                           (void*)routine, NULL, NULL,
                                           priority,
                                           0,
                                           K_NO_WAIT);
}
