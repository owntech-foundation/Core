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


// Stdlib
#include <stdlib.h>
#include <stdint.h>

//Zephyr
#include <zephyr.h>

// OwnTech API
#include "adc.h"
#include "Scheduling.h"
#include "scheduling_internal.h"
#include "hrtim.h"
#include "leg.h"

// Current module header
#include "DataAcquisition.h"
#include "dma.h"

// Current file header
#include "data_dispatch.h"


/////
// Local variables

#define CHANNELS_BUFFERS_SIZE 32
#define ADC_COUNT 4

// Number of channels in each ADC (cell i is ADC number i+1)
static uint8_t* enabled_channels_count = nullptr;

// Array of per-adc/per-channel buffers.
// adc_channel_buffers[x][y][z][] is ADC x+1 channel y buffer z
// with z either 0 or 1 as there are two buffers per channel (double buffering)
static uint16_t**** adc_channel_buffers = nullptr;

// Number of readings stored in each channel.
// buffers_data_count[x][y] is the current nuumber of
// values stored in the currently written buffer of ADC x+1 Channel y
static uint32_t** buffers_data_count = nullptr;

// Currently written buffer for each channel.
// Either 0 or 1.
// If current_buffer[x][y] is 0, the currently written buffer
// for ADC x+1 Channel y is buffer 0 and the user buffer is buffer 1
static uint8_t** current_buffer = nullptr;

// Small memory to retain latest value available to
// the peek() function after a buffer swap.
static uint16_t** peek_memory = nullptr;

// DMA buffers: data from the ADC 1/2 are stored in these
// buffers until dispatch is done (ADC 3/4 won't use DMA).
// Main buffers are always used, while secondary buffers
// will only be used when double-buffering is activated.
// Double buffering is activated in Interrupt mode,
// while Task mode doesn't need it.
static uint16_t* dma_main_buffers[ADC_COUNT]      = {nullptr};
static uint16_t* dma_secondary_buffers[ADC_COUNT] = {nullptr};
static uint8_t   current_dma_buffer[ADC_COUNT]    = {0};
static size_t    dma_buffer_sizes[ADC_COUNT]      = {0};

// Dispatch method
static dispatch_t dispatch_type;


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

__STATIC_INLINE void _data_dispatch_swap_buffers(uint8_t adc_index, uint8_t channel_index)
{
	uint8_t* active_buffer = &current_buffer[adc_index][channel_index];

	*active_buffer = ((*active_buffer) == 0) ? 1 : 0;
	buffers_data_count[adc_index][channel_index] = 0;
}

/////
// Public API

void data_dispatch_init(dispatch_t dispatch_method)
{
	// Store dispatch method
	dispatch_type = dispatch_method;

	// Prepare arrays for each ADC
	enabled_channels_count = (uint8_t*)    k_malloc(ADC_COUNT * sizeof(uint8_t));
	adc_channel_buffers    = (uint16_t****)k_calloc(ADC_COUNT,  sizeof(uint16_t***));
	buffers_data_count     = (uint32_t**)  k_calloc(ADC_COUNT,  sizeof(uint32_t*));
	current_buffer         = (uint8_t**)   k_calloc(ADC_COUNT,  sizeof(uint8_t*));
	peek_memory            = (uint16_t**)  k_calloc(ADC_COUNT,  sizeof(uint16_t*));

	// Configure DMA 1 channels
	for (uint8_t adc_num = 1 ; adc_num <= ADC_COUNT ; adc_num++)
	{
		uint8_t adc_index = adc_num-1;
		enabled_channels_count[adc_index] = adc_get_enabled_channels_count(adc_num);

		if (enabled_channels_count[adc_index] > 0)
		{
			// Prepare buffers for DMA
			size_t dma_buffer_size;

			if (dispatch_type == interrupt)
			{
				dma_buffer_size = enabled_channels_count[adc_index];

				// DMA double-buffering
				dma_buffer_size = dma_buffer_size * 2;
			}
			else
			{
				uint32_t repetition;
				if (scheduling_get_uninterruptible_synchronous_task_interrupt_source() == source_hrtim)
				{
					repetition = hrtim_PeriodicEvent_GetRep(MSTR);
				}
				else
				{
					uint32_t hrtim_period_us = leg_get_period_us();
					uint32_t task_period_us = scheduling_get_uninterruptible_synchronous_task_period_us();
					repetition = task_period_us / hrtim_period_us;
				}

				dma_buffer_size = repetition;

				// Make sure buffer size is a multiple of enabled channels count
				// so that each channel data will always be at the same position
				if (repetition % enabled_channels_count[adc_index] != 0)
				{
					dma_buffer_size += enabled_channels_count[adc_index] - (repetition % enabled_channels_count[adc_index]);
				}
				else
				{
					// Add room for one additional measure per channel.
					// This prevents DMA buffer to do exactly one rotation
					// between two tasks calls, to prevent edge cases in
					// acquired data count computation.
					dma_buffer_size += enabled_channels_count[adc_index];
				}
			}

			dma_buffer_sizes[adc_index] = dma_buffer_size;
			dma_main_buffers[adc_index] = (uint16_t*)k_malloc(dma_buffer_size * sizeof(uint16_t));
			if (dispatch_type == interrupt)
			{
				dma_secondary_buffers[adc_index] = dma_main_buffers[adc_index] + enabled_channels_count[adc_index];
			}

			// Initialize DMA
			bool disable_interrupts = false;
			if (dispatch_type == task)
			{
				disable_interrupts = true;
			}
			dma_configure_adc_acquisition(adc_num, disable_interrupts, dma_main_buffers[adc_index], dma_buffer_size);

			// Prepare arrays for each channel
			adc_channel_buffers[adc_index] = (uint16_t***)k_malloc(enabled_channels_count[adc_index] * sizeof(uint16_t**));

			buffers_data_count[adc_index] = (uint32_t*)k_calloc(enabled_channels_count[adc_index], sizeof(uint32_t));
			current_buffer[adc_index]     = (uint8_t*) k_calloc(enabled_channels_count[adc_index], sizeof(uint8_t));
			peek_memory[adc_index]        = (uint16_t*)k_calloc(enabled_channels_count[adc_index], sizeof(uint16_t));
			for (int channel_index = 0 ; channel_index < enabled_channels_count[adc_index] ; channel_index++)
			{
				// Prepare double buffer
				adc_channel_buffers[adc_index][channel_index] = (uint16_t**)k_malloc(sizeof(uint16_t*) * 2);
				adc_channel_buffers[adc_index][channel_index][0] = (uint16_t*)k_malloc(sizeof(uint16_t) * CHANNELS_BUFFERS_SIZE);
				adc_channel_buffers[adc_index][channel_index][1] = (uint16_t*)k_malloc(sizeof(uint16_t) * CHANNELS_BUFFERS_SIZE);

				peek_memory[adc_index][channel_index] = PEEK_NO_VALUE;
			}
		}
	}
}

