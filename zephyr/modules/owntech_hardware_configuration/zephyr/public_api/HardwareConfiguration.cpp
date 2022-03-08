/*
 * Copyright (c) 2022 LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


// OwnTech headers
#include "../src/dac_configuration.h"
#include "../src/ngnd_configuration.h"
#include "../src/led_configuration.h"
#include "../src/timer_configuration.h"
#include "../src/hrtim_configuration.h"
#include "../src/uart_configuration.h"
#include "../src/adc_configuration.h"

// Current class header
#include "HardwareConfiguration.h"


/////
// Public object to interact with the class

HardwareConfiguration hwConfig;


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
	if (hardware_version == v_1_1_2)
	{
		uart_lpuart1_swap_rx_tx();
	}
}


/////
// DAC

void HardwareConfiguration::initDac1Dac3CurrentMode()
{
	dac_config_dac1_dac3_current_mode_init();
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
	hrtim_init_interleaved_buck_mode();
}

void HardwareConfiguration::initInterleavedBoostMode()
{
	hrtim_init_interleaved_boost_mode();
}

void HardwareConfiguration::initFullBridgeBuckMode()
{
	hrtim_init_interleaved_buck_mode();
}

void HardwareConfiguration::initFullBridgeBoostMode()
{
	hrtim_init_interleaved_boost_mode();
}

void HardwareConfiguration::initIndependentMode(bool leg1_buck_mode, bool leg2_buck_mode)
{
	hrtim_init_independent_mode(leg1_buck_mode, leg2_buck_mode);
}

void HardwareConfiguration::setInterleavedDutyCycle(float32_t duty_cycle)
{
	hrtim_interleaved_pwm_update(duty_cycle);
}

void HardwareConfiguration::setFullBridgeDutyCycle(float32_t duty_cycle)
{
	hrtim_hbridge_pwm_update(duty_cycle);
}

void HardwareConfiguration::setLeg1DutyCycle(float32_t duty_cycle)
{
	hrtim_leg1_pwm_update(duty_cycle);
}

void HardwareConfiguration::setLeg2DutyCycle(float32_t duty_cycle)
{
	hrtim_leg2_pwm_update(duty_cycle);
}

void HardwareConfiguration::setInterleavedOn()
{
	hrtim_start_interleaved();
}

void HardwareConfiguration::setFullBridgeOn()
{
	hrtim_start_interleaved();
}

void HardwareConfiguration::setLeg1On()
{
	hrtim_start_leg1();
}

void HardwareConfiguration::setLeg2On()
{
	hrtim_start_leg2();
}

void HardwareConfiguration::setInterleavedOff()
{
	hrtim_stop_interleaved();
}

void HardwareConfiguration::setFullBridgeOff()
{
	hrtim_stop_interleaved();
}

void HardwareConfiguration::setLeg1Off()
{
	hrtim_stop_leg1();
}

void HardwareConfiguration::setLeg2Off()
{
	hrtim_stop_leg2();
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

void HardwareConfiguration::configureAdcTriggerSource(uint8_t adc_number, adc_src_t trigger_source)
{
	configure_adc_trigger_source(adc_number, trigger_source);
}

void HardwareConfiguration::configureAdcDefaultAllMeasurements()
{
	configure_adc_default_all_measurements();
}
