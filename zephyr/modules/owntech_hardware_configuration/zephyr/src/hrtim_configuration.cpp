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


// OwnTech Power API
#include "leg.h"    // PWM management layer by inverter leg interface

// Current file header
#include "HardwareConfiguration.h"


uint16_t HardwareConfiguration::hrtimPwmPeriod;
uint16_t HardwareConfiguration::hrtimPwmPhaseShift;
uint16_t HardwareConfiguration::hrtimPwmPhaseShiftLeg1;
uint16_t HardwareConfiguration::hrtimPwmPhaseShiftLeg2;
bool     HardwareConfiguration::hrtimFullBridgeBipolarMode;

hrtim_tu_t HardwareConfiguration::hrtimLeg1Tu;
hrtim_tu_t HardwareConfiguration::hrtimLeg2Tu;



void HardwareConfiguration::hrtimLegTu(hrtim_tu_t tu1, hrtim_tu_t tu2)
{
	hrtimLeg1Tu = tu1;
	hrtimLeg2Tu = tu2;
}

void HardwareConfiguration::initInterleavedBuckMode()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtimInitIndependentMode(false, true); //patch for the TWIST v0.9 - the second leg is inverted
	}else{
		hrtimInitInterleavedBuckMode();
	}
}

void HardwareConfiguration::initInterleavedBuckModeCenterAligned()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtimInitIndependentModeCenterAligned(false, true); //patch for the TWIST v0.9 - the second leg is inverted
	}else{
		hrtimInitInterleavedBuckModeCenterAligned();
	}
}

void HardwareConfiguration::initInterleavedBoostMode()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtimInitIndependentMode(true, false); //patch for the TWIST v0.9 - the second leg is inverted
	}else{
		hrtimInitInterleavedBoostMode();
	}
}

void HardwareConfiguration::initInterleavedBoostModeCenterAligned()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtimInitIndependentModeCenterAligned(true, false); //patch for the TWIST v0.9 - the second leg is inverted
	}else{
		hrtimInitInterleavedBoostModeCenterAligned();
	}
}

void HardwareConfiguration::initFullBridgeBuckMode()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtimInitFullBridgeBuckMode(true); //patch for the TWIST v0.9 - the second leg is inverted
	}else{
		hrtimInitFullBridgeBuckMode(false);
	}

}

void HardwareConfiguration::initFullBridgeBuckModeCenterAligned(inverter_modulation_t inverter_modulation_type)
{
	bool bipolar_mode;
	if (inverter_modulation_type == bipolar) bipolar_mode = true; else bipolar_mode = false;

	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtimInitFullBridgeBuckModeCenterAligned(bipolar_mode,true); //patch for the TWIST v0.9 - the first leg is inverted
	}else{
		hrtimInitFullBridgeBuckModeCenterAligned(bipolar_mode,false);
	}

}

void HardwareConfiguration::initFullBridgeBoostMode()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtimInitIndependentMode(true, false); //patch for the TWIST v1.2 - the first leg is inverted
	}else{
		hrtimInitInterleavedBoostMode();
	}
}

void HardwareConfiguration::initFullBridgeBoostModeCenterAligned()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtimInitIndependentModeCenterAligned(true, false); //patch for the TWIST v0.9 - the first leg is inverted
	}else{
		hrtimInitInterleavedBoostModeCenterAligned();
	}
}

void HardwareConfiguration::initBuckCurrentMode()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2)
	{
		hrtimInitCurrentMode(false,true,TIMA,TIMC);
		dacConfigDac3CurrentmodeInit(TIMA);
		dacConfigDac1CurrentmodeInit(TIMC);
	}
	else
	{
		hrtimInitCurrentMode(true,true,TIMA,TIMB);
		dacConfigDac3CurrentmodeInit(TIMA);
		dacConfigDac1CurrentmodeInit(TIMB);
	}

	comparatorInitialize();
}

