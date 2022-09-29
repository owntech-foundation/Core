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
static uint16_t pwm_phase_shift_leg1;
static uint16_t pwm_phase_shift_leg2;
static uint16_t pwm_low_pulse_width;
static uint16_t pwm_high_pulse_width;
static bool     full_bridge_bipolar_mode;

static hrtim_tu_t leg1_tu;
static hrtim_tu_t leg2_tu; 


/**
 * This function initializes the timer managing each leg
 */

void hrtim_leg_tu(hrtim_tu_t tu1, hrtim_tu_t tu2)
{
	leg1_tu = tu1;
	leg2_tu = tu2;
}


/**
 * This function initializes both legs in buck mode
 */
void hrtim_init_interleaved_buck_mode()
{
	hrtim_init_voltage_buck(leg1_tu,leg2_tu);

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
	hrtim_init_voltage_buck_center_aligned(leg1_tu, leg2_tu);

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
	hrtim_init_voltage_boost(leg1_tu, leg2_tu);

	pwm_period = leg_period();
	pwm_phase_shift = pwm_period / 2;
	pwm_low_pulse_width = pwm_period * LOW_DUTY;
	pwm_high_pulse_width = pwm_period * HIGH_DUTY;
}

void hrtim_init_interleaved_boost_mode_center_aligned()
{
	hrtim_init_voltage_boost_center_aligned(leg1_tu, leg2_tu);

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
		hrtim_init_voltage_leg1_buck_leg2_boost(leg1_tu, leg2_tu);
	}
	else if (!leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_leg1_boost_leg2_buck(leg1_tu, leg2_tu);
	}
	else if (leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_buck(leg1_tu, leg2_tu);
	}
	else if (!leg1_buck_mode && !leg2_buck_mode){
		hrtim_init_voltage_boost(leg1_tu, leg2_tu);
	}

	pwm_period = leg_period();
	pwm_phase_shift_leg1 = 0;
	pwm_phase_shift_leg2 = pwm_period / 2;
	pwm_phase_shift = pwm_period/2;
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
		hrtim_init_voltage_leg1_buck_leg2_boost_center_aligned(leg1_tu, leg2_tu);
	}
	else if (!leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_leg1_boost_leg2_buck_center_aligned(leg1_tu,leg2_tu);
	}
	else if (leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_buck_center_aligned(leg1_tu, leg2_tu);
	}
	else if (!leg1_buck_mode && !leg2_buck_mode){
		hrtim_init_voltage_boost_center_aligned(leg1_tu, leg2_tu);
	}

	pwm_period = leg_period();
	pwm_phase_shift = pwm_period;
	pwm_low_pulse_width = pwm_period * LOW_DUTY;
	pwm_high_pulse_width = pwm_period * HIGH_DUTY;
}

/**
 * This function initializes both legs in full-bridge mode
 */
void hrtim_init_full_bridge_buck_mode(bool SPIN_board_V_1_1_2)
{
	if(SPIN_board_V_1_1_2){
		hrtim_init_voltage_leg1_buck_leg2_boost(leg1_tu, leg2_tu); //patch for the spin v0.9 - the second leg is inverted
	}else{
		hrtim_init_voltage_buck(leg1_tu, leg2_tu);
	}
	full_bridge_bipolar_mode = false; //left-aligned inverter is always on unipolar mode

	pwm_period = leg_period();
	pwm_low_pulse_width = pwm_period * LOW_DUTY;
	pwm_high_pulse_width = pwm_period * HIGH_DUTY;

	pwm_phase_shift = pwm_period / 2;

}

/**
 * This function initializes both legs in full-bridge mode
 */
