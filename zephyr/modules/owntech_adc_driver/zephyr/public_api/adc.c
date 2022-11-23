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
 */


// STM32 LL
#include <stm32_ll_adc.h>

// Current module private functions
#include "../src/adc_channels.h"
#include "../src/adc_core.h"

// Current file header
#include "adc.h"


/////
// Constants

#define NUMBER_OF_ADCS 4


/////
// Local variables

static uint32_t adc_trigger_sources[NUMBER_OF_ADCS] = {0};
static uint32_t adc_discontinuous_mode[NUMBER_OF_ADCS] = {0};


/////
// Public API

void adc_init()
{
	adc_core_init();
	adc_channels_init();
}

void adc_set_dual_mode(uint8_t dual_mode)
{
	adc_core_set_dual_mode(dual_mode);
}

void adc_configure_trigger_source(uint8_t adc_number, adc_ev_src_t trigger_source)
{
	/////
	// Convert to LL constants

	uint32_t trig;
	switch (trigger_source)
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

	// Only store configuration: it must be applied after ADC enable
	if ( (adc_number > 0) && (adc_number <= NUMBER_OF_ADCS) )
	{
		adc_trigger_sources[adc_number-1] = trig;
	}
}

void adc_configure_discontinuous_mode(uint8_t adc_number, uint32_t dicontinuous_count)
{
	// Only store configuration: it must be applied after ADC enable
	if ( (adc_number > 0) && (adc_number <= NUMBER_OF_ADCS) )
	{
		adc_discontinuous_mode[adc_number-1] = dicontinuous_count;
	}
}

int8_t adc_configure_adc_channels(uint8_t adc_number, const char* channel_list[], uint8_t channel_count)
{
	return adc_channnels_configure_adc_channels(adc_number, channel_list, channel_count);
}

void adc_start()
{
	uint8_t enabled_channels_count[NUMBER_OF_ADCS];

	for (uint8_t i = 0 ; i < NUMBER_OF_ADCS ; i++)
	{
		enabled_channels_count[i] = adc_channels_get_enabled_channels_count(i+1);
	}

	/////
	// Enable ADCs
	for (uint8_t i = 0 ; i < NUMBER_OF_ADCS ; i++)
	{
		if (enabled_channels_count[i] > 0)
		{
			adc_core_enable(i+1);
		}
	}

	/////
	// Configure ADCs channels
	for (uint8_t i = 0 ; i < NUMBER_OF_ADCS ; i++)
	{
		if (enabled_channels_count[i] > 0)
		{
			adc_channels_configure(i+1);
		}
	}

	/////
	// Configure ADCs
	for (uint8_t i = 0 ; i < NUMBER_OF_ADCS ; i++)
	{
		if (enabled_channels_count[i] > 0)
		{
			adc_core_configure_dma_mode(i+1);
		}
	}

	for (uint8_t i = 0 ; i < NUMBER_OF_ADCS ; i++)
	{
		if ( (enabled_channels_count[i] > 0) && (adc_discontinuous_mode[i] != 0) )
		{
			adc_core_configure_discontinuous_mode(i+1, adc_discontinuous_mode[i]);
		}
	}

	for (uint8_t i = 0 ; i < NUMBER_OF_ADCS ; i++)
	{
		if ( (enabled_channels_count[i] > 0) && (adc_trigger_sources[i] != 0) )
		{
			adc_core_configure_trigger_source(i+1, LL_ADC_REG_TRIG_EXT_RISING, adc_trigger_sources[i]);
		}
	}

	/////
	// Finally, start ADCs
	for (uint8_t i = 0 ; i < NUMBER_OF_ADCS ; i++)
	{
		if (enabled_channels_count[i] > 0)
		{
			adc_core_start(i+1);
		}
	}
}

const char* adc_get_channel_name(uint8_t adc_number, uint8_t channel_rank)
{
	return adc_channels_get_channel_name(adc_number, channel_rank);
}

uint8_t adc_get_enabled_channels_count(uint8_t adc_num)
{
	return adc_channels_get_enabled_channels_count(adc_num);
}

void adc_software_trigger_conversion(uint8_t adc_number)
{
	adc_core_start(adc_number);
}