void HardwareConfiguration::initIndependentMode(leg_operation_t leg1_operation_type, leg_operation_t leg2_operation_type)
{
	bool leg1_mode, leg2_mode;
	if (HardwareConfiguration::hardware_version == TWIST_v_1_1_2){										//patch for the TWIST v0.9 - the second leg is inverted
		if (leg1_operation_type == buck) leg1_mode = false; else leg1_mode = true;
	}else{
		if (leg1_operation_type == buck) leg1_mode = true; else leg1_mode = false;
	}

	if (leg2_operation_type == buck) leg2_mode = true; else leg2_mode = false;

	hrtimInitIndependentMode(leg1_mode, leg2_mode);
}

void HardwareConfiguration::initIndependentModeCenterAligned(leg_operation_t leg1_operation_type, leg_operation_t leg2_operation_type)
{
	bool leg1_mode, leg2_mode;
	if (HardwareConfiguration::hardware_version == TWIST_v_1_1_2){										//patch for the TWIST v0.9 - the second leg is inverted
		if (leg1_operation_type == buck) leg1_mode = false; else leg1_mode = true;
	}else{
		if (leg1_operation_type == buck) leg1_mode = true; else leg1_mode = false;
	}

	if (leg2_operation_type == buck) leg2_mode = true; else leg2_mode = false;

	hrtimInitIndependentModeCenterAligned(leg1_mode, leg2_mode);
}

void HardwareConfiguration::setInterleavedOn()
{
	powerDriverInterleavedOn();
	hrtimStartInterleaved();
}

void HardwareConfiguration::setFullBridgeBuckOn()
{
	powerDriverInterleavedOn();
	hrtimStartFullBridgeBuck();
}

void HardwareConfiguration::setLeg1On()
{
	powerDriverLeg1On();
	hrtimStartLeg1();
}

void HardwareConfiguration::setLeg2On()
{
	powerDriverLeg2On();
	hrtimStartLeg2();
}

void HardwareConfiguration::setInterleavedOff()
{
	powerDriverInterleavedOff();
	hrtimStopInterleaved();
}

void HardwareConfiguration::setFullBridgeBuckOff()
{
	powerDriverInterleavedOff();
	hrtimStopFullBridgeBuck();
}

void HardwareConfiguration::setLeg1Off()
{
	powerDriverLeg1Off();
	hrtimStopLeg1();
}

void HardwareConfiguration::setLeg2Off()
{
	powerDriverLeg2Off();
	hrtimStopLeg2();
}


/**
 * This function initializes both legs in buck mode
 */
void HardwareConfiguration::hrtimInitInterleavedBuckMode()
{
	hrtim_init_voltage_buck(hrtimLeg1Tu, hrtimLeg2Tu);

	hrtimPwmPeriod = leg_period();
	hrtimPwmPhaseShift = hrtimPwmPeriod / 2;
}

/**
 * This function initializes both legs in buck mode in up-down mode
 */
void HardwareConfiguration::hrtimInitInterleavedBuckModeCenterAligned()
{
	hrtim_init_voltage_buck_center_aligned(hrtimLeg1Tu, hrtimLeg2Tu);

	hrtimPwmPeriod = leg_period();
	hrtimPwmPhaseShift = hrtimPwmPeriod;
}

/**
 * This function initializes both legs in boost mode
 */
void HardwareConfiguration::hrtimInitInterleavedBoostMode()
{
	hrtim_init_voltage_boost(hrtimLeg1Tu, hrtimLeg2Tu);

	hrtimPwmPeriod = leg_period();
	hrtimPwmPhaseShift = hrtimPwmPeriod / 2;
}

void HardwareConfiguration::hrtimInitInterleavedBoostModeCenterAligned()
{
	hrtim_init_voltage_boost_center_aligned(hrtimLeg1Tu, hrtimLeg2Tu);

	hrtimPwmPeriod = leg_period();
	hrtimPwmPhaseShift = hrtimPwmPeriod;
}

/**
 * This leg initializes each leg independently. It receives the modes of each leg and triggers them accordingly.
 */