void hrtim_init_full_bridge_buck_mode_center_aligned(bool bipolar_mode,bool SPIN_board_V_1_1_2)
{
	if(SPIN_board_V_1_1_2){
		hrtim_init_voltage_leg1_buck_leg2_boost_center_aligned(leg1_tu, leg2_tu); //patch for the spin v0.9 - the second leg is inverted
	}else{
		hrtim_init_voltage_buck_center_aligned(leg1_tu, leg2_tu);
	}

	full_bridge_bipolar_mode = bipolar_mode;

	pwm_period = leg_period();
	pwm_low_pulse_width = pwm_period * LOW_DUTY;
	pwm_high_pulse_width = pwm_period * HIGH_DUTY;

	if (bipolar_mode){
		pwm_phase_shift = 0;
	}else{
		pwm_phase_shift = pwm_period;
	}

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
		leg_set(leg1_tu, pwm_pulse_width, 0);
		leg_set(leg2_tu, pwm_pulse_width, pwm_phase_shift);
	}

	else if (pwm_duty_cycle < LOW_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = LOW_DUTY;
		pwm_pulse_width = pwm_low_pulse_width;
		leg_set(leg1_tu, pwm_pulse_width, 0);
		leg_set(leg2_tu, pwm_pulse_width, pwm_phase_shift);
	}

	else
	{
		pwm_pulse_width = (pwm_duty_cycle * pwm_period);
		leg_set(leg1_tu, pwm_pulse_width, 0);
		leg_set(leg2_tu, pwm_pulse_width, pwm_phase_shift);
	}

	hrtim_update_adc_trig_interleaved( (pwm_pulse_width>>1) + (pwm_pulse_width>>2), leg1_tu, leg2_tu);  //works only on left aligned as center aligned does not use the same comparators
}

/**
 * This function transfer the calculated PWM value to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds
 */

