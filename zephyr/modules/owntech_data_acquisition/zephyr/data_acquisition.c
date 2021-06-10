/*
 * Copyright (c) 2021 LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


/////
// OwnTech Power API includes

#include "timer.h"
#include "dma/dma.h"
#include "adc/adc.h"
#include "data_dispatch/data_dispatch.h"


/////
// Timer

#define TIMER7_LABEL DT_PROP(DT_NODELABEL(timers7), label)
static const struct device* timer7;


/////
// Private functions


/////
// Public API

void data_acquisition_init()
{
	/////
	// Initialize peripherals

	// ADC
	adc_init();

	// DMA
	dma_init();

	/////
	// Initialize data dispatch
	data_dispatch_init();

	/////
	// Configure timer for background data dispatch
	timer7 = device_get_binding(TIMER7_LABEL);
	struct timer_config_t timer_cfg =
	{
		.timer_enable_irq = 1,
		.timer_callback   = data_dispatch_do_dispatch
	};
	timer_config(timer7, &timer_cfg);
	timer_start(timer7, 25);
}