void HardwareConfiguration::hrtimInitIndependentMode(bool leg1_buck_mode, bool leg2_buck_mode)
{
	// High resolution timer initialization
	if (leg1_buck_mode && !leg2_buck_mode){
		hrtim_init_voltage_leg1_buck_leg2_boost(hrtimLeg1Tu, hrtimLeg2Tu);
	}
	else if (!leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_leg1_boost_leg2_buck(hrtimLeg1Tu, hrtimLeg2Tu);
	}
	else if (leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_buck(hrtimLeg1Tu, hrtimLeg2Tu);
	}
	else if (!leg1_buck_mode && !leg2_buck_mode){
		hrtim_init_voltage_boost(hrtimLeg1Tu, hrtimLeg2Tu);
	}

	hrtimPwmPeriod = leg_period();
	hrtimPwmPhaseShiftLeg1 = 0;
	hrtimPwmPhaseShiftLeg2 = hrtimPwmPeriod / 2;
	hrtimPwmPhaseShift = hrtimPwmPeriod/2;
}

/**
 * This leg initializes each leg independently. It receives the modes of each leg and triggers them accordingly.
 * The counting mode is set to up-down (center aligned).
 */
void HardwareConfiguration::hrtimInitIndependentModeCenterAligned(bool leg1_buck_mode, bool leg2_buck_mode)
{
	// High resolution timer initialization
	if (leg1_buck_mode && !leg2_buck_mode){
		hrtim_init_voltage_leg1_buck_leg2_boost_center_aligned(hrtimLeg1Tu, hrtimLeg2Tu);
	}
	else if (!leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_leg1_boost_leg2_buck_center_aligned(hrtimLeg1Tu, hrtimLeg2Tu);
	}
	else if (leg1_buck_mode && leg2_buck_mode){
		hrtim_init_voltage_buck_center_aligned(hrtimLeg1Tu, hrtimLeg2Tu);
	}
	else if (!leg1_buck_mode && !leg2_buck_mode){
		hrtim_init_voltage_boost_center_aligned(hrtimLeg1Tu, hrtimLeg2Tu);
	}

	hrtimPwmPeriod = leg_period();
	hrtimPwmPhaseShift = hrtimPwmPeriod;
}

/**
 * This function initializes both legs in full-bridge mode
 */
void HardwareConfiguration::hrtimInitFullBridgeBuckMode(bool SPIN_board_V_1_1_2)
{
	if(SPIN_board_V_1_1_2){
		hrtim_init_voltage_leg1_boost_leg2_buck(hrtimLeg1Tu, hrtimLeg2Tu); //patch for the spin v0.9 - the second leg is inverted
	}else{
		hrtim_init_voltage_buck(hrtimLeg1Tu, hrtimLeg2Tu);
	}
	hrtimFullBridgeBipolarMode = false; //left-aligned inverter is always on unipolar mode

	hrtimPwmPeriod = leg_period();
	hrtimPwmPhaseShift = hrtimPwmPeriod / 2;

}

/**
 * This function initializes both legs in full-bridge mode
 */
void HardwareConfiguration::hrtimInitFullBridgeBuckModeCenterAligned(bool bipolar_mode,bool SPIN_board_V_1_1_2)
{
	if(SPIN_board_V_1_1_2){
		hrtim_init_voltage_leg1_boost_leg2_buck_center_aligned(hrtimLeg1Tu, hrtimLeg2Tu); //patch for the spin v0.9 - the second leg is inverted
	}else{
		hrtim_init_voltage_buck_center_aligned(hrtimLeg1Tu, hrtimLeg2Tu);
	}

	hrtimFullBridgeBipolarMode = bipolar_mode;

	hrtimPwmPeriod = leg_period();

	if (bipolar_mode){
		hrtimPwmPhaseShift = 0;
	}else{
		hrtimPwmPhaseShift = hrtimPwmPeriod;
	}

}

/**
 * This function initializes both legs in Current Mode configuration
 */