void data_dispatch_do_dispatch(uint8_t adc_num)
{
	uint8_t adc_index = adc_num - 1;

	if (enabled_channels_count[adc_index] == 0)
		return;

	uint16_t* dma_buffer = dma_main_buffers[adc_index];
	if (dma_secondary_buffers[adc_index] != nullptr)
	{
		if (current_dma_buffer[adc_index] == 0)
		{
			current_dma_buffer[adc_index] = 1;
		}
		else
		{
			dma_buffer = dma_secondary_buffers[adc_index];
			current_dma_buffer[adc_index] = 0;
		}
	}

	size_t data_count_in_dma_buffer;
	if (dispatch_type == interrupt)
	{
		data_count_in_dma_buffer = enabled_channels_count[adc_index];
	}
	else
	{
		data_count_in_dma_buffer = dma_get_retreived_data_count(adc_num);
	}

	for (size_t dma_index = 0 ; dma_index < data_count_in_dma_buffer ; dma_index++)
	{
		// Copy data
		size_t dma_buffer_index;
		if (dispatch_type == interrupt)
		{
			dma_buffer_index = dma_index % enabled_channels_count[adc_index];
		}
		else
		{
			static size_t next_dma_buffer_index[ADC_COUNT] = {0};

			dma_buffer_index = next_dma_buffer_index[adc_index];
			if (next_dma_buffer_index[adc_index] < dma_buffer_sizes[adc_index]-1)
			{
				next_dma_buffer_index[adc_index]++;
			}
			else
			{
				next_dma_buffer_index[adc_index] = 0;
			}
		}

		// Get info on buffer
		size_t channel_index = dma_buffer_index % enabled_channels_count[adc_index];
		uint16_t* active_buffer = _data_dispatch_get_buffer(adc_index, channel_index);
		uint32_t  current_count = _data_dispatch_get_count(adc_index, channel_index);

		active_buffer[current_count] = dma_buffer[dma_buffer_index];

		// Increment count
		_data_dispatch_increment_count(adc_index, channel_index);
	}
}

void data_dispatch_do_full_dispatch()
{
	for (uint8_t adc_num = 1 ; adc_num <= ADC_COUNT ; adc_num++)
	{
		data_dispatch_do_dispatch(adc_num);
	}
}


/////
// Accessors

uint16_t* data_dispatch_get_acquired_values(uint8_t adc_number, uint8_t channel_rank, uint32_t& number_of_values_acquired)
{
	// Prepare default value
	number_of_values_acquired = 0;

	// Check index
	uint8_t adc_index = adc_number-1;
	if (adc_index >= ADC_COUNT)
		return nullptr;

	// Get and check data count
	uint32_t current_count = _data_dispatch_get_count(adc_index, channel_rank);
	if (current_count == 0)
		return nullptr;

	// Get and swap buffer
	uint16_t* active_buffer = _data_dispatch_get_buffer(adc_index, channel_rank);
	_data_dispatch_swap_buffers(adc_index, channel_rank);

	// Retain latest value for peek() functions
	if (current_count > 0)
	{
		peek_memory[adc_number][channel_rank] = active_buffer[current_count-1];
	}

	// Return data
	number_of_values_acquired = current_count;
	return active_buffer;
}

uint16_t data_dispatch_peek_acquired_value(uint8_t adc_number, uint8_t channel_rank)
{
	uint8_t adc_index = adc_number-1;
	if (adc_index < ADC_COUNT)
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
