/*
 * Copyright (c) 2021-2023 LAAS-CNRS
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

/*
 * @date   2023
 *
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

typedef void(*hrtim_callback_t)();


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



void hrtim_init_current(bool leg1_convention, bool leg2_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_buck(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_buck_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_boost(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_boost_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_leg1_buck_leg2_boost(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_leg1_buck_leg2_boost_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_leg1_boost_leg2_buck(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_init_voltage_leg1_boost_leg2_buck_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
void hrtim_update_adc_trig_interleaved(uint16_t new_trig, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);

/**
 * @brief Configure interrupt on repetition counter for the chosen timing unit
 * @param tu_src timing unit which will be the source for the ISR:
 *         @arg @ref MSTR
 *         @arg @ref TIMA
 *         @arg @ref TIMB
 *         @arg @ref TIMC
 *         @arg @ref TIMD
 *         @arg @ref TIME
 *         @arg @ref TIMF
 * @param repetition value between 1 and 256 for the repetition counter:
 *        period of the event wrt. periods of the HRTIM.
 *        E.g. when set to 10, one event will be triggered every 10 HRTIM period.
 * @param callback Pointer to a void(void) function that will be called
 *        when the event is triggerred.
 */
void hrtim_PeriodicEvent_configure(hrtim_tu_t tu_src, uint32_t repetition, hrtim_callback_t callback);

/**
 * @brief Enable interrupt on repetition counter for the chosen timing unit.
 *        The periodic event configuration must have been done previously.
 * @param tu_src timing unit which will be the source for the ISR:
 *         @arg @ref MSTR
 *         @arg @ref TIMA
 *         @arg @ref TIMB
 *         @arg @ref TIMC
 *         @arg @ref TIMD
 *         @arg @ref TIME
 *         @arg @ref TIMF
 */
void hrtim_PeriodicEvent_en(hrtim_tu_t tu_src);

/**
 * @brief Disable interrupt on repetition counter for the chosen timing unit
 * @param tu_src timing unit which will be the source for the ISR:
 *         @arg @ref MSTR
 *         @arg @ref TIMA
 *         @arg @ref TIMB
 *         @arg @ref TIMC
 *         @arg @ref TIMD
 *         @arg @ref TIME
 *         @arg @ref TIMF
 */
void hrtim_PeriodicEvent_dis(hrtim_tu_t tu_src);

/**
 * @brief Change the repetition counter value to control the ISR interrupt
 * @param tu_src timing unit which will be the source for the ISR:
 *         @arg @ref MSTR
 *         @arg @ref TIMA
 *         @arg @ref TIMB
 *         @arg @ref TIMC
 *         @arg @ref TIMD
 *         @arg @ref TIME
 *         @arg @ref TIMF
 * @param repetion value between 1 and 256 for the repetition counter:
 * period of the event wrt. periods of the HRTIM.
 * E.g. when set to 10, one event will be triggered every 10 HRTIM period.
 */
void hrtim_PeriodicEvent_SetRep(hrtim_tu_t tu_src, uint32_t repetition);

/**
 * @brief Get the current value of the repetition counter.
 * @param tu_src timing unit which will be the source for the ISR:
 *         @arg @ref MSTR
 *         @arg @ref TIMA
 *         @arg @ref TIMB
 *         @arg @ref TIMC
 *         @arg @ref TIMD
 *         @arg @ref TIME
 *         @arg @ref TIMF
 * @return Value between 1 and 256 for the repetition counter:
 * period of the event wrt. periods of the HRTIM.
 */
uint32_t hrtim_PeriodicEvent_GetRep(hrtim_tu_t tu_src);

#ifdef __cplusplus
}
#endif

#endif // HRTIM_H_
