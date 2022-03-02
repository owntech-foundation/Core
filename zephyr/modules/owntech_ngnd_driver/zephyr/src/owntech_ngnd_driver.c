/*
 * Copyright (c) 2020-2022 LAAS-CNRS
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
 * @brief  Owntech board neutral to ground gpio pin implementation
 * @date   2022
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


// Zephyr
#include <drivers/gpio.h>

// Current file header
#include "owntech_ngnd_driver.h"


/////
// Private functions

static struct owntech_ngnd_driver_dev_data
{
	struct gpio_dt_spec ngnd_pin_spec;
} data;

static int ngnd_init(const struct device* dev)
{
	struct owntech_ngnd_driver_dev_data* data = dev->data;

	const struct gpio_dt_spec ngnd_spec = GPIO_DT_SPEC_GET(DT_CHILD(NGND_NODE, ngnd_gpio_pin), gpios);
	data->ngnd_pin_spec = ngnd_spec;

	gpio_pin_configure_dt(&data->ngnd_pin_spec, GPIO_OUTPUT_ACTIVE);

	return 0;
}


/////
// Public API

void ngnd_set(const struct device* dev, int value)
{
	struct owntech_ngnd_driver_dev_data* data = dev->data;

	gpio_pin_set_dt(&data->ngnd_pin_spec, value);
}


//////
// Device definition

#if DT_NODE_HAS_STATUS(NGND_NODE, okay)

DEVICE_DT_DEFINE(NGND_NODE,
                 ngnd_init,
                 NULL,
                 &data,
                 NULL,
                 PRE_KERNEL_2,
                 CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                 NULL
                );

#endif
