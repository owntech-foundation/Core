/*
 * Copyright (c) 2020-2021 LAAS-CNRS
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
 * @brief   Owntech board neutral to ground gpio pin implementation
 *
 * @author  Clément Foucher <clement.foucher@laas.fr>
 */


// Zephyr
#include <drivers/gpio.h>

// Current file header
#include "owntech_ngnd_driver.h"


/////
// Private functions

static struct owntech_ngnd_driver_dev_data
{
	const struct device* gpio_dev;
} data;

static int ngnd_init(const struct device* dev)
{
    struct owntech_ngnd_driver_dev_data* data = dev->data;

	data->gpio_dev = device_get_binding(NGND_GPIO_LABEL);

	__ASSERT(data->gpio_dev, "ERROR: unable to obtain ngnd device binding");

    gpio_pin_configure(data->gpio_dev, NGND_GPIO_PIN, GPIO_OUTPUT_ACTIVE | NGND_GPIO_FLAGS);

	return 0;
}


/////
// Public API

void ngnd_set(const struct device* dev, int value)
{
	struct owntech_ngnd_driver_dev_data* data = dev->data;

	gpio_pin_set(data->gpio_dev, NGND_GPIO_PIN, value);
}


//////
// Device definition

DEVICE_DEFINE(owntech_ngnd_driver,
              NGND_LABEL,
              ngnd_init,
              device_pm_control_nop,
              &data,
              NULL,
              PRE_KERNEL_2,
              CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
              NULL
             );
