/*
 * Copyright (c) 2021-present LAAS-CNRS
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
 * @date   2022
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef OWNTECH_NGND_DRIVER_H_
#define OWNTECH_NGND_DRIVER_H_


/* Public header */
#include "ngnd.h"


#define NGND_NODE DT_NODELABEL(ngnd)


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Initialize the NGND (Neutral to Ground) switch.
 *
 * This function retrieves the GPIO specification from the device tree,
 * stores it in the driver's internal data structure, and configures the
 * pin as an active output.
 *
 * - Extracts the NGND GPIO specification from the device tree.
 *
 * - Saves the GPIO descriptor to the driver's runtime data.
 *
 * - Configures the pin as an output and sets it to active.
 *
 * @param dev Pointer to the device structure.
 *
 * @return Always returns 0.
 *
 */
static int ngnd_init(const struct device* dev);


#ifdef __cplusplus
}
#endif

#endif /* OWNTECH_NGND_DRIVER_H_ */
