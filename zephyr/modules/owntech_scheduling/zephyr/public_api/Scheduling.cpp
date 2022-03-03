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
// Local variables

static const struct device* timer6 = NULL;


/////
// Public object to interact with the class

Scheduling scheduling;


/////
// Public API

void Scheduling::controlTaskInit(void (*periodic_task)(), uint32_t task_period_us)
{
	if (periodic_task != NULL)
	{
		// Configure timer
		timer6 = device_get_binding(TIMER6_DEVICE);

		struct timer_config_t timer_cfg = {0};
		timer_cfg.timer_enable_irq   = 1;
		timer_cfg.timer_irq_callback = periodic_task;
		timer_cfg.timer_irq_t_usec   = task_period_us;

		timer_config(timer6, &timer_cfg);
	}
}

void Scheduling::controlTaskLaunch()
{
	timer_start(timer6);
}
