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
 * @author Hugues Larrive <hugues.larrive@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 *
 * @brief  ADC driver for stm32g474re
 */


// Stdlib
#include <string.h>

// Zephyr
#include <zephyr.h>

// Current file header
#include "adc_channels.h"

// OwnTech API
#include "HardwareConfiguration.h"


/////
// Device-tree related defines

typedef struct
{
	const char* name;
	bool        is_differential;
	uint8_t     number;
	uint32_t    adc_reg_addr; // ADC addr is used to identify ADC
} channel_prop_t;

#define ADC_INPUTS_NODELABEL DT_NODELABEL(mychannels)

// Channel properties
#define CHANNEL_NAME(node_id)    DT_PROP(node_id, label)
#define CHANNEL_IS_DIFF(node_id) DT_PROP(node_id, differential)
#define CHANNEL_NUMBER(node_id)  DT_PHA_BY_IDX(node_id, io_channels, 0, input)
#define ADC_REG_ADDR(node_id)    DT_REG_ADDR(DT_PHANDLE(node_id, io_channels))

#define CHANNEL_WRITE_PROP(node_id)                \
	{                                              \
		.name=CHANNEL_NAME(node_id),               \
		.is_differential=CHANNEL_IS_DIFF(node_id), \
		.number=CHANNEL_NUMBER(node_id),           \
		.adc_reg_addr=ADC_REG_ADDR(node_id)        \
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

static bool initialized = false;


/////
// ADC channels private functions

uint8_t _get_adc_number_by_address(uint32_t adc_address)
{
	uint8_t adc_number = 0;

	if (adc_address == 0x50000000)
		adc_number = 1;
	else if (adc_address == 0x50000100)
		adc_number = 2;
	else if (adc_address == 0x50005000)
		adc_number = 3;
	else if (adc_address == 0x50005100)
		adc_number = 4;

	return adc_number;
}

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
		uint8_t adc_number = _get_adc_number_by_address(available_channels_props[i].adc_reg_addr);
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
	adc1_available_channels_list = (channel_prop_t**)k_malloc(sizeof(channel_prop_t*) * adc1_available_channels_count);
	adc2_available_channels_list = (channel_prop_t**)k_malloc(sizeof(channel_prop_t*) * adc2_available_channels_count);
	adc3_available_channels_list = (channel_prop_t**)k_malloc(sizeof(channel_prop_t*) * adc3_available_channels_count);
	adc4_available_channels_list = (channel_prop_t**)k_malloc(sizeof(channel_prop_t*) * adc4_available_channels_count);

	int adc1_index = 0;
	int adc2_index = 0;
	int adc3_index = 0;
	int adc4_index = 0;
	for (int i = 0 ; i < CHANNEL_COUNT ; i++)
	{
		uint8_t adc_number = _get_adc_number_by_address(available_channels_props[i].adc_reg_addr);
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

	initialized = true;
}

static channel_prop_t** _adc_channels_get_enabled_channels_list(uint8_t adc_num)
{
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

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
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

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
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

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
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

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
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

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
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

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

int8_t adc_channels_configure_adc_channels(uint8_t adc_num, const char* channel_list[], uint8_t channel_count)
{
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

	uint8_t result = 0;

	channel_prop_t** current_adc_enabled_channels_list = (channel_prop_t**)k_malloc(channel_count * sizeof(channel_prop_t*));

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

			hwConfig.adcConfigureDma(adc_num, true);
			hwConfig.adcAddChannel(adc_num, current_channel->number);
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
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

	channel_prop_t** current_adc_enabled_channels_list = _adc_channels_get_enabled_channels_list(adc_num);

	if ( (current_adc_enabled_channels_list != NULL) && (channel_rank < _adc_channels_get_enabled_channels_count(adc_num)) )
	{
		return current_adc_enabled_channels_list[channel_rank]->name;
	}

	return NULL;
}

uint8_t adc_channels_get_enabled_channels_count(uint8_t adc_num)
{
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

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

void configure_adc_default_all_measurements()
{
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

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

	adc_channels_configure_adc_channels(1, adc1_channels, number_of_channels_adc1);
	adc_channels_configure_adc_channels(2, adc2_channels, number_of_channels_adc2);
}
