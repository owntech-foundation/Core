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
 * @defgroup    owntech_modules_leg OwnTech's leg module
 * @ingroup     owntech_modules
 * @brief       OwnTech PWM management layer by inverter leg
 *
 * @file
 * @brief   PWM management layer by inverter leg interface definitions
 * @date    2022
 * @author  Hugues Larrive <hugues.larrive@laas.fr>
 * @author  Antoine Boche <antoine.boche@laas.fr>
 * @author  Luiz Villa <luiz.villa@laas.fr>
 * @author  Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef LEG_H_
#define LEG_H_

#include <assert.h>
#include <stdint.h>

#include <zephyr.h>

#include "hrtim.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEG_DEFAULT_DT  (100U)      /**< dead-time in ns */
#define LEG_FREQ        KHZ(200U)   /**< frequency in Hz*/

/**
 * @brief   Inverter leg configuration data structure
 */
typedef struct {
    hrtim_t hrtim;              /**< HRTIM device */
    hrtim_tu_t timing_unit;     /**< Timing unit */
    uint16_t rise_dead_time;    /**< Rising Edge Dead time */
    uint16_t fall_dead_time;    /**< Falling Edge Dead time */
    uint16_t pulse_width;       /**< Pulse width */
} leg_conf_t;

/**
 * @brief   Initializes all the configured devices with the chosen switch convention
 *
 * @param[in]   leg1_upper_switch_convention    Choice of the switch convention for leg 1
 * @param[in]   leg2_upper_switch_convention    Choice of the switch convention for leg 2 
 *
 * @return                      HRTIM period
 */
uint16_t leg_init(bool leg1_upper_switch_convention, bool leg2_upper_switch_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);

/**
 * @brief   Initializes all the configured devices with up-down mode and the chosen switch convention
 *
 * @param[in]   leg1_upper_switch_convention    Choice of the switch convention for leg 1
 * @param[in]   leg2_upper_switch_convention    Choice of the switch convention for leg 2 
 *
 * @return                      HRTIM period
 */
uint16_t leg_init_center_aligned(bool leg1_upper_switch_convention, bool leg2_upper_switch_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);

/**
 * @brief   Set the PWM pulse width for a given leg device
 *
 * @param[in]   timing_unit     timing_unit from TIMA to TIMF
 * @param[in]   pulse_width     pulse width to set
 * @param[in]   phase_shift     phase shift
 */
void leg_set(hrtim_tu_t timing_unit, uint16_t pulse_width, uint16_t phase_shift);

/**
 * @brief   Set the dead time for a given leg device
 *
 * @param[in]   timing_unit     timing_unit from TIMA to TIMF
 * @param[in]   rise_ns         rising edge dead time to set in nano seconds
 * @param[in]   fall_ns         falling edge dead time to set in nano seconds
 */
void leg_set_dt(hrtim_tu_t timing_unit, uint16_t rise_ns, uint16_t fall_ns);

/**
 * @brief   Stop the leg (its 2 outputs goes low)
 *
 * @param[in]   timing_unit     timing_unit from TIMA to TIMF
 */
void leg_stop(hrtim_tu_t timing_unit);

/**
 * @brief   Start the leg (its 2 outputs goes low)
 *
 * @param[in]   timing_unit     timing_unit from TIMA to TIMF
 */
void leg_start(hrtim_tu_t timing_unit); 

/**
 * @brief   period getter
 *
 * @return                      period value returned by leg_init()
 */
uint16_t leg_period(void);

/**
 * @brief   LEG_NUMOF getter
 *
 * @return                      number of configured leg devices
 */
uint8_t leg_numof(void);

/**
 * @brief   leg_config getter
 *
 * @param[in]   leg             leg from 0 to LEG_NUMOF
 *
 * @return                      the leg configuration data structure
 */
leg_conf_t leg_get_conf(uint8_t leg);

/**
 * @brief   LEG_FREQ getter
 *
 * @return                      value of LEG_FREQ in KHz
 */
uint16_t leg_get_freq(void);

#ifdef __cplusplus
}
#endif

#endif // LEG_H_
