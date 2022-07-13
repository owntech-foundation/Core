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
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


// OwnTech Power API
#include "leg.h"    // PWM management layer by inverter leg interface
#include "hrtim.h"

// Current file header
#include "hrtim_configuration.h"


/////
// Defines

// Saturation values used for the PWM duty cycle
#define LOW_DUTY 0.1
#define HIGH_DUTY 0.9

/////
// Local variables
static uint16_t pwm_period;
static uint16_t pwm_phase_shift;
static uint16_t pwm_low_pulse_width;
static uint16_t pwm_high_pulse_width;


/**
 * This function initializes both legs in buck mode
 */
void hrtim_init_interleaved_buck_mode()
{
	hrtim_init_voltage_buck();

	pwm_period = leg_period();
	pwm_phase_shift = pwm_period / 2;
	pwm_low_pulse_width = pwm_period * LOW_DUTY;
	pwm_high_pulse_width = pwm_period * HIGH_DUTY;
}

/**
 * This function initializes both legs in buck mode in up-down mode
 */
void hrtim_init_interleaved_buck_mode_center_aligned()
{
	hrtim_init_voltage_buck_center_aligned();

	pwm_period = leg_period();
	pwm_phase_shift = pwm_period;
	pwm_low_pulse_width = pwm_period * LOW_DUTY;
	pwm_high_pulse_width = pwm_period * HIGH_DUTY;
}

/**
 * This function initializes both legs in boost mode
 */
void hrtim_init_interleaved_boost_mode()
{
	hrtim_init_voltage_boost();

	pwm_period = leg_period();
	pwm_phase_shift = pwm_period / 2;
	pwm_low_pulse_width = pwm_period * LOW_DUTY;
	pwm_high_pulse_width = pwm_period * HIGH_DUTY;
}

void hrtim_init_interleaved_boost_mode_center_aligned()
{
	hrtim_init_voltage_boost_center_aligned();

	pwm_period = leg_period();
	pwm_phase_shift = pwm_period;
	pwm_low_pulse_width = pwm_period * LOW_DUTY;
	pwm_high_pulse_width = pwm_period * HIGH_DUTY;
}

/**
 * This leg initializes each leg independently. It receives the modes of each leg and triggers them accordingly.
 */
void hrtim_init_independent_mode(bool leg1_buck_mode, bool leg2_buck_mode)
{
	// High resolution timer initialization
	if (leg1_buck_mode && !leg2_buck_mode){
		hrtim_init_voltage_leg1_buck_leg2_boost();
	}
	else if (!leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_leg1_boost_leg2_buck();
	}
	else if (leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_buck();
	}
	else if (!leg1_buck_mode && !leg2_buck_mode){
		hrtim_init_voltage_boost();
	}

	pwm_period = leg_period();
	pwm_phase_shift = pwm_period / 2;
	pwm_low_pulse_width = pwm_period * LOW_DUTY;
	pwm_high_pulse_width = pwm_period * HIGH_DUTY;
}

/**
 * This leg initializes each leg independently. It receives the modes of each leg and triggers them accordingly.
 * The counting mode is set to up-down (center aligned).
 */
void hrtim_init_independent_mode_center_aligned(bool leg1_buck_mode, bool leg2_buck_mode)
{
	// High resolution timer initialization
	if (leg1_buck_mode && !leg2_buck_mode){
		hrtim_init_voltage_leg1_buck_leg2_boost_center_aligned();
	}
	else if (!leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_leg1_boost_leg2_buck_center_aligned();
	}
	else if (leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_buck_center_aligned();
	}
	else if (!leg1_buck_mode && !leg2_buck_mode){
		hrtim_init_voltage_boost_center_aligned();
	}

	pwm_period = leg_period();
	pwm_phase_shift = pwm_period;
	pwm_low_pulse_width = pwm_period * LOW_DUTY;
	pwm_high_pulse_width = pwm_period * HIGH_DUTY;
}


/**
 * This function transfer the calculated PWM value to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds
 */

void hrtim_interleaved_pwm_update(float32_t pwm_duty_cycle)
{
	uint16_t pwm_pulse_width;

	// TESTING PWM VALUE TO AVOID OVERFLOW AND PWM UPDATE//
	if (pwm_duty_cycle > HIGH_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = HIGH_DUTY;
		pwm_pulse_width = pwm_high_pulse_width;
		leg_set(TIMA, pwm_pulse_width, 0);
		leg_set(TIMB, pwm_pulse_width, pwm_phase_shift);
	}

	else if (pwm_duty_cycle < LOW_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = LOW_DUTY;
		pwm_pulse_width = pwm_low_pulse_width;
		leg_set(TIMA, pwm_pulse_width, 0);
		leg_set(TIMB, pwm_pulse_width, pwm_phase_shift);
	}

	else
	{
		pwm_pulse_width = (pwm_duty_cycle * pwm_period);
		leg_set(TIMA, pwm_pulse_width, 0);
		leg_set(TIMB, pwm_pulse_width, pwm_phase_shift);
	}

	hrtim_update_adc_trig_interleaved( (pwm_pulse_width>>1) + (pwm_pulse_width>>2));
}

