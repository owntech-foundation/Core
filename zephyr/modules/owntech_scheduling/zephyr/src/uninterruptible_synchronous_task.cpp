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


// Current module
#include "scheduling_common.hpp"

// OwnTech Power API
#include "timer.h"
#include "leg.h"
#include "hrtim.h"


/////
// Local variables and constants

// Timer device
static const struct device* timer6 = DEVICE_DT_GET(TIMER6_DEVICE);

// Task status
static task_status_t uninterruptibleTaskStatus = task_status_t::inexistent;

// Interrupt source
static scheduling_interrupt_source_t interrupt_source = source_uninitialized;

// For HRTIM interrupts
static uint32_t repetition = 0;
static task_function_t user_periodic_task = NULL;


/////
// Public API


void scheduling_set_uninterruptible_synchronous_task_interrupt_source(scheduling_interrupt_source_t int_source)
{
	interrupt_source = int_source;
}

int8_t scheduling_define_uninterruptible_synchronous_task(task_function_t periodic_task, uint32_t task_period_us)
{
	if ( (uninterruptibleTaskStatus != task_status_t::inexistent) && (uninterruptibleTaskStatus != task_status_t::suspended))
		return -1;

	if (periodic_task == NULL)
		return -1;

	if (interrupt_source == source_tim6)
	{
		if (device_is_ready(timer6) == false)
			return -1;

		// Everything OK, go on with timer configuration
		struct timer_config_t timer_cfg = {0};
		timer_cfg.timer_enable_irq   = 1;
		timer_cfg.timer_irq_callback = periodic_task;
		timer_cfg.timer_irq_t_usec   = task_period_us;

		timer_config(timer6, &timer_cfg);

		uninterruptibleTaskStatus = task_status_t::defined;

		return 0;
	}
	else if (interrupt_source == source_hrtim)
	{
		uint32_t hrtim_period_us = leg_get_period_us();

		if (task_period_us % hrtim_period_us != 0)
			return -1;

		repetition = task_period_us / hrtim_period_us;

		if (repetition == 0)
			return -1;

		user_periodic_task = periodic_task;

		uninterruptibleTaskStatus = task_status_t::defined;

		return 0;
	}

	return -1;
}

void scheduling_start_uninterruptible_synchronous_task()
{
	if ( (uninterruptibleTaskStatus != task_status_t::defined) && (uninterruptibleTaskStatus != task_status_t::suspended) )
		return;

	if (interrupt_source == source_tim6)
	{
		if (device_is_ready(timer6) == false)
			return;

		timer_start(timer6);

		uninterruptibleTaskStatus = task_status_t::running;
	}
	else if (interrupt_source == source_hrtim)
	{
		if ( (repetition == 0) || (user_periodic_task == NULL) )
			return;

		hrtim_PeriodicEvent_configure(MSTR, repetition, user_periodic_task);
		hrtim_PeriodicEvent_en(MSTR);

		uninterruptibleTaskStatus = task_status_t::running;
	}
}

void scheduling_stop_uninterruptible_synchronous_task()
{
	if (uninterruptibleTaskStatus != task_status_t::running)
		return;

	if (interrupt_source == source_tim6)
	{
		if (device_is_ready(timer6) == false)
			return;

		timer_stop(timer6);

		uninterruptibleTaskStatus = task_status_t::suspended;
	}
	else if (interrupt_source == source_hrtim)
	{
		hrtim_PeriodicEvent_dis(MSTR);

		uninterruptibleTaskStatus = task_status_t::suspended;
	}
}
