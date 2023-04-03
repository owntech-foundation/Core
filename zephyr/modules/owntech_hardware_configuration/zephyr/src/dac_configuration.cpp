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
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

// Zephyr
#include <zephyr.h>

// Owntech drivers
#include "dac.h"
#include "hrtim.h"

// Current file header
#include "HardwareConfiguration.h"

// Define the voltage reference used for ADC.
// It depends on the board used (On nucleo, choose Vref = 2.48V).

#define VREF 2.048f

static const struct device* dac1 = DEVICE_DT_GET(DAC1_DEVICE);
static const struct device* dac2 = DEVICE_DT_GET(DAC2_DEVICE);
static const struct device* dac3 = DEVICE_DT_GET(DAC3_DEVICE);

void HardwareConfiguration::initDacConstValue(uint8_t dac_number)
{
	const struct device* dac_dev;

	if (dac_number == 1)
	{
		dac_dev = dac1;
	}
	else if (dac_number == 3)
	{
		dac_dev = dac3;
	}
	else
	{
		dac_dev = dac2; // sets the dac 2 as default
	}

	if (device_is_ready(dac_dev) == true)
	{
		dac_set_const_value(dac_dev, 1, 0);
		dac_pin_configure(dac_dev, 1, dac_pin_external);
		dac_start(dac_dev, 1);
	}
}

void HardwareConfiguration::setDacConstValue(uint8_t dac_number, uint8_t channel, uint32_t const_value)
{
	const struct device* dac_dev;

	if (dac_number == 1)
	{
		dac_dev = dac1;
	}
	else if (dac_number == 3)
	{
		dac_dev = dac3;
	}
	else
	{
		dac_dev = dac2; // sets the dac 2 as default
	}

	if (device_is_ready(dac_dev) == true)
	{
		dac_set_const_value(dac_dev, channel, const_value);
	}
}

void HardwareConfiguration::dacConfigDac1CurrentmodeInit(hrtim_tu_t tu_src)
{
	// DAC 1
	dac_function_config_t function_config =
		{
			.dac_function = dac_function_sawtooth,
			.reset_trigger_source = hrtim_trig1,
			.step_trigger_source = hrtim_trig1,
			.polarity = dac_polarity_decrement,
			.reset_data = 4000,
			.step_data = 200};

	switch (tu_src)
	{
	case TIMB:
		function_config.reset_trigger_source = hrtim_trig2;
		function_config.step_trigger_source = hrtim_trig2;
		break;

	case TIMC:
		function_config.reset_trigger_source = hrtim_trig3;
		function_config.step_trigger_source = hrtim_trig3;
		break;

	case TIMD:
		function_config.reset_trigger_source = hrtim_trig4;
		function_config.step_trigger_source = hrtim_trig4;
		break;

	case TIME:
		function_config.reset_trigger_source = hrtim_trig5;
		function_config.step_trigger_source = hrtim_trig5;
		break;

	case TIMF:
		function_config.reset_trigger_source = hrtim_trig6;
		function_config.step_trigger_source = hrtim_trig6;
		break;

	default:
		break;
	}

	dac_set_function(dac1, 1, &function_config);
	dac_pin_configure(dac1, 1, dac_pin_internal_and_external);
	dac_start(dac1, 1);
}

void HardwareConfiguration::dacConfigDac3CurrentmodeInit(hrtim_tu_t tu_src)
{
	// DAC 3
	dac_function_config_t function_config =
		{
			.dac_function = dac_function_sawtooth,
			.reset_trigger_source = hrtim_trig1,
			.step_trigger_source = hrtim_trig1,
			.polarity = dac_polarity_decrement,
			.reset_data = 4000,
			.step_data = 200};

	switch (tu_src)
	{
	case TIMB:
		function_config.reset_trigger_source = hrtim_trig2;
		function_config.step_trigger_source = hrtim_trig2;
		break;

	case TIMC:
		function_config.reset_trigger_source = hrtim_trig3;
		function_config.step_trigger_source = hrtim_trig3;
		break;

	case TIMD:
		function_config.reset_trigger_source = hrtim_trig4;
		function_config.step_trigger_source = hrtim_trig4;
		break;

	case TIME:
		function_config.reset_trigger_source = hrtim_trig5;
		function_config.step_trigger_source = hrtim_trig5;
		break;

	case TIMF:
		function_config.reset_trigger_source = hrtim_trig6;
		function_config.step_trigger_source = hrtim_trig6;
		break;

	default:
		break;
	}

	dac_set_function(dac3, 1, &function_config);
	dac_pin_configure(dac3, 1, dac_pin_internal);
	dac_start(dac3, 1);
}

void HardwareConfiguration::slopeCompensationLeg1(float32_t set_voltage, float32_t reset_voltage)
{
	float32_t Dv = set_voltage - reset_voltage;

	if (Dv < 0)
		Dv = 0;

	if (Dv > set_voltage)
	{
		Dv = set_voltage;
		if (Dv > VREF)
			Dv = VREF;
	}

	uint32_t set_data = (uint32_t)(4096U * set_voltage) / (VREF);

	if (set_data > 4095U)
		set_data = 4095U;

	dac_function_update_reset(dac3, 1, set_data);

	uint32_t reset_data = (uint32_t)(Dv * 65536) / (VREF * 100); // divided by 100 because we have 100 voltage steps

	dac_function_update_step(dac3, 1, reset_data);
}

void HardwareConfiguration::slopeCompensationLeg2(float32_t set_voltage, float32_t reset_voltage)
{
	float32_t Dv = set_voltage - reset_voltage;

	if (Dv < 0)
		Dv = 0;

	if (Dv > set_voltage)
		Dv = set_voltage;

	if (Dv > VREF)
		Dv = VREF;

	uint32_t set_data = (uint32_t)(4096U * set_voltage) / (VREF);

	if (set_data > 4095U)
		set_data = 4095U;

	dac_function_update_reset(dac1, 1, set_data);

	uint32_t reset_data = (uint32_t)(Dv * 65536U) / (VREF * 100); // divided by 100 because we have 100 voltage steps

	dac_function_update_step(dac1, 1, reset_data);
}
