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


#ifndef TIMER_CONFIGURATION_H_
#define TIMER_CONFIGURATION_H_


#include <stdint.h>


/**
 * @brief Launches the timer4 which is adapted for reading an encoder.
 */
void timer_incremental_encoder_tim4_start();

/**
 * @brief Gets the encoder step value.
 * @return An uint32 value of the counter which corresponds to the step of the system.
 */
uint32_t timer_incremental_encoder_tim4_get_step();


#endif // TIMER_CONFIGURATION_H_
