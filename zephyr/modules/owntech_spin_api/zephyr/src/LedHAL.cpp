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
 * @author Luiz Villa <luiz.villa@laas.fr>
 */


/* Zephyr */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* Current file header */
#include "LedHAL.h"


bool LedHAL::ledInitialized = false;

static struct gpio_dt_spec led_pin_spec =
							GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);


void LedHAL::initialize()
{
	gpio_pin_configure_dt(&led_pin_spec, GPIO_OUTPUT_INACTIVE);
	ledInitialized = true;
}

void LedHAL::turnOn()
{
	if (ledInitialized == false)
	{
		initialize();
	}

	gpio_pin_set_dt(&led_pin_spec, 1);
}

void LedHAL::turnOff()
{
	if (ledInitialized == false)
	{
		initialize();
	}

	gpio_pin_set_dt(&led_pin_spec, 0);
}

void LedHAL::toggle()
{
	if (ledInitialized == false)
	{
		initialize();
	}

	gpio_pin_toggle_dt(&led_pin_spec);
}
