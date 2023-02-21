/*
 * Copyright (c) 2023 LAAS-CNRS
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
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


// Current module private functions
#include "../src/dac_configuration.h"
#include "../src/ngnd_configuration.h"
#include "../src/led_configuration.h"
#include "../src/timer_configuration.h"
#include "../src/hrtim_configuration.h"
#include "../src/uart_configuration.h"
#include "../src/adc_configuration.h"
#include "../src/power_driver_configuration.h"
#include "../src/comparator_configuration.h"

// Current class header
#include "HardwareConfiguration.h"

// Include public header
#include "hrtim.h"


/////
// Public object to interact with the class

HardwareConfiguration hwConfig;

/////
// Static class members

hardware_version_t HardwareConfiguration::hardware_version = nucleo_G474RE;

/////
// Public static configuration functions


/////
// Common

/**
 * @brief Sets the version of the underlying hardware.
 *        Depending on the hardware version, some software
 *        configuration, such as pinout, has to be tweaked.
 * @param hardware_version Enum representing the hardware version.
 */
void HardwareConfiguration::setBoardVersion(hardware_version_t hardware_version)
{
	HardwareConfiguration::hardware_version = hardware_version;

	if (hardware_version == O2_v_1_1_2 || hardware_version == O2_v_0_9)
	{
		uart_lpuart1_swap_rx_tx();
		hrtim_leg_tu(TIMA, TIMB);
	}else if(hardware_version == SPIN_v_0_1){
		uart_lpuart1_swap_rx_tx();
		hrtim_leg_tu(TIMA, TIMC);
	}else if(hardware_version == SPIN_v_0_9 || hardware_version == TWIST_v_1_1_2){
		hrtim_leg_tu(TIMA, TIMC);
	}else if(hardware_version == nucleo_G474RE){
		hrtim_leg_tu(TIMA, TIMB);
	}

}


/////
// DAC

void HardwareConfiguration::initDacConstValue(uint8_t dac_number)
{
	dac_config_const_value_init(dac_number);
}

void HardwareConfiguration::setDacConstValue(uint8_t dac_number, uint8_t channel, uint32_t const_value)
{
	dac_set_const_value(dac_number, channel, const_value);
}

void HardwareConfiguration::slopeCompensationLeg1(float32_t peak_voltage, float32_t low_voltage)
{
	set_satwtooth_DAC3(peak_voltage, low_voltage);
}

void HardwareConfiguration::slopeCompensationLeg2(float32_t peak_voltage, float32_t low_voltage)
{
	set_satwtooth_DAC1(peak_voltage, low_voltage);
}


/////
// NGND

void HardwareConfiguration::setNgndOn()
{
	ngnd_config_on();
}

void HardwareConfiguration::setNgndOff()
{
	ngnd_config_off();
}


/////
// LED

void HardwareConfiguration::setLedOn()
{
	led_config_on();
}

void HardwareConfiguration::setLedOff()
{
	led_config_off();
}

void HardwareConfiguration::setLedToggle()
{
	led_config_toggle();
}


/////
// Incremental encoder

void HardwareConfiguration::startLoggingIncrementalEncoder()
{
	timer_incremental_encoder_tim4_start();
}

uint32_t HardwareConfiguration::getIncrementalEncoderValue()
{
	return timer_incremental_encoder_tim4_get_step();
}


/////
// Power converter

void HardwareConfiguration::initInterleavedBuckMode()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtim_init_independent_mode(false, true); //patch for the TWIST v0.9 - the second leg is inverted
	}else{
		hrtim_init_interleaved_buck_mode();
	}
}

void HardwareConfiguration::initInterleavedBuckModeCenterAligned()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtim_init_independent_mode_center_aligned(false, true); //patch for the TWIST v0.9 - the second leg is inverted
	}else{
		hrtim_init_interleaved_buck_mode_center_aligned();
	}
}

void HardwareConfiguration::initInterleavedBoostMode()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtim_init_independent_mode(true, false); //patch for the TWIST v0.9 - the second leg is inverted
	}else{
		hrtim_init_interleaved_boost_mode();
	}
}

void HardwareConfiguration::initInterleavedBoostModeCenterAligned()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtim_init_independent_mode_center_aligned(true, false); //patch for the TWIST v0.9 - the second leg is inverted
	}else{
		hrtim_init_interleaved_boost_mode_center_aligned();
	}
}

void HardwareConfiguration::initFullBridgeBuckMode()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtim_init_full_bridge_buck_mode(true); //patch for the TWIST v0.9 - the second leg is inverted
	}else{
		hrtim_init_full_bridge_buck_mode(false);
	}

}

void HardwareConfiguration::initFullBridgeBuckModeCenterAligned(inverter_modulation_t inverter_modulation_type)
{
	bool bipolar_mode;
	if (inverter_modulation_type == bipolar) bipolar_mode = true; else bipolar_mode = false;

	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtim_init_full_bridge_buck_mode_center_aligned(bipolar_mode,true); //patch for the TWIST v0.9 - the first leg is inverted
	}else{
		hrtim_init_full_bridge_buck_mode_center_aligned(bipolar_mode,false);
	}

}

void HardwareConfiguration::initFullBridgeBoostMode()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtim_init_independent_mode(true, false); //patch for the TWIST v1.2 - the first leg is inverted
	}else{
		hrtim_init_interleaved_boost_mode();
	}
}

void HardwareConfiguration::initFullBridgeBoostModeCenterAligned()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2){
		hrtim_init_independent_mode_center_aligned(true, false); //patch for the TWIST v0.9 - the first leg is inverted
	}else{
		hrtim_init_interleaved_boost_mode_center_aligned();
	}
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

	hrtim_init_independent_mode(leg1_mode, leg2_mode);
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

	hrtim_init_independent_mode_center_aligned(leg1_mode, leg2_mode);
}

