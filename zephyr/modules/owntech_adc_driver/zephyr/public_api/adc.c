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
 */


// STM32 LL
#include <stm32_ll_adc.h>

// Current module private functions
#include "../src/adc_core.h"

// Current file header
#include "adc.h"


/////
// Constants

#define NUMBER_OF_ADCS 4
#define NUMBER_OF_CHANNELS_PER_ADC 16


/////
// Local variables

static adc_ev_src_t adc_trigger_sources[NUMBER_OF_ADCS]    = {software};
static uint32_t     adc_discontinuous_mode[NUMBER_OF_ADCS] = {0};
static uint32_t     enabled_channels_count[NUMBER_OF_ADCS] = {0};
static bool         enable_dma[NUMBER_OF_ADCS]             = {false};

static uint32_t     enabled_channels[NUMBER_OF_ADCS][NUMBER_OF_CHANNELS_PER_ADC] = {0};


/////
// Public API

void adc_configure_trigger_source(uint8_t adc_number, adc_ev_src_t trigger_source)
{
	if ( (adc_number == 0) || (adc_number > NUMBER_OF_ADCS) )
		return;

	adc_trigger_sources[adc_number-1] = trigger_source;

}

void adc_configure_discontinuous_mode(uint8_t adc_number, uint32_t discontinuous_count)
{
	if ( (adc_number == 0) || (adc_number > NUMBER_OF_ADCS) )
		return;

	adc_discontinuous_mode[adc_number-1] = discontinuous_count;
}

void adc_add_channel(uint8_t adc_number, uint8_t channel)
{
	if ( (adc_number == 0) || (adc_number > NUMBER_OF_ADCS) )
		return;

	uint8_t adc_index = adc_number-1;

	if (enabled_channels_count[adc_index] == NUMBER_OF_CHANNELS_PER_ADC)
		return;

	enabled_channels[adc_index][enabled_channels_count[adc_index]] = channel;
	enabled_channels_count[adc_index]++;
}

void adc_remove_channel(uint8_t adc_number, uint8_t channel)
{
	if ( (adc_number == 0) || (adc_number > NUMBER_OF_ADCS) )
		return;

	uint8_t adc_index = adc_number-1;

	for (int i = 0 ; i < NUMBER_OF_CHANNELS_PER_ADC ; i++)
	{
		if (enabled_channels[adc_index][i] == channel)
		{
			for (int j = i ; j < NUMBER_OF_CHANNELS_PER_ADC-1 ; j++)
			{
				enabled_channels[adc_index][i] = enabled_channels[adc_index][i+1];
			}
			enabled_channels[adc_index][NUMBER_OF_CHANNELS_PER_ADC-1] = 0;

			enabled_channels_count[adc_index]--;

			break;
		}
	}
}

void adc_configure_use_dma(uint8_t adc_number, bool use_dma)
{
	if ( (adc_number == 0) || (adc_number > NUMBER_OF_ADCS) )
		return;

	enable_dma[adc_number-1] = use_dma;
}

void adc_start()
{
	/////
	// Initialize ADCs

	adc_core_init();

	/////
	// Pre-enable configuration

	// Nothing here for now.

	// If some channels have to be set as differential,
	// or ADCs have to be set as dual mode,
	// this shoud be done here.

	/////
	// Enable ADCs

	for (int i = 1 ; i <= NUMBER_OF_ADCS ; i++)
	{
		adc_core_enable(i);
	}

	/////
	// Post-enable configuration

	for (uint8_t adc_num = 1 ; adc_num <= NUMBER_OF_ADCS ; adc_num++)
	{
		uint8_t adc_index = adc_num-1;
		if (enabled_channels_count[adc_index] > 0)
		{
			for (int channel_index = 0 ; channel_index < NUMBER_OF_CHANNELS_PER_ADC ; channel_index++)
			{
				if (enabled_channels[adc_index][channel_index] == 0)
					break;

				adc_core_configure_channel(adc_num, enabled_channels[adc_index][channel_index], channel_index+1);
			}
		}
	}

	for (uint8_t adc_num = 1 ; adc_num <= NUMBER_OF_ADCS ; adc_num++)
	{
		uint8_t adc_index = adc_num-1;
		if (enabled_channels_count[adc_index] > 0)
		{
			adc_core_configure_dma_mode(adc_num, enable_dma[adc_index]);
		}
	}

	for (uint8_t adc_num = 1 ; adc_num <= NUMBER_OF_ADCS ; adc_num++)
	{
		uint8_t adc_index = adc_num-1;
		if (enabled_channels_count[adc_index] > 0)
		{
			adc_core_configure_discontinuous_mode(adc_num, adc_discontinuous_mode[adc_index]);
		}
	}

	for (uint8_t adc_num = 1 ; adc_num <= NUMBER_OF_ADCS ; adc_num++)
	{
		uint8_t adc_index = adc_num-1;
		if (enabled_channels_count[adc_index] > 0)
		{
			// Convert to LL constants
			uint32_t trig;
			switch (adc_trigger_sources[adc_index])
			{
			case hrtim_ev1:
				trig = LL_ADC_REG_TRIG_EXT_HRTIM_TRG1;
				break;
			case hrtim_ev2:
				trig = LL_ADC_REG_TRIG_EXT_HRTIM_TRG2;
				break;
			case hrtim_ev3:
				trig = LL_ADC_REG_TRIG_EXT_HRTIM_TRG3;
				break;
			case hrtim_ev4:
				trig = LL_ADC_REG_TRIG_EXT_HRTIM_TRG4;
				break;
			case software:
			default:
				trig = LL_ADC_REG_TRIG_SOFTWARE;
				break;
			}

			adc_core_configure_trigger_source(adc_num, LL_ADC_REG_TRIG_EXT_RISING, trig);
		}
	}

	/////
	// Start ADCs

	for (uint8_t adc_num = 1 ; adc_num <= NUMBER_OF_ADCS ; adc_num++)
	{
		uint8_t adc_index = adc_num-1;
		if (enabled_channels_count[adc_index] > 0)
		{
			adc_core_start(adc_num, enabled_channels_count[adc_index]);
		}
	}
}

void adc_stop()
{
	for (uint8_t adc_num = 1 ; adc_num <= NUMBER_OF_ADCS ; adc_num++)
	{
		uint8_t adc_index = adc_num-1;
		if (enabled_channels_count[adc_index] > 0)
		{
			adc_core_stop(adc_num);
		}
	}
}

void adc_trigger_software_conversion(uint8_t adc_number)
{
	adc_core_start(adc_number, 1);
}
