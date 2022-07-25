/*
 * Copyright (c) 2021-2022 LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef HRTIM_H_
#define HRTIM_H_


#include <stdint.h>

#include <stm32_ll_hrtim.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   HRTIM have 5 or 6 timing units
 */
#ifdef HRTIM_MCR_TFCEN
#define HRTIM_STU_NUMOF (6U)        /**< number of slave timing units */
#else
#define HRTIM_STU_NUMOF (5U)
#endif

/**
 * @brief   Default HRTIM type definition
 */
#ifndef HAVE_HRTIM_T
typedef unsigned int hrtim_t;
#endif

/**
 * @brief   HRTIM timing units definition
 */
typedef enum {
    TIMA = LL_HRTIM_TIMER_A,
    TIMB = LL_HRTIM_TIMER_B,
    TIMC = LL_HRTIM_TIMER_C,
    TIMD = LL_HRTIM_TIMER_D,
    TIME = LL_HRTIM_TIMER_E,
#if (HRTIM_STU_NUMOF == 6)
    TIMF = LL_HRTIM_TIMER_F,
#endif
    MSTR = LL_HRTIM_TIMER_MASTER
} hrtim_tu_t;


/**
 * @brief   Set the duty-cycle, dead-time and phase shift for a given
 *          timing unit of a given HRTIM device
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] value         Duty cycle
 * @param[in] shift         Phase shifting
 */
void hrtim_pwm_set(hrtim_t dev, hrtim_tu_t tu, uint16_t value, uint16_t shift);



void hrtim_init_current();
void hrtim_init_voltage_buck(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_buck_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_boost(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_boost_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_leg1_buck_leg2_boost(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_leg1_buck_leg2_boost_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_leg1_boost_leg2_buck(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_leg1_boost_leg2_buck_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_update_adc_trig_interleaved(uint16_t new_trig, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);



#ifdef __cplusplus
}
#endif

#endif // HRTIM_H_