/**
 * This function transfer the calculated PWM value to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds
 */

void hrtim_hbridge_pwm_update(float32_t pwm_duty_cycle)
{
	uint16_t pwm_pulse_width;
	uint16_t pwm_reverse_pulse_width;

	// TESTING PWM VALUE TO AVOID OVERFLOW AND PWM UPDATE//
	if (pwm_duty_cycle > HIGH_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = HIGH_DUTY;
		pwm_pulse_width = pwm_high_pulse_width;
		pwm_reverse_pulse_width = (1-pwm_duty_cycle) * pwm_period;
		leg_set(TIMA, pwm_pulse_width, 0);
		leg_set(TIMB, pwm_reverse_pulse_width, pwm_period*pwm_duty_cycle);
	}

	else if (pwm_duty_cycle < LOW_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = LOW_DUTY;
		pwm_pulse_width = pwm_low_pulse_width;
		pwm_reverse_pulse_width = (1-pwm_duty_cycle) * pwm_period;
		leg_set(TIMA, pwm_pulse_width, 0);
		leg_set(TIMB, pwm_reverse_pulse_width, pwm_period*pwm_duty_cycle);
	}

	else
	{
		pwm_pulse_width = (pwm_duty_cycle * pwm_period);
		pwm_reverse_pulse_width = (1-pwm_duty_cycle) * pwm_period;
		leg_set(TIMA, pwm_pulse_width, 0);
		leg_set(TIMB, pwm_reverse_pulse_width, pwm_period*pwm_duty_cycle);
	}
}


/**
 * This function transfer the calculated PWM value of leg_1 to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds
 */
void hrtim_leg1_pwm_update(float32_t pwm_duty_cycle)
{
	uint16_t pwm_pulse_width;

	// TESTING PWM VALUE TO AVOID OVERFLOW AND PWM UPDATE//
	if (pwm_duty_cycle > HIGH_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = HIGH_DUTY;
		pwm_pulse_width = pwm_high_pulse_width;
		leg_set(TIMA, pwm_pulse_width, 0);
	}

	else if (pwm_duty_cycle < LOW_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = LOW_DUTY;
		pwm_pulse_width = pwm_low_pulse_width;
		leg_set(TIMA, pwm_pulse_width, 0);
	}

	else
	{
		pwm_pulse_width = (pwm_duty_cycle * pwm_period);
		leg_set(TIMA, pwm_pulse_width, 0);
	}
}

/**
 * This function transfer the calculated PWM value of leg_2 to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds
 */
void hrtim_leg2_pwm_update(float32_t pwm_duty_cycle)
{
	uint16_t pwm_pulse_width;

	// TESTING PWM VALUE TO AVOID OVERFLOW AND PWM UPDATE//
	if (pwm_duty_cycle > HIGH_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = HIGH_DUTY;
		pwm_pulse_width = pwm_high_pulse_width;
		leg_set(TIMB, pwm_pulse_width, pwm_phase_shift);
	}

	else if (pwm_duty_cycle < LOW_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = LOW_DUTY;
		pwm_pulse_width = pwm_low_pulse_width;
		leg_set(TIMB, pwm_pulse_width, pwm_phase_shift);
	}

	else
	{
		pwm_pulse_width = (pwm_duty_cycle * pwm_period);
		leg_set(TIMB, pwm_pulse_width, pwm_phase_shift);
	}
}

/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void hrtim_stop_interleaved()
{
    leg_stop(TIMA);
    leg_stop(TIMB);
}

/**
 * This stops only leg 1
 */
void hrtim_stop_leg1()
{
    leg_stop(TIMA);
}

/**
 * This stops only leg 2
 */
void hrtim_stop_leg2()
{
    leg_stop(TIMB);
}
/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void hrtim_start_interleaved()
{
	leg_start(TIMA);
	leg_start(TIMB);
}
/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void hrtim_start_leg1()
{
	leg_start(TIMA);
}
/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void hrtim_start_leg2()
{
	leg_start(TIMB);
}

void set_adc_trig_interleaved(uint16_t new_trig)
{
	hrtim_update_adc_trig_interleaved(new_trig);
}
