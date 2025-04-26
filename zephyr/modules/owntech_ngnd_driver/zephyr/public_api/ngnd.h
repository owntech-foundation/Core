/*
 * Copyright (c) 2020-present LAAS-CNRS
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

/**
 * @brief   Zephyr's defines for neutral to ground gpio pin (PA0)
 * @date    2022
 * @author  Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef NGND_H_
#define NGND_H_


/* Zephyr */
#include <zephyr/device.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 *  Public Device Name
 */

#define NGND_DEVICE DT_NODELABEL(ngnd)


/**
 * @brief Set the state of the NGND (Neutral Ground) switch.
 *
 * This function controls the activation state of the NGND hardware line.
 * 
 * Passing a non-zero value activates (connects) NGND to the GND of the O2 board.
 * 
 * Zero disconnects the NGND from the GND.
 *
 * @param dev   Pointer to the NGND device structure.
 * @param value Desired state: `1` to activate, `0` to deactivate.
 */
void ngnd_set(const struct device* dev, int value);


#ifdef __cplusplus
}
#endif

#endif /* NGND_H_ */