void HardwareConfiguration::hrtimInitCurrentMode(bool leg1_buck, bool leg2_buck, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	hrtim_init_current(leg1_buck, leg2_buck, leg1_tu, leg2_tu);
	hrtimPwmPeriod = leg_period();
	hrtimPwmPhaseShift = 0;
	CM_leg_set(leg1_tu, 0);
	CM_leg_set(leg2_tu, hrtimPwmPhaseShift);
}


/**
 * This function transfer the calculated PWM value to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds
 */

void HardwareConfiguration::setInterleavedDutyCycle(float32_t pwm_duty_cycle)
{
	uint16_t pwm_pulse_width;

	pwm_pulse_width = (pwm_duty_cycle * hrtimPwmPeriod);
	leg_set(hrtimLeg1Tu, pwm_pulse_width, 0);
	leg_set(hrtimLeg2Tu, pwm_pulse_width, hrtimPwmPhaseShift);

}

/**
 * This function transfer the calculated PWM value to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds
 */

void HardwareConfiguration::setFullBridgeBuckDutyCycle(float32_t pwm_duty_cycle)
{
	uint16_t pwm_pulse_width;
	uint16_t pwm_reverse_pulse_width;

	// TESTING PWM VALUE TO AVOID OVERFLOW AND PWM UPDATE//
	if(hrtimFullBridgeBipolarMode)
	{
		pwm_pulse_width = (pwm_duty_cycle * hrtimPwmPeriod);
		pwm_reverse_pulse_width = (1-pwm_duty_cycle) * hrtimPwmPeriod;
		leg_set(hrtimLeg1Tu, pwm_pulse_width, 0);
		leg_set(hrtimLeg2Tu, pwm_reverse_pulse_width, hrtimPwmPeriod*pwm_duty_cycle);
	}
	else
	{
		pwm_pulse_width = (pwm_duty_cycle * hrtimPwmPeriod);
		pwm_reverse_pulse_width = (1-pwm_duty_cycle) * hrtimPwmPeriod;
		leg_set(hrtimLeg1Tu, pwm_pulse_width, 0);
		leg_set(hrtimLeg2Tu, pwm_reverse_pulse_width, hrtimPwmPhaseShift);
	}
}

/**
 * This function transfer the calculated PWM value of leg_1 to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds
 */
void HardwareConfiguration::setLeg1DutyCycle(float32_t pwm_duty_cycle)
{
	uint16_t pwm_pulse_width;

	pwm_pulse_width = (pwm_duty_cycle * hrtimPwmPeriod);
	leg_set(hrtimLeg1Tu, pwm_pulse_width, hrtimPwmPhaseShiftLeg1);
}

/**
 * This function transfer the calculated PWM value of leg_2 to the
 * HRTIM peripheral and make sure it is between saturation
 * bounds
 */
void HardwareConfiguration::setLeg2DutyCycle(float32_t pwm_duty_cycle)
{
	uint16_t pwm_pulse_width;

	pwm_pulse_width = (pwm_duty_cycle * hrtimPwmPeriod);
	leg_set(hrtimLeg2Tu, pwm_pulse_width, hrtimPwmPhaseShiftLeg2);
}

/**
 * This function updates the phase shift between leg 1 and hrtim master
 */
void HardwareConfiguration::setLeg1PhaseShift(float32_t phase_shift)
{
	phase_shift = int(phase_shift)%360; // modulo
	if(phase_shift<0) phase_shift = phase_shift + 360; // case of negative phase
	hrtimPwmPhaseShiftLeg1 = (uint16_t)(hrtimPwmPeriod * (phase_shift/360) );
}

/**
 * This function updates the phase shift between leg 2 and hrtim master
 */
void HardwareConfiguration::setLeg2PhaseShift(float32_t phase_shift)
{
	phase_shift = int(phase_shift)%360; // modulo
	if(phase_shift<0) phase_shift = phase_shift + 360; // case of negative phase
	hrtimPwmPhaseShiftLeg2 = (uint16_t)(hrtimPwmPeriod * (phase_shift/360) );
}

/**
 * This function updates the phase shift between leg 1 and hrtim master for the center aligned application.
 * In center aligned, the master timer has a frequency 2 times higher than the timers.
 */
