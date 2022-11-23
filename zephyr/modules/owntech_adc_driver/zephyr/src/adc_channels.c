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
 * @author Hugues Larrive <hugues.larrive@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 *
 * @brief  ADC driver for stm32g474re
 */


// Stdlib
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Zephyr
#include <zephyr.h>

// Current file header
#include "adc_channels.h"

// OwnTech API
#include "adc_helper.h"
#include "adc_error_codes.h"


/////
// Device-tree related defines

typedef struct
{
	char*   name;
	bool    is_differential;
	uint8_t number;
	char*   adc;
} channel_prop_t;

#define ADC_INPUTS_NODELABEL DT_NODELABEL(mychannels)

// Channel properties
#define CHANNEL_NAME(node_id)    DT_PROP(node_id, label)
#define CHANNEL_IS_DIFF(node_id) DT_PROP(node_id, differential)
#define CHANNEL_NUMBER(node_id)  DT_PHA_BY_IDX(node_id, io_channels, 0, input)
#define CHANNEL_ADC(node_id)     DT_PROP_BY_PHANDLE_IDX(node_id, io_channels, 0, label)

#define CHANNEL_WRITE_PROP(node_id)                \
	{                                              \
		.name=CHANNEL_NAME(node_id),               \
		.is_differential=CHANNEL_IS_DIFF(node_id), \
		.number=CHANNEL_NUMBER(node_id),           \
		.adc=CHANNEL_ADC(node_id)                  \
	},

// Channel count. This is very dirty!
#define CHANNEL_COUNTER(node_id) +1
#define CHANNEL_COUNT (DT_FOREACH_CHILD(ADC_INPUTS_NODELABEL, CHANNEL_COUNTER))


/////
// Variables

static channel_prop_t available_channels_props[] =
{
	DT_FOREACH_CHILD(ADC_INPUTS_NODELABEL, CHANNEL_WRITE_PROP)
};

// Number of available channels defined in device tree
static uint8_t adc1_available_channels_count;
static uint8_t adc2_available_channels_count;
static uint8_t adc3_available_channels_count;
static uint8_t adc4_available_channels_count;

// List of available channels as defined in device tree.
// These are arrays (range 0 to adcX_available_channels_count-1)
// of pointers to channel definition in available_channels_props array.
static channel_prop_t** adc1_available_channels_list;
static channel_prop_t** adc2_available_channels_list;
static channel_prop_t** adc3_available_channels_list;
static channel_prop_t** adc4_available_channels_list;

// Number of enabled channels defined by the user configuration
static uint8_t adc1_enabled_channels_count;
static uint8_t adc2_enabled_channels_count;
static uint8_t adc3_enabled_channels_count;
static uint8_t adc4_enabled_channels_count;

// List of channels enabled by user configuration.
// These are arrays (range 0 to adcX_enabled_channels_count-1)
// of pointers to channel definition in available_channels_props array.
static channel_prop_t** adc1_enabled_channels_list;
static channel_prop_t** adc2_enabled_channels_list;
static channel_prop_t** adc3_enabled_channels_list;
static channel_prop_t** adc4_enabled_channels_list;


/////
// ADC channels private functions

/**
 * Builds list of device-tree defined channels for each ADC.
 */
static void _adc_channels_build_available_channels_lists()
{
	// Count total number of channels
	adc1_available_channels_count = 0;
	adc2_available_channels_count = 0;
	adc3_available_channels_count = 0;
	adc4_available_channels_count = 0;

	for (int i = 0 ; i < CHANNEL_COUNT ; i++)
	{
		uint8_t adc_number = _get_adc_number_by_name(available_channels_props[i].adc);
		if (adc_number == 1)
		{
			adc1_available_channels_count++;
		}
		else if (adc_number == 2)
		{
			adc2_available_channels_count++;
		}
		else if (adc_number == 3)
		{
			adc3_available_channels_count++;
		}
		else if (adc_number == 4)
		{
			adc4_available_channels_count++;
		}
	}

	// Build a list of channels by ADC
	adc1_available_channels_list = k_malloc(sizeof(channel_prop_t*) * adc1_available_channels_count);
	adc2_available_channels_list = k_malloc(sizeof(channel_prop_t*) * adc2_available_channels_count);
	adc3_available_channels_list = k_malloc(sizeof(channel_prop_t*) * adc3_available_channels_count);
	adc4_available_channels_list = k_malloc(sizeof(channel_prop_t*) * adc4_available_channels_count);

	int adc1_index = 0;
	int adc2_index = 0;
	int adc3_index = 0;
	int adc4_index = 0;
	for (int i = 0 ; i < CHANNEL_COUNT ; i++)
	{
		uint8_t adc_number = _get_adc_number_by_name(available_channels_props[i].adc);
		if (adc_number == 1)
		{
			adc1_available_channels_list[adc1_index] = &available_channels_props[i];
			adc1_index++;
		}
		else if (adc_number == 2)
		{
			adc2_available_channels_list[adc2_index] = &available_channels_props[i];
			adc2_index++;
		}
		else if (adc_number == 3)
		{
			adc3_available_channels_list[adc3_index] = &available_channels_props[i];
			adc3_index++;
		}
		else if (adc_number == 4)
		{
			adc4_available_channels_list[adc4_index] = &available_channels_props[i];
			adc4_index++;
		}
	}
}

