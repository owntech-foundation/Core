/*
 * Copyright (c) 2024 LAAS-CNRS
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
 * @date 2024
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 *
 */

#ifndef SHIELD_SAFETY_H_
#define SHIELD_SAFETY_H_

/**
 * @brief This function initialize the threshold max/min values with the default value
 *        from the device tree. If there are values stored and found in the NVS they
 *        will be used instead.
 *
 * @param watch_all a boolean value, if true all the sensors will be monitored to detect faults.
 *                   If false, none of the sensors will be monitored and we can set the sensors we desire
 *                   to watch later.
 *        @arg true
 *        @arg false
*/
void safety_init_shield(bool watch_all);

#endif // SHIELD_SAFETY_H_
