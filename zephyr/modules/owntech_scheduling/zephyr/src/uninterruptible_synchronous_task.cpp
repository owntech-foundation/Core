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

static const struct device* timer6 = DEVICE_DT_GET(TIMER6_DEVICE);
static bool uninterruptibleTaskDefined = false;

int8_t scheduling_define_uninterruptible_synchronous_task(void (*periodic_task)(), uint32_t task_period_us)
{
	if ( (periodic_task != NULL) && (device_is_ready(timer6) == true) && (uninterruptibleTaskDefined == false) )
	{
		// Configure and start timer

		struct timer_config_t timer_cfg = {0};
		timer_cfg.timer_enable_irq   = 1;
		timer_cfg.timer_irq_callback = periodic_task;
		timer_cfg.timer_irq_t_usec   = task_period_us;

		timer_config(timer6, &timer_cfg);

		uninterruptibleTaskDefined = true;

		return 0;
	}
	else
	{
		return -1;
	}
}

void scheduling_start_uninterruptible_synchronous_task()
{
	if ( (device_is_ready(timer6) == true) && (uninterruptibleTaskDefined == true ) )
	{
		timer_start(timer6);
	}
}
