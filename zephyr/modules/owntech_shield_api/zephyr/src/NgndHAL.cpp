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
 */


/* Owntech driver */
#include "ngnd.h"

/* Current file header */
#include "NgndHAL.h"


static const struct device* ngnd_switch = DEVICE_DT_GET(NGND_DEVICE);


void NgndHAL::turnOn()
{
	if (device_is_ready(ngnd_switch) == true)
	{
		ngnd_set(ngnd_switch, 1);
	}
}

void NgndHAL::turnOff()
{
	if (device_is_ready(ngnd_switch) == true)
	{
		ngnd_set(ngnd_switch, 0);
	}
}
