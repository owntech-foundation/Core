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


// Zephyr
#include "zephyr.h"
#include "drivers/gpio.h"


static struct gpio_dt_spec led_pin_spec = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static bool init = false;


void led_config_init()
{
	gpio_pin_configure_dt(&led_pin_spec, GPIO_OUTPUT_ACTIVE);
	init = true;
}

void led_config_on()
{
	if (init == false)
	{
		led_config_init();
	}

	gpio_pin_set_dt(&led_pin_spec, 1);
}

void led_config_off()
{
	if (init == false)
	{
		led_config_init();
	}

	gpio_pin_set_dt(&led_pin_spec, 0);
}

void led_config_toggle()
{
	if (init == false)
	{
		led_config_init();
	}

	gpio_pin_toggle_dt(&led_pin_spec);
}