/**
 * ADC differential channel set-up:
 * Applies differential mode to specified channel.
 * Refer to RM 21.4.7
 */
static void _adc_channels_set_channel_differential(char* adc_name, uint8_t channel)
{
	ADC_TypeDef* adc = _get_adc_by_name(adc_name);

	if (adc != NULL)
	{
		LL_ADC_SetChannelSingleDiff(adc,
		                            __LL_ADC_DECIMAL_NB_TO_CHANNEL(channel),
		                            LL_ADC_DIFFERENTIAL_ENDED
		                           );
	}

}

/**
 * Differential channel setup.
 * Must be done before ADC is enabled.
 */
static void _adc_channels_differential_setup()
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		if (available_channels_props[i].is_differential)
		{
			_adc_channels_set_channel_differential(available_channels_props[i].adc, available_channels_props[i].number);
		}
	}
}

static channel_prop_t** _adc_channels_get_enabled_channels_list(uint8_t adc_num)
{
	channel_prop_t** enabled_channels_list = NULL;
	switch (adc_num)
	{
		case 1:
			enabled_channels_list = adc1_enabled_channels_list;
			break;
		case 2:
			enabled_channels_list = adc2_enabled_channels_list;
			break;
		case 3:
			enabled_channels_list = adc3_enabled_channels_list;
			break;
		case 4:
			enabled_channels_list = adc4_enabled_channels_list;
			break;
	}
	return enabled_channels_list;
}

static channel_prop_t** _adc_channels_get_available_channels_list(uint8_t adc_num)
{
	channel_prop_t** available_channels_list = NULL;
	switch (adc_num)
	{
		case 1:
			available_channels_list = adc1_available_channels_list;
			break;
		case 2:
			available_channels_list = adc2_available_channels_list;
			break;
		case 3:
			available_channels_list = adc3_available_channels_list;
			break;
		case 4:
			available_channels_list = adc4_available_channels_list;
			break;
	}
	return available_channels_list;
}

static uint8_t _adc_channels_get_available_channels_count(uint8_t adc_num)
{
	uint8_t available_channels_count = 0;
	switch (adc_num)
	{
		case 1:
			available_channels_count = adc1_available_channels_count;
			break;
		case 2:
			available_channels_count = adc2_available_channels_count;
			break;
		case 3:
			available_channels_count = adc3_available_channels_count;
			break;
		case 4:
			available_channels_count = adc4_available_channels_count;
			break;
	}
	return available_channels_count;
}

static uint8_t _adc_channels_get_enabled_channels_count(uint8_t adc_num)
{
	uint8_t enabled_channels_count = 0;
	switch (adc_num)
	{
		case 1:
			enabled_channels_count = adc1_enabled_channels_count;
			break;
		case 2:
			enabled_channels_count = adc2_enabled_channels_count;
			break;
		case 3:
			enabled_channels_count = adc3_enabled_channels_count;
			break;
		case 4:
			enabled_channels_count = adc4_enabled_channels_count;
			break;
	}
	return enabled_channels_count;
}

static void _adc_channels_set_enabled_channels(uint8_t adc_num, channel_prop_t** enabled_channels, uint8_t enabled_channels_count)
{
	switch (adc_num)
	{
		case 1:
			adc1_enabled_channels_list = enabled_channels;
			adc1_enabled_channels_count = enabled_channels_count;
			break;
		case 2:
			adc2_enabled_channels_list = enabled_channels;
			adc2_enabled_channels_count = enabled_channels_count;
			break;
		case 3:
			adc3_enabled_channels_list = enabled_channels;
			adc3_enabled_channels_count = enabled_channels_count;
			break;
		case 4:
			adc4_enabled_channels_list = enabled_channels;
			adc4_enabled_channels_count = enabled_channels_count;
			break;
	}
}

static channel_prop_t* _adc_channels_get_available_channel_by_name(uint8_t adc_num, const char* channel_name)
{
	channel_prop_t** current_adc_available_channels = _adc_channels_get_available_channels_list(adc_num);

	for (int i = 0 ; i < _adc_channels_get_available_channels_count(adc_num) ; i++)
	{
		if (strcmp(current_adc_available_channels[i]->name, channel_name) == 0)
		{
			return current_adc_available_channels[i];
		}
	}

	return NULL;
}


/////
// ADC channels public functions

void adc_channels_init()
{
	_adc_channels_build_available_channels_lists();
	_adc_channels_differential_setup();
}

