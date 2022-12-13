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
#include "scheduling_common.hpp"


/////
// Local variables

// Timer device
static const struct device* timer6 = DEVICE_DT_GET(TIMER6_DEVICE);

// Task status
static task_status_t uninterruptibleTaskStatus = task_status_t::inexistent;


/////
// API

int8_t scheduling_define_uninterruptible_synchronous_task(void (*periodic_task)(), uint32_t task_period_us)
{
	if (device_is_ready(timer6) == true)
	{
		if ( (uninterruptibleTaskStatus == task_status_t::inexistent) || (uninterruptibleTaskStatus == task_status_t::suspended))
		{
			// Configure and start timer

			struct timer_config_t timer_cfg = {0};
			timer_cfg.timer_enable_irq   = 1;
			timer_cfg.timer_irq_callback = periodic_task;
			timer_cfg.timer_irq_t_usec   = task_period_us;

			timer_config(timer6, &timer_cfg);

			uninterruptibleTaskStatus = task_status_t::defined;

			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

void scheduling_start_uninterruptible_synchronous_task()
{
	if (device_is_ready(timer6) == true)
	{
		if ( (uninterruptibleTaskStatus == task_status_t::defined) || (uninterruptibleTaskStatus == task_status_t::suspended) )
		{
			timer_start(timer6);
			uninterruptibleTaskStatus = task_status_t::running;
		}
	}
}

void scheduling_stop_uninterruptible_synchronous_task()
{
	if (device_is_ready(timer6) == true)
	{
		if (uninterruptibleTaskStatus == task_status_t::running)
		{
			timer_stop(timer6);
			uninterruptibleTaskStatus = task_status_t::suspended;
		}
	}
}
