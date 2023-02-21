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

/**
 * @date   2023
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef HRTIM_CONFIGURATION_H_
#define HRTIM_CONFIGURATION_H_


#include <arm_math.h>
#include "hrtim.h"


/**
 * @brief     This function initializes the timer managing the legs
 * 
 * @param[in] tu1 timer unit for leg 1
 * @param[in] tu2 timer unit for leg 2
 */

void hrtim_leg_tu(hrtim_tu_t tu1, hrtim_tu_t tu2);

/**
 * @brief     This function initializes the converter in interleaved buck mode
 * with the input on the high side and the output on the low side
 */
void hrtim_init_interleaved_buck_mode();

/**
 * @brief     This function initializes the converter in interleaved buck mode
 * with the input on the high side and the output on the low side. The counting mode
 * is set to up-down (center aligned).
 */
void hrtim_init_interleaved_buck_mode_center_aligned();

/**
 * @brief     This function initializes the converter in interleaved boost mode
 * with the input on the low side and the output on the high side
 */
void hrtim_init_interleaved_boost_mode();

/**
 * @brief     This function initializes the converter in full bridge mode
 * with the input on the high side and the output on the low side with the HRTIM on left aligned
 * @param[in] SPIN_board_V_1_1_2 boolean to treat the case of the spin board V 0.9 -- Patch
*/
void hrtim_init_full_bridge_buck_mode(bool SPIN_board_V_1_1_2);

/**
 * @brief     This function initializes the converter in full bridge mode
 * with the input on the high side and the output on the low side with the HRTIM as center aligned
 * @param[in] SPIN_board_V_1_1_2 boolean to treat the case of the spin board V 0.9 -- Patch
 */
void hrtim_init_full_bridge_buck_mode_center_aligned(bool bipolar_mode, bool SPIN_board_V_1_1_2);


/**
 * @brief     This function initializes the converter in interleaved boost mode
 * with the input on the low side and the output on the high side. The counting mode
 * is set to up-down (center aligned).
 */
void hrtim_init_interleaved_boost_mode_center_aligned();

/**
 * @brief     This function initializes the converter in independent mode
 * the user must define the mode for each leg separetely
 */
void hrtim_init_independent_mode(bool leg1_buck_mode, bool leg2_buck_mode);

/**
 * @brief     This function initializes the converter in independent mode
 * the user must define the mode for each leg separetely. The counting mode
 * is set to up-down (center aligned).
 */
void hrtim_init_independent_mode_center_aligned(bool leg1_buck_mode, bool leg2_buck_mode);


/**
 * @brief initialize the converter in current mode
 * @param[in] leg1_buck  bool parameter, 0->leg1 in boost mode 1->leg1 in buck mode
 * @param[in] leg2_buck  bool parameter, 0->leg2 in boost mode 1->leg2 in buck mode
 * @param[in] leg1_tu    one of the timing unit, from TIMA to TIMF :
 *            @arg @ref   TIMA
 *            @arg @ref   TIMB
 *            @arg @ref   TIMC
 *            @arg @ref   TIMD
 *            @arg @ref   TIME
 *            @arg @ref   TIMF
 * @param[in] leg2_tu    one of the timing unit, from TIMA to TIMF :
 *            @arg @ref   TIMA
 *            @arg @ref   TIMB
 *            @arg @ref   TIMC
 *            @arg @ref   TIMD
 *            @arg @ref   TIME
 *            @arg @ref   TIMF             
*/
void hrtim_init_CurrentMode(bool leg1_buck, bool leg2_buck, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);


/**
 * @brief     This function transfer the calculated PWM value to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds with a phase shift compatible with the interleaved application.
 *
 * @param[in] duty_cycle    floating point duty cycle comprised between 0 and 1.
 */

void hrtim_interleaved_pwm_update(float32_t duty_cycle);

/**
 * @brief     This function transfer the calculated PWM value to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds with a complementary approach to the duty cycles compatible with the HBridge application.
 *
 * @param[in] duty_cycle    floating point duty cycle comprised between 0 and 1.
 */
void hrtim_full_bridge_buck_pwm_update(float32_t duty_cycle);

/**
 * @brief     This function transfer the calculated PWM value of leg_1 to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds.
 *
 * @param[in] duty_cycle    floating point duty cycle of leg_1 comprised between 0 and 1.
 */

void hrtim_leg1_pwm_update(float32_t duty_cycle);
/**
 * @brief     This function transfer the calculated PWM value of leg_2 to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds.
 *
 * @param[in] duty_cycle    floating point duty cycle of leg_2 comprised between 0 and 1.
 */

void hrtim_leg2_pwm_update(float32_t duty_cycle);

/**
 * @brief     This function updates the phase shift between the leg 1 and the master hrtim
 *
 * @param[in] phase_shift    floating point phase shift of leg_1 in degrees
 */

void hrtim_leg1_phase_shift_update(float32_t phase_shift);
/**
 * @brief     This function updates the phase shift between the leg 1 and the master hrtim
 *
 * @param[in] phase_shift    floating point phase shift of leg_2 in degrees
 */

void hrtim_leg2_phase_shift_update(float32_t phase_shift);

/**
 * @brief     This function updates the phase shift between the leg 1 and the master hrtim for the center aligned
 *
 * @param[in] phase_shift    floating point phase shift of leg_1 in degrees
 */

void hrtim_leg1_phase_shift_update_center_aligned(float32_t phase_shift);

/**
 * @brief     This function updates the phase shift between the leg 1 and the master hrtim for the center aligned
 *
 * @param[in] phase_shift    floating point phase shift of leg_2 in degrees
 */

void hrtim_leg2_phase_shift_update_center_aligned(float32_t phase_shift);

/**
 * @brief This function stops the converter by putting both timing
 * units outputs low
 */
void hrtim_stop_interleaved();

/**
 * @brief This function stops the inverter
 */
void hrtim_stop_full_bridge_buck();

/**
 * @brief This function stops only leg 1
 */
void hrtim_stop_leg1();

/**
 * @brief This function stops only leg 2
 */
void hrtim_stop_leg2();

/**
 * @brief This function starts both legs
 */
void hrtim_start_interleaved();

/**
 * @brief This function starts the inverter
 */
void hrtim_start_full_bridge_buck();

/**
 * @brief This function starts only leg 1
 */
void hrtim_start_leg1();

/**
 * @brief This function starts only leg 2
 */
void hrtim_start_leg2();

/**
 * @brief     Updates the adc trigger moment
 *
 * @param[in] new_trig    defines the triggering moment
 */
void set_adc_trig_interleaved(float32_t new_trig);

/**
 * @brief This function sets the dead time of the leg 1
 */
void hrtim_set_dead_time_leg1(uint16_t rise_ns, uint16_t fall_ns);

/**
 * @brief This function sets the dead time of the leg 2
 */
void hrtim_set_dead_time_leg2(uint16_t rise_ns, uint16_t fall_ns);

/**
 * @brief Sets the frequency of the HRTIMER
 */
void hrtim_set_frequency(uint32_t frequency_Hz);

/**
 * @brief Gets the frequency of the HRTIMER
 */
uint32_t hrtim_get_frequency();

/**
 * @brief Updates the minimum duty cycle of both legs
 */
void hrtim_set_min_duty_cycle(float32_t duty_cycle);

/**
 * @brief This updates the minimum duty cycle of both legs
 */
void hrtim_set_max_duty_cycle(float32_t duty_cycle);



#endif // HRTIM_CONFIGURATION_H_