void adc_channels_configure(uint8_t adc_num)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);
	uint8_t enabled_channels_in_this_adc = adc_channels_get_enabled_channels_count(adc_num);

	channel_prop_t** enabled_channels_list = _adc_channels_get_enabled_channels_list(adc_num);

	for (int rank = 0; rank < enabled_channels_in_this_adc; rank++)
	{
		uint8_t current_channel = enabled_channels_list[rank]->number;

		// Set regular sequence
		LL_ADC_REG_SetSequencerRanks(adc,
		                             adc_decimal_nb_to_rank(rank+1), // Indexed from 1 => +1
		                             __LL_ADC_DECIMAL_NB_TO_CHANNEL(current_channel)
		                            );
		// Set channels sampling time

		/* 000: 2.5 ADC clock cycles
		 * 001: 6.5 ADC clock cycles
		 * 010: 12.5 ADC clock cycles
		 * 011: 24.5 ADC clock cycles
		 * 100: 47.5 ADC clock cycles
		 * 101: 92.5 ADC clock cycles
		 * 110: 247.5 ADC clock cycles
		 * 111: 640.5 ADC clock cycles
		 */
		/* Vrefint minimum sampling time : 4us
		 */
		/* Vts minimum sampling time : 5us
		 */
		/* For 0b110:
		 * Tadc_clk = 1 / 42.5 MHz = 23.5 ns
		 * Tsar = 12.5 * Tadc_clk = 293.75 ns
		 * Tsmpl = 247.5 * Tadc_clk = 5816.25 ns
		 * Tconv = Tsmpl + Tsar = 6.11 us
		 * -> Fconv up to 163.6 KSPS for 1 channel per ADC
		 * Fconv up to 27.2 KSPS with the 6 channels actally
		 * used on the ADC1
		 *
		 * For 0b001 (ok for voltage):
		 * Tadc_clk = 1 / 42.5 MHz = 23.5 ns
		 * Tsar = 12.5 * Tadc_clk = 293.75 ns
		 * Tsmpl = 6.5 * Tadc_clk = 152.75 ns
		 * Tconv = Tsmpl + Tsar = 446.4 ns
		 * -> Fconv up to 2239 KSPS for 1 channel per ADC
		 * Fconv up to 373 KSPS with the 6 channels actally
		 * used on the ADC1
		 *
		 * For 0b101 (ok for current):
		 * Tadc_clk = 1 / 42.5 MHz = 23.5 ns
		 * Tsar = 12.5 * Tadc_clk = 293.75 ns
		 * Tsmpl = 92.5 * Tadc_clk = 2173.75 ns
		 * Tconv = Tsmpl + Tsar = 2.47 µs
		 * -> Fconv up to 404 KSPS for 1 channel per ADC
		 * Fconv up to 134 KSPS for 3 channels actally
		 * used on each ADC
		 */
		LL_ADC_SetChannelSamplingTime(adc,
		                              __LL_ADC_DECIMAL_NB_TO_CHANNEL(current_channel),
		                              LL_ADC_SAMPLINGTIME_12CYCLES_5
		                             );

	}

	// Set regular sequence length
	LL_ADC_REG_SetSequencerLength(adc, (uint32_t)enabled_channels_in_this_adc-1);
}

int8_t adc_channnels_configure_adc_channels(uint8_t adc_num, const char* channel_list[], uint8_t channel_count)
{
	uint8_t result = 0;

	channel_prop_t** current_adc_enabled_channels_list = k_malloc(channel_count * sizeof(channel_prop_t*));

	for (int i = 0 ; i < channel_count ; i++)
	{
		channel_prop_t* current_channel = _adc_channels_get_available_channel_by_name(adc_num, channel_list[i]);
		if (current_channel == NULL)
		{
			result = ECHANNOTFOUND;
			break;
		}
		else
		{
			current_adc_enabled_channels_list[i] = current_channel;
		}
	}

	if (result == 0)
	{
		channel_prop_t** previous_adc_enabled_channels_list = _adc_channels_get_enabled_channels_list(adc_num);
		if (previous_adc_enabled_channels_list != NULL)
		{
			k_free(previous_adc_enabled_channels_list);
		}

		_adc_channels_set_enabled_channels(adc_num, current_adc_enabled_channels_list, channel_count);
	}
	else
	{
		k_free(current_adc_enabled_channels_list);
	}

	return result;
}

const char* adc_channels_get_channel_name(uint8_t adc_num, uint8_t channel_rank)
{
	channel_prop_t** current_adc_enabled_channels_list = _adc_channels_get_enabled_channels_list(adc_num);

	if ( (current_adc_enabled_channels_list != NULL) && (channel_rank < _adc_channels_get_enabled_channels_count(adc_num)) )
	{
		return current_adc_enabled_channels_list[channel_rank]->name;
	}

	return NULL;
}

uint8_t adc_channels_get_enabled_channels_count(uint8_t adc_num)
{
	uint8_t enabled_channels = 0;

	switch (adc_num)
	{
		case 1:
			enabled_channels = adc1_enabled_channels_count;
			break;
		case 2:
			enabled_channels = adc2_enabled_channels_count;
			break;
		case 3:
			enabled_channels = adc3_enabled_channels_count;
			break;
		case 4:
			enabled_channels = adc4_enabled_channels_count;
			break;
	}

	return enabled_channels;
}