void HardwareConfiguration::setLeg1PhaseShiftCenterAligned(float32_t phase_shift)
{
	phase_shift = int(phase_shift)%360; // modulo
	if(phase_shift<0) phase_shift = phase_shift + 360; // case of negative phase
	hrtimPwmPhaseShiftLeg1 = (uint16_t)(2*hrtimPwmPeriod * (phase_shift/360) );
}

/**
 * This function updates the phase shift between leg 2 and hrtim master for the center aligned application
 * In center aligned, the master timer has a frequency 2 times higher than the timers.
 */
void HardwareConfiguration::setLeg2PhaseShiftCenterAligned(float32_t phase_shift)
{
	phase_shift = int(phase_shift)%360; // modulo
	if(phase_shift<0) phase_shift = phase_shift + 360; // case of negative phase
	hrtimPwmPhaseShiftLeg2 = (uint16_t)(2*hrtimPwmPeriod * (phase_shift/360) );
}

/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void HardwareConfiguration::hrtimStopInterleaved()
{
	leg_stop(hrtimLeg1Tu);
	leg_stop(hrtimLeg2Tu);
}

/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void HardwareConfiguration::hrtimStopFullBridgeBuck()
{
	leg_stop(hrtimLeg1Tu);
	leg_stop(hrtimLeg2Tu);
}

/**
 * This stops only leg 1
 */
void HardwareConfiguration::hrtimStopLeg1()
{
	leg_stop(hrtimLeg1Tu);
}

/**
 * This stops only leg 2
 */
void HardwareConfiguration::hrtimStopLeg2()
{
	leg_stop(hrtimLeg2Tu);
}

/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void HardwareConfiguration::hrtimStartInterleaved()
{
	leg_start(hrtimLeg1Tu);
	leg_start(hrtimLeg2Tu);
}

/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void HardwareConfiguration::hrtimStartFullBridgeBuck()
{
	leg_start(hrtimLeg1Tu);
	leg_start(hrtimLeg2Tu);
}

/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void HardwareConfiguration::hrtimStartLeg1()
{
	leg_start(hrtimLeg1Tu);
}

/**
 * This stops the converter by putting both timing
 * units outputs low
 */
void HardwareConfiguration::hrtimStartLeg2()
{
	leg_start(hrtimLeg2Tu);
}

void HardwareConfiguration::setHrtimAdcTrigInterleaved(float32_t new_trig)
{
	uint16_t new_trig_int = new_trig * hrtimPwmPeriod;
	hrtim_update_adc_trig_interleaved(new_trig_int, hrtimLeg1Tu, hrtimLeg2Tu);
}

/**
 * This updates the dead time of the leg 1

 */
void HardwareConfiguration::setLeg1DeadTime(uint16_t rise_ns, uint16_t fall_ns)
{
	leg_set_dt(hrtimLeg1Tu, rise_ns, fall_ns);
}

/**
 * This updates the dead time of the leg 2
 */
void HardwareConfiguration::setLeg2DeadTime(uint16_t rise_ns, uint16_t fall_ns)
{
	leg_set_dt(hrtimLeg2Tu, rise_ns, fall_ns);
}

/**
 * This sets the frequency of the HRTIMER
 */
void HardwareConfiguration::setHrtimFrequency(uint32_t frequency_Hz)
{
	leg_set_freq(frequency_Hz);
}

/**
 * This gets the frequency of the HRTIMER
 */
uint32_t HardwareConfiguration::getHrtimFrequency()
{
	return leg_get_freq();
}

/**
 * This updates the minimum duty cycle of both legs
 */
void HardwareConfiguration::setHrtimMinDutyCycle(float32_t duty_cycle)
{
	leg_set_min_duty_cycle(duty_cycle);
}

/**
 * This updates the minimum duty cycle of both legs
 */
void HardwareConfiguration::setHrtimMaxDutyCycle(float32_t duty_cycle)
{
	leg_set_max_duty_cycle(duty_cycle);
}
