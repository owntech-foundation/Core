/*
 * Copyright (c) 2022-2024 LAAS-CNRS
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
 * @date   2024
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


// Current module
#include "scheduling_common.hpp"

// OwnTech Power API
#include "timer.h"
#include "hrtim.h"
#include "SpinAPI.h"
#include "data_api_internal.h"
#include "safety_internal.h"
#include "SafetyAPI.h"

/* size of stack area used by error thread */
#define STACKSIZE 512
#define PRIORITY 0

void thread_error(void *, void *, void *);

K_THREAD_DEFINE(thread_error_id, STACKSIZE, thread_error, NULL, NULL, NULL,
				PRIORITY, 0, 0);

/////
// Local variables and constants

// Timer device
static const struct device* timer6 = DEVICE_DT_GET(TIMER6_DEVICE);

// Task status
static task_status_t uninterruptibleTaskStatus = task_status_t::inexistent;

// Interrupt source
static scheduling_interrupt_source_t interrupt_source = source_uninitialized;

// For HRTIM interrupts
static task_function_t user_periodic_task = NULL;

// Data dispatch
static bool do_data_dispatch = false;
static uint32_t task_period = 0;

// Safety
static bool safety_alert = false;

/////
// Private API

void thread_error(void *, void *, void *)
{
	while (1)
	{
		if (safety_alert)
		{
			printk("SAFETY ERROR : reset the MCU \n"
					"problem with I1 : %s \n"
					"problem with V1 : %s \n"
					"problem with I2 : %s \n"
					"problem with V2 : %s \n"
					"problem with Ihigh : %s \n"
					"problem with Vhigh : %s \n"
					, Safety.getChannelError(I1_LOW) ? "true" : "false"
					, Safety.getChannelError(V1_LOW) ? "true" : "false"
					, Safety.getChannelError(I2_LOW) ? "true" : "false"
					, Safety.getChannelError(I2_LOW) ? "true" : "false"
					, Safety.getChannelError(I_HIGH) ? "true" : "false"
					, Safety.getChannelError(V_HIGH) ? "true" : "false");

		}

		k_msleep(200);
	}
}

void user_task_proxy()
{
#ifdef CONFIG_OWNTECH_SAFETY_API

	if (safety_task() != 0) safety_alert = true;

#endif

	if (user_periodic_task == NULL) return;

	if (do_data_dispatch == true)
	{
		data_dispatch_do_full_dispatch();
	}

	user_periodic_task();
}

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

		task_period = task_period_us;
		user_periodic_task = periodic_task;

		// Everything OK, go on with timer configuration
		struct timer_config_t timer_cfg = {0};
		timer_cfg.timer_enable_irq   = 1;
		timer_cfg.timer_irq_callback = user_task_proxy;
		timer_cfg.timer_irq_t_usec   = task_period_us;

		timer_config(timer6, &timer_cfg);

		uninterruptibleTaskStatus = task_status_t::defined;

		return 0;
	}
	else if (interrupt_source == source_hrtim)
	{
		uint32_t hrtim_period_us = hrtim_period_Master_get_us();

		if (hrtim_period_us == 0)
			return -1;

		if (task_period_us % hrtim_period_us != 0)
			return -1;

		uint32_t repetition = task_period_us / hrtim_period_us;

		if (repetition == 0)
			return -1;

		task_period = task_period_us;
		user_periodic_task = periodic_task;
		hrtim_PeriodicEvent_configure(MSTR, repetition, user_task_proxy);

		uninterruptibleTaskStatus = task_status_t::defined;

		return 0;
	}

	return -1;
}

void scheduling_start_uninterruptible_synchronous_task(bool manage_data_acquisition)
{
	if ( (uninterruptibleTaskStatus != task_status_t::defined) && (uninterruptibleTaskStatus != task_status_t::suspended) )
		return;

	if (interrupt_source == scheduling_interrupt_source_t::source_uninitialized)
		return;

	if ( (manage_data_acquisition == true) && (spin.data.started() == false) )
	{
		// If Data Acquisition has not been started yet,
		// then Scheduling will be in charge of data dispatch
		do_data_dispatch = true;

		// Configure Data Acquisition module
		spin.data.setDispatchMethod(DispatchMethod_t::externally_triggered);

		uint32_t repetition;
		if (interrupt_source == scheduling_interrupt_source_t::source_hrtim)
		{
			repetition = hrtim_PeriodicEvent_GetRep(MSTR);
		}
		else // (interrupt_source == scheduling_interrupt_source_t::source_tim6)
		{
			uint32_t hrtim_period_us = hrtim_period_Master_get_us();
			if (hrtim_period_us == 0)
			{
				return;
			}

			repetition = task_period / hrtim_period_us;
		}
		spin.data.setRepetitionsBetweenDispatches(repetition);

		// Then start it
		spin.data.start();
	}

	if (interrupt_source == source_tim6)
	{
		if (device_is_ready(timer6) == false)
			return;

		timer_start(timer6);

		uninterruptibleTaskStatus = task_status_t::running;
	}
	else if (interrupt_source == source_hrtim)
	{
		if (user_periodic_task == NULL)
			return;

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
