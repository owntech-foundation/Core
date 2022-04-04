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
 * @date   2022
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef HRTIM_CONFIGURATION_H_
#define HRTIM_CONFIGURATION_H_


#include <arm_math.h>


/**
 * @brief     This function initializes the converter in interleaved buck mode
 * with the input on the high side and the output on the low side
 */
void hrtim_init_interleaved_buck_mode();

/**
 * @brief     This function initializes the converter in interleaved boost mode
 * with the input on the low side and the output on the high side
 */
void hrtim_init_interleaved_boost_mode();

/**
 * @brief     This function initializes the converter in independent mode
 * the user must define the mode for each leg separetely
 */
void hrtim_init_independent_mode(bool leg1_buck_mode, bool leg2_buck_mode);

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
void hrtim_hbridge_pwm_update(float32_t duty_cycle);

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
 * @brief This function stops the converter by putting both timing
 * units outputs low
 */
void hrtim_stop_interleaved();

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
void set_adc_trig_interleaved(uint16_t new_trig);


#endif // HRTIM_CONFIGURATION_H_
