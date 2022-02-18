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


// Stdlib
#include <stdint.h>

// STM32 LL
#include <stm32g4xx_ll_adc.h>

// OwnTech API
#include "adc_channels.h"
#include "adc_core.h"


/////
// Local variables

static uint32_t adc_trigger_sources[3] = {0};


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

void adc_configure_trigger_source(uint8_t adc_number, uint32_t trigger_source)
{
	// Only store configuration: it must be applied after ADC enable
	if (adc_number < 3)
		adc_trigger_sources[adc_number-1] = trigger_source;
}

int8_t adc_configure_adc_channels(uint8_t adc_number, const char* channel_list[], uint8_t channel_count)
{
	return adc_channnels_configure_adc_channels(adc_number, channel_list, channel_count);
}

void adc_start()
{
	uint8_t enabled_channels_count[3];

	for (uint8_t i = 0 ; i < 3 ; i++)
	{
		enabled_channels_count[i] = adc_channels_get_enabled_channels_count(i+1);
	}

	/////
	// Enable ADCs
	for (uint8_t i = 0 ; i < 3 ; i++)
	{
		if (enabled_channels_count[i] > 0)
			adc_core_enable(i+1);
	}

	/////
	// Configure ADCs channels
	for (uint8_t i = 0 ; i < 3 ; i++)
	{
		if (enabled_channels_count[i] > 0)
			adc_channels_configure(i+1);
	}

	/////
	// Configure ADCs
	for (uint8_t i = 0 ; i < 3 ; i++)
	{
		if (enabled_channels_count[i] > 0)
			adc_core_configure_dma_mode(i+1);
	}

	for (uint8_t i = 0 ; i < 3 ; i++)
	{
		if ( (enabled_channels_count[i] > 0) && (adc_trigger_sources[i] != 0) )
			adc_core_configure_trigger_source(i+1, LL_ADC_REG_TRIG_EXT_RISING, adc_trigger_sources[i]);
	}

	/////
	// Finally, start ADCs
	for (uint8_t i = 0 ; i < 3 ; i++)
	{
		if (enabled_channels_count[i] > 0)
			adc_core_start(i+1);
	}
}

const char* adc_get_channel_name(uint8_t adc_number, uint8_t channel_rank)
{
	return adc_channels_get_channel_name(adc_number, channel_rank);
}
