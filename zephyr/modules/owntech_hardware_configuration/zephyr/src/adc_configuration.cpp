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
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */


// OwnTech Power API
#include "adc.h"

// Current file headers
#include "adc_configuration.h"
#include "adc_error_codes.h"


static uint8_t initialized;
static uint8_t channels_configured;


void _initialize()
{
	if (initialized == 0)
	{
		// Initialize the ADCs
		adc_init();
		initialized = 1;

		// Perform default configration
		configure_adc_trigger_source(1, hrtim_ev1);
		configure_adc_trigger_source(2, hrtim_ev3);
		configure_adc_trigger_source(3, software);
		configure_adc_trigger_source(4, software);

		adc_configure_discontinuous_mode(1, 1);
		adc_configure_discontinuous_mode(2, 1);
	}
}

void configure_adc12_dual_mode(uint8_t dual_mode)
{
	/////
	// Make sure module is initialized

	if (initialized == 0)
	{
		_initialize();
	}

	/////
	// Proceed

	adc_set_dual_mode(dual_mode);
}

int8_t configure_adc_channels(uint8_t adc_number, const char* channel_list[], uint8_t channel_count)
{
	/////
	// Make sure module is initialized

	if (initialized == 0)
	{
		_initialize();
	}

	/////
	// Proceed

	int8_t result = adc_configure_adc_channels(adc_number, channel_list, channel_count);

	if (result != NOERROR)
	{
		return result;
	}

	channels_configured = 1;

	return NOERROR;
}

void configure_adc_trigger_source(uint8_t adc_number, adc_ev_src_t trigger_source)
{
	/////
	// Make sure module is initialized

	if (initialized == 0)
	{
		_initialize();
	}

	/////
	// Proceed

	adc_configure_trigger_source(adc_number, trigger_source);
}

void configure_adc_discontinuous_mode(uint8_t adc_number, uint32_t dicontinuous_count)
{
	/////
	// Make sure module is initialized

	if (initialized == 0)
	{
		_initialize();
	}

	/////
	// Proceed

	adc_configure_discontinuous_mode(adc_number, dicontinuous_count);
}

void configure_adc_default_all_measurements()
{
	uint8_t number_of_channels_adc1 = 3;
	uint8_t number_of_channels_adc2 = 3;

	const char* adc1_channels[] =
	{
		"I1_LOW",
		"V1_LOW",
		"V_HIGH"
	};

	const char* adc2_channels[] =
	{
		"I2_LOW",
		"V2_LOW",
		"I_HIGH"
	};

	configure_adc_channels(1, adc1_channels, number_of_channels_adc1);
	configure_adc_channels(2, adc2_channels, number_of_channels_adc2);
}


void configure_adc_default_all_measurements_and_extra()
{
	uint8_t number_of_channels_adc1 = 3;
	uint8_t number_of_channels_adc2 = 4;

	const char* adc1_channels[] =
	{
		"I1_LOW",
		"V1_LOW",
		"V_HIGH"
	};

	const char* adc2_channels[] =
	{
		"I2_LOW",
		"V2_LOW",
		"I_HIGH",
		"EXTRA_MEAS"
	};

	configure_adc_channels(1, adc1_channels, number_of_channels_adc1);
	configure_adc_channels(2, adc2_channels, number_of_channels_adc2);
}