void hrtim_full_bridge_buck_pwm_update(float32_t pwm_duty_cycle)
{
	uint16_t pwm_pulse_width;
	uint16_t pwm_reverse_pulse_width;

	// TESTING PWM VALUE TO AVOID OVERFLOW AND PWM UPDATE//
	if(full_bridge_bipolar_mode)
	{
		if (pwm_duty_cycle > HIGH_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
		{
			pwm_duty_cycle = HIGH_DUTY;
			pwm_pulse_width = pwm_high_pulse_width;
			pwm_reverse_pulse_width = (1-pwm_duty_cycle) * pwm_period;
			leg_set(leg1_tu, pwm_pulse_width, 0);
			leg_set(leg2_tu, pwm_reverse_pulse_width, pwm_period*pwm_duty_cycle);
		}
		else if (pwm_duty_cycle < LOW_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
		{
			pwm_duty_cycle = LOW_DUTY;
			pwm_pulse_width = pwm_low_pulse_width;
			pwm_reverse_pulse_width = (1-pwm_duty_cycle) * pwm_period;
			leg_set(leg1_tu, pwm_pulse_width, 0);
			leg_set(leg2_tu, pwm_reverse_pulse_width, pwm_period*pwm_duty_cycle);
		}
		else
		{
			pwm_pulse_width = (pwm_duty_cycle * pwm_period);
			pwm_reverse_pulse_width = (1-pwm_duty_cycle) * pwm_period;
			leg_set(leg1_tu, pwm_pulse_width, 0);
			leg_set(leg2_tu, pwm_reverse_pulse_width, pwm_period*pwm_duty_cycle);
		}
	}
	else
	{
		if (pwm_duty_cycle > HIGH_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
		{
			pwm_duty_cycle = HIGH_DUTY;
			pwm_pulse_width = pwm_high_pulse_width;
			pwm_reverse_pulse_width = (1-pwm_duty_cycle) * pwm_period;
			leg_set(leg1_tu, pwm_pulse_width, 0);
			leg_set(leg2_tu, pwm_reverse_pulse_width, pwm_phase_shift);
		}
		else if (pwm_duty_cycle < LOW_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
		{
			pwm_duty_cycle = LOW_DUTY;
			pwm_pulse_width = pwm_low_pulse_width;
			pwm_reverse_pulse_width = (1-pwm_duty_cycle) * pwm_period;
			leg_set(leg1_tu, pwm_pulse_width, 0);
			leg_set(leg2_tu, pwm_reverse_pulse_width, pwm_phase_shift);
		}
		else
		{
			pwm_pulse_width = (pwm_duty_cycle * pwm_period);
			pwm_reverse_pulse_width = (1-pwm_duty_cycle) * pwm_period;
			leg_set(leg1_tu, pwm_pulse_width, 0);
			leg_set(leg2_tu, pwm_reverse_pulse_width, pwm_phase_shift);
		}
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
		leg_set(leg1_tu, pwm_pulse_width, pwm_phase_shift_leg1);
	}

	else if (pwm_duty_cycle < LOW_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = LOW_DUTY;
		pwm_pulse_width = pwm_low_pulse_width;
		leg_set(leg1_tu, pwm_pulse_width, pwm_phase_shift_leg1);
	}

	else
	{
		pwm_pulse_width = (pwm_duty_cycle * pwm_period);
		leg_set(leg1_tu, pwm_pulse_width, pwm_phase_shift_leg1);
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
		leg_set(leg2_tu, pwm_pulse_width, pwm_phase_shift_leg2);
	}

	else if (pwm_duty_cycle < LOW_DUTY) // SATURATION CONDITIONS TO AVOID DIVERGENCE.
	{
		pwm_duty_cycle = LOW_DUTY;
		pwm_pulse_width = pwm_low_pulse_width;
		leg_set(leg2_tu, pwm_pulse_width, pwm_phase_shift_leg2);
	}

	else
	{
		pwm_pulse_width = (pwm_duty_cycle * pwm_period);
		leg_set(leg2_tu, pwm_pulse_width, pwm_phase_shift_leg2);
	}
}

/**
 * This function updates the phase shift between leg 1 and hrtim master
 */
void hrtim_leg1_phase_shift_update(float32_t phase_shift)
{
	phase_shift = int(phase_shift)%360; // modulo 
    if(phase_shift<0) phase_shift = phase_shift + 360; // case of negative phase
	pwm_phase_shift_leg1 = (uint16_t)(pwm_period * (phase_shift/360) );
}


/**
 * This function updates the phase shift between leg 2 and hrtim master
 */
void hrtim_leg2_phase_shift_update(float32_t phase_shift)
{
	phase_shift = int(phase_shift)%360; // modulo 
    if(phase_shift<0) phase_shift = phase_shift + 360; // case of negative phase
	pwm_phase_shift_leg2 = (uint16_t)(pwm_period * (phase_shift/360) );
}

/**
 * This function updates the phase shift between leg 1 and hrtim master for the center aligned application.
 * In center aligned, the master timer has a frequency 2 times higher than the timers.
 */
void hrtim_leg1_phase_shift_update_center_aligned(float32_t phase_shift)
{
	phase_shift = int(phase_shift)%360; // modulo 
    if(phase_shift<0) phase_shift = phase_shift + 360; // case of negative phase
	pwm_phase_shift_leg1 = (uint16_t)(2*pwm_period * (phase_shift/360) );
}


/**
 * This function updates the phase shift between leg 2 and hrtim master for the center aligned application
 * In center aligned, the master timer has a frequency 2 times higher than the timers.
 */
void hrtim_leg2_phase_shift_update_center_aligned(float32_t phase_shift)
{
	phase_shift = int(phase_shift)%360; // modulo 
    if(phase_shift<0) phase_shift = phase_shift + 360; // case of negative phase
	pwm_phase_shift_leg2 = (uint16_t)(2*pwm_period * (phase_shift/360) );
}


/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void hrtim_stop_interleaved()
{
	leg_stop(leg1_tu);
	leg_stop(leg2_tu);
}

/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void hrtim_stop_full_bridge_buck()
{
	leg_stop(leg1_tu);
	leg_stop(leg2_tu);
}


/**
 * This stops only leg 1
 */
void hrtim_stop_leg1()
{
	leg_stop(leg1_tu);
}

/**
 * This stops only leg 2
 */
void hrtim_stop_leg2()
{
	leg_stop(leg2_tu);
}
/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void hrtim_start_interleaved()
{
	leg_start(leg1_tu);
	leg_start(leg2_tu);
}

/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void hrtim_start_full_bridge_buck()
{
	leg_start(leg1_tu);
	leg_start(leg2_tu);
}

/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void hrtim_start_leg1()
{
	leg_start(leg1_tu);
}
/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void hrtim_start_leg2()
{
	leg_start(leg2_tu);
}

void set_adc_trig_interleaved(uint16_t new_trig)
{
	hrtim_update_adc_trig_interleaved(new_trig, leg1_tu, leg2_tu);
}


/**
 * This updates the dead time of the leg 1

 */
void hrtim_set_dead_time_leg1(uint16_t rise_ns, uint16_t fall_ns)
{
	leg_set_dt(leg1_tu, rise_ns, fall_ns);
}
/**
 * This updates the dead time of the leg 2
 */
void hrtim_set_dead_time_leg2(uint16_t rise_ns, uint16_t fall_ns)
{
	leg_set_dt(leg2_tu, rise_ns, fall_ns);
}
