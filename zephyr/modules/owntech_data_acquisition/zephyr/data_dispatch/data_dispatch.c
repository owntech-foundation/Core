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
#include <stdlib.h>
#include <stdint.h>

//Zephyr
#include <zephyr.h>

// OwnTech API
#include "adc.h"


/////
// Local variables

#define CHANNELS_BUFFERS_SIZE 32

static uint8_t number_of_adcs = 0;

// Number of channels in each ADC (cell i is ADC number i+1)
static uint8_t* enabled_channels_count = NULL;

// Array of per-adc/per-channel buffers.
// adc_channel_buffers[x][y][z][] is ADC x+1 channel y buffer z
// with z either 0 or 1 as there are two buffers per channel (double buffering)
static uint16_t**** adc_channel_buffers = NULL;

// Number of readings stored in each channel.
// buffers_data_count[x][y] is the current nuumber of
// values stored in the currently written buffer of ADC x+1 Channel y
static uint32_t** buffers_data_count = NULL;

// Currently written buffer for each channel.
// Either 0 or 1.
// If current_buffer[x][y] is 0, the currently written buffer
// for ADC x+1 Channel y is buffer 0 and the user buffer is buffer 1
static uint8_t** current_buffer = NULL;

// Small memory to retain latest value available to
// the peek() function after a buffer swap.
static uint16_t** peek_memory = NULL;


/////
// Private functions

__STATIC_INLINE uint16_t* _data_dispatch_get_buffer(uint8_t adc_index, uint8_t channel_index)
{
	uint8_t active_buffer = current_buffer[adc_index][channel_index];
	return adc_channel_buffers[adc_index][channel_index][active_buffer];
}

__STATIC_INLINE uint32_t _data_dispatch_get_count(uint8_t adc_index, uint8_t channel_index)
{
	return buffers_data_count[adc_index][channel_index];
}

__STATIC_INLINE void _data_dispatch_increment_count(uint8_t adc_index, uint8_t channel_index)
{
	uint32_t* current_count = &buffers_data_count[adc_index][channel_index];
	if ( (*current_count) < CHANNELS_BUFFERS_SIZE)
	{
		(*current_count)++;
	}
}

__STATIC_INLINE void _data_dispatch_swap_buffer(uint8_t adc_index, uint8_t channel_index)
{
	uint8_t* active_buffer = &current_buffer[adc_index][channel_index];

	*active_buffer = ((*active_buffer) == 0) ? 1 : 0;
	buffers_data_count[adc_index][channel_index] = 0;
}

/////
// Public API

void data_dispatch_init(uint8_t adc_count)
{
	// Store number on ADCs
	number_of_adcs = adc_count;

	// Prepare arrays for each ADC
	enabled_channels_count = k_malloc(number_of_adcs * sizeof(uint8_t));
	adc_channel_buffers    = k_calloc(number_of_adcs,  sizeof(uint16_t***));
	buffers_data_count     = k_calloc(number_of_adcs,  sizeof(uint32_t*));
	current_buffer         = k_calloc(number_of_adcs,  sizeof(uint8_t*));
	peek_memory            = k_calloc(number_of_adcs,  sizeof(uint16_t*));

	for (int adc_index = 0 ; adc_index < number_of_adcs ; adc_index++)
	{
		enabled_channels_count[adc_index] = adc_get_enabled_channels_count(adc_index+1);
		if (enabled_channels_count[adc_index] > 0)
		{
			// Prepare arrays for each channel
			adc_channel_buffers[adc_index] = k_malloc(enabled_channels_count[adc_index] * sizeof(uint16_t**));
			buffers_data_count[adc_index]  = k_calloc(enabled_channels_count[adc_index],  sizeof(uint32_t));
			current_buffer[adc_index]      = k_calloc(enabled_channels_count[adc_index],  sizeof(uint8_t));
			peek_memory[adc_index]         = k_calloc(enabled_channels_count[adc_index],  sizeof(uint16_t));
			for (int channel_index = 0 ; channel_index < enabled_channels_count[adc_index] ; channel_index++)
			{
				// Prepare double buffer
				adc_channel_buffers[adc_index][channel_index]    = k_malloc(sizeof(uint16_t*) * 2);
				adc_channel_buffers[adc_index][channel_index][0] = k_malloc(sizeof(uint16_t) * CHANNELS_BUFFERS_SIZE);
				adc_channel_buffers[adc_index][channel_index][1] = k_malloc(sizeof(uint16_t) * CHANNELS_BUFFERS_SIZE);
			}
		}
	}
}

void data_dispatch_do_dispatch(uint8_t adc_num, uint16_t* dma_buffer)
{
	uint8_t adc_index = adc_num-1;
	for (int channel_index = 0 ; channel_index < enabled_channels_count[adc_index] ; channel_index++)
	{
		// Get info on buffer
		uint16_t* active_buffer = _data_dispatch_get_buffer(adc_index, channel_index);
		uint32_t  current_count = _data_dispatch_get_count(adc_index, channel_index);

		// Copy data
		active_buffer[current_count] = dma_buffer[channel_index];

		// Increment count
		_data_dispatch_increment_count(adc_index, channel_index);
	}
}


/////
// Accessors

uint16_t* data_dispatch_get_acquired_values(uint8_t adc_number, uint8_t channel_rank, uint32_t* number_of_values_acquired)
{
	uint8_t adc_index = adc_number-1;
	if (adc_index < number_of_adcs)
	{
		// Get info on buffer
		uint16_t* active_buffer = _data_dispatch_get_buffer(adc_index, channel_rank);
		uint32_t  current_count = _data_dispatch_get_count(adc_index, channel_rank);

		// Swap buffers
		_data_dispatch_swap_buffer(adc_index, channel_rank);

		// Retain latest value for peek() functions
		if (current_count > 0)
		{
			peek_memory[adc_number][channel_rank] = active_buffer[current_count-1];
		}

		// Return data
		*number_of_values_acquired = current_count;
		return active_buffer;
	}
	else
	{
		*number_of_values_acquired = 0;
		return NULL;
	}
}

uint16_t data_dispatch_peek_acquired_value(uint8_t adc_number, uint8_t channel_rank)
{
	uint8_t adc_index = adc_number-1;
	if (adc_index < number_of_adcs)
	{
		// Get info on buffer
		uint16_t* active_buffer = _data_dispatch_get_buffer(adc_index, channel_rank);
		uint32_t  current_count = _data_dispatch_get_count(adc_index, channel_rank);

		// Return data
		if (current_count > 0)
		{
			return active_buffer[current_count-1];
		}
		else
		{
			return peek_memory[adc_number][channel_rank];
		}
	}
	else
	{
		return 0;
	}
}
