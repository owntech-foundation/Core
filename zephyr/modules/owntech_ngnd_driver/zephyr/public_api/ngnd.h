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
 * @brief   Zephyr's defines for neutral to ground gpio pin (PA0)
 * @date    2022
 * @author  Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef NGND_H_
#define NGND_H_


// Zephyr
#include <device.h>


#ifdef __cplusplus
extern "C" {
#endif


/////
// Public device name

#define NGND_DEVICE DT_NODELABEL(ngnd)


/////
// API

void ngnd_set(const struct device* dev, int value);


#ifdef __cplusplus
}
#endif

#endif // NGND_H_
