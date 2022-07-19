/*
 * Copyright (c) 2020 LAAS-CNRS
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
 * @ingroup     owntech_modules_leg
 * @file
 *
 * @brief   leg_config array definition
 * @date    2020
 * @author  Hugues Larrive <hugues.larrive@laas.fr>
 *
 * @note    This must only be included in owntech_leg_driver.c
 */

#ifndef OWNTECH_LEG_DRIVER_H_
#define OWNTECH_LEG_DRIVER_H_

/**
 * @brief   Inverter leg configuration
 */
static leg_conf_t leg_config[] = {
    {
        .hrtim = 0,
        .timing_unit = TIMA,
        .rise_dead_time = LEG_DEFAULT_DT,
        .fall_dead_time = LEG_DEFAULT_DT
    },
    {
        .hrtim = 0,
        .timing_unit = TIMB,
        .rise_dead_time = LEG_DEFAULT_DT,
        .fall_dead_time = LEG_DEFAULT_DT
    },
    {
        .hrtim = 0,
        .timing_unit = TIMC,
        .rise_dead_time = LEG_DEFAULT_DT,
        .fall_dead_time = LEG_DEFAULT_DT
    },
    {
        .hrtim = 0,
        .timing_unit = TIMD,
        .rise_dead_time = LEG_DEFAULT_DT,
        .fall_dead_time = LEG_DEFAULT_DT
    },
    {
        .hrtim = 0,
        .timing_unit = TIME,
        .rise_dead_time = LEG_DEFAULT_DT,
        .fall_dead_time = LEG_DEFAULT_DT
    },
#if defined(HRTIM_MCR_TFCEN)
    {
        .hrtim = 0,
        .timing_unit = TIMF,
        .rise_dead_time = LEG_DEFAULT_DT,
        .fall_dead_time = LEG_DEFAULT_DT
    },
#endif
};

#define LEG_NUMOF           ARRAY_SIZE(leg_config)

#endif // OWNTECH_LEG_DRIVER_H_