void HardwareConfiguration::initBuckCurrentMode()
{
	if(HardwareConfiguration::hardware_version == TWIST_v_1_1_2)
	{
		hrtim_init_CurrentMode(false,true,TIMA,TIMC);
		dac_config_dac3_current_mode_init(TIMA);
		dac_config_dac1_current_mode_init(TIMC);
	}
	else
	{
		hrtim_init_CurrentMode(true,true,TIMA,TIMB);
		dac_config_dac3_current_mode_init(TIMA);
		dac_config_dac1_current_mode_init(TIMB);
	}

	comparator_initialization();
}

void HardwareConfiguration::setInterleavedDutyCycle(float32_t duty_cycle)
{
	hrtim_interleaved_pwm_update(duty_cycle);
}

void HardwareConfiguration::setFullBridgeBuckDutyCycle(float32_t duty_cycle)
{
	hrtim_full_bridge_buck_pwm_update(duty_cycle);
}

void HardwareConfiguration::setLeg1DutyCycle(float32_t duty_cycle)
{
	hrtim_leg1_pwm_update(duty_cycle);
}

void HardwareConfiguration::setLeg2DutyCycle(float32_t duty_cycle)
{
	hrtim_leg2_pwm_update(duty_cycle);
}

void HardwareConfiguration::setLeg1PhaseShift(float32_t phase_shift)
{
	hrtim_leg1_phase_shift_update(phase_shift);
}

void HardwareConfiguration::setLeg2PhaseShift(float32_t phase_shift)
{
	hrtim_leg2_phase_shift_update(phase_shift);
}

void HardwareConfiguration::setLeg1PhaseShiftCenterAligned(float32_t phase_shift)
{
	hrtim_leg1_phase_shift_update_center_aligned(phase_shift);
}

void HardwareConfiguration::setLeg2PhaseShiftCenterAligned(float32_t phase_shift)
{
	hrtim_leg2_phase_shift_update_center_aligned(phase_shift);
}


void HardwareConfiguration::setHrtimFrequency(uint32_t frequency_Hz)
{
	hrtim_set_frequency(frequency_Hz);
}

uint32_t HardwareConfiguration::getHrtimFrequency()
{
	return hrtim_get_frequency();
}

void HardwareConfiguration::setHrtimMinDutyCycle(float32_t duty_cycle)
{
	hrtim_set_min_duty_cycle(duty_cycle);
}

void HardwareConfiguration::setHrtimMaxDutyCycle(float32_t duty_cycle)
{
	hrtim_set_max_duty_cycle(duty_cycle);
}

void HardwareConfiguration::setInterleavedOn()
{
	power_driver_interleaved_on();
	hrtim_start_interleaved();
}

void HardwareConfiguration::setFullBridgeBuckOn()
{
	power_driver_interleaved_on();
	hrtim_start_full_bridge_buck();
}

void HardwareConfiguration::setLeg1On()
{
	power_driver_leg1_on();
	hrtim_start_leg1();
}

void HardwareConfiguration::setLeg2On()
{
	power_driver_leg2_on();
	hrtim_start_leg2();
}

void HardwareConfiguration::setInterleavedOff()
{
	power_driver_interleaved_off();
	hrtim_stop_interleaved();
}

void HardwareConfiguration::setFullBridgeBuckOff()
{
	power_driver_interleaved_off();
	hrtim_stop_full_bridge_buck();
}

void HardwareConfiguration::setLeg1Off()
{
	power_driver_leg1_off();
	hrtim_stop_leg1();
}

void HardwareConfiguration::setLeg2Off()
{
	power_driver_leg2_off();
	hrtim_stop_leg2();
}

void HardwareConfiguration::setHrtimAdcTrigInterleaved(float32_t new_trig)
{
	set_adc_trig_interleaved(new_trig);
}

/////
// Extra UART

void HardwareConfiguration::extraUartInit()
{
	uart_usart1_init();
}

char HardwareConfiguration::extraUartReadChar()
{
	return uart_usart1_get_data();
}

void HardwareConfiguration::extraUartWriteChar(char data)
{
	uart_usart1_write_single(data);
}


/////
// ADC

void HardwareConfiguration::configureAdc12DualMode(uint8_t dual_mode)
{
	configure_adc12_dual_mode(dual_mode);
}

int8_t HardwareConfiguration::configureAdcChannels(uint8_t adc_number, const char* channel_list[], uint8_t channel_count)
{
	return configure_adc_channels(adc_number, channel_list, channel_count);
}

void HardwareConfiguration::configureAdcTriggerSource(uint8_t adc_number, adc_ev_src_t trigger_source)
{
	configure_adc_trigger_source(adc_number, trigger_source);
}

void HardwareConfiguration::configureAdcDiscontinuousMode(uint8_t adc_number, uint32_t dicontinuous_count)
{
	configure_adc_discontinuous_mode(adc_number, dicontinuous_count);
}

void HardwareConfiguration::configureAdcDefaultAllMeasurements()
{
	configure_adc_default_all_measurements();
}

void HardwareConfiguration::configureAdcDefaultAllMeasurementsAndExtra()
{
	configure_adc_default_all_measurements_and_extra();
}


void HardwareConfiguration::setLeg1DeadTime(uint16_t rise_ns, uint16_t fall_ns)
{
	hrtim_set_dead_time_leg1(rise_ns, fall_ns);
}

void HardwareConfiguration::setLeg2DeadTime(uint16_t rise_ns, uint16_t fall_ns)
{
	hrtim_set_dead_time_leg2(rise_ns, fall_ns);
}
