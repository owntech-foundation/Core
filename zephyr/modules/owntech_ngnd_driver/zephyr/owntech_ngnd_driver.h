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
 * @author  Clément Foucher <clement.foucher@laas.fr>
 */
#ifndef OWNTECH_NGND_DRIVER_H_
#define OWNTECH_NGND_DRIVER_H_


// Public header
#include "ngnd.h"


#ifdef __cplusplus
extern "C" {
#endif


#define NGND_GPIO_NODE  DT_ALIAS(ngnd)
#define NGND_GPIO_LABEL DT_GPIO_LABEL(NGND_GPIO_NODE, gpios)
#define NGND_GPIO_PIN   DT_GPIO_PIN(NGND_GPIO_NODE, gpios)
#define NGND_GPIO_FLAGS DT_GPIO_FLAGS(NGND_GPIO_NODE, gpios)

static int ngnd_init(const struct device* dev);


#ifdef __cplusplus
}
#endif

#endif // OWNTECH_NGND_DRIVER_H_
