/*
 * Copyright (c) 2021 LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


// Stdlib
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

//Zephyr
#include <zephyr.h>

// OwnTech API
#include "../adc/adc_channels.h"


/////
// General data

#define CHANNELS_BUFFERS_SIZE 32

// Number of channels in each ADC (cell i is ADC number i+1)
static uint8_t enabled_channels_count[3];


/////
// Per-channel buffers

// Bidimentionnal arrays: each of these variables is an array
// of per-channel arrays holding the readings
static uint16_t** adc1_channels_buffers = NULL;
static uint16_t** adc2_channels_buffers = NULL;

// Arrays to store the read and write indexes in each channel array
static uint32_t* adc1_next_read_indexes  = NULL;
static uint32_t* adc2_next_read_indexes  = NULL;
static uint32_t* adc1_next_write_indexes = NULL;
static uint32_t* adc2_next_write_indexes = NULL;

static uint16_t** adc1_user_buffers = NULL;
static uint16_t** adc2_user_buffers = NULL;

/////
// Private functions

__STATIC_INLINE void _increment_circular_index(uint32_t* buffer_index, uint32_t buffer_size)
{
	(*buffer_index) =  ( (*buffer_index) < (buffer_size-1) ) ? ( (*buffer_index) + 1 ) : 0;
}

__STATIC_INLINE void _circular_buffer_copy(uint16_t* src_buffer,                 uint16_t* dest_buffer,
                                           uint32_t  src_buffer_next_read_index, uint32_t  dest_buffer_next_write_index
                                          )
{
	dest_buffer[dest_buffer_next_write_index] = src_buffer[src_buffer_next_read_index];
}

static uint32_t _get_values_available_count_in_buffer(uint32_t next_read_index, uint32_t next_write_index, uint32_t buffer_size)
{
	if (next_read_index == next_write_index)
		return 0;

	uint32_t i_next_write_index = (next_write_index < next_read_index) ? (next_write_index + buffer_size) : next_write_index;
	return i_next_write_index - next_read_index;
}

static uint16_t _get_next_value_from_buffer(uint16_t* buffer, uint32_t* next_read_index, uint32_t buffer_size)
{
	uint16_t value = buffer[*next_read_index];
	_increment_circular_index(next_read_index, buffer_size);
	return value;
}


/////
// Public API

void data_dispatch_init()
{
	enabled_channels_count[0] = adc_channels_get_enabled_channels_count(1);
	if (enabled_channels_count[0] > 0)
	{
		adc1_channels_buffers   = k_malloc(sizeof(uint16_t*) * enabled_channels_count[0]);
		adc1_next_read_indexes  = k_malloc(sizeof(uint32_t)  * enabled_channels_count[0]);
		adc1_next_write_indexes = k_malloc(sizeof(uint32_t)  * enabled_channels_count[0]);
		adc1_user_buffers       = k_malloc(sizeof(uint16_t*) * enabled_channels_count[0]);
		for (int i = 0 ; i < enabled_channels_count[0] ; i++)
		{
			adc1_channels_buffers[i]   = k_malloc(sizeof(uint16_t) * CHANNELS_BUFFERS_SIZE);
			adc1_next_read_indexes[i]  = 0;
			adc1_next_write_indexes[i] = 0;
			adc1_user_buffers[i]       = k_malloc(sizeof(uint16_t) * CHANNELS_BUFFERS_SIZE);
		}
	}

	enabled_channels_count[1] = adc_channels_get_enabled_channels_count(2);
	if (enabled_channels_count[1] > 0)
	{
		adc2_channels_buffers   = k_malloc(sizeof(uint16_t*) * enabled_channels_count[1]);
		adc2_next_read_indexes  = k_malloc(sizeof(uint32_t)  * enabled_channels_count[1]);
		adc2_next_write_indexes = k_malloc(sizeof(uint32_t)  * enabled_channels_count[1]);
		adc2_user_buffers       = k_malloc(sizeof(uint16_t*) * enabled_channels_count[1]);
		for (int i = 0 ; i < enabled_channels_count[1] ; i++)
		{
			adc2_channels_buffers[i]   = k_malloc(sizeof(uint16_t) * CHANNELS_BUFFERS_SIZE);
			adc2_next_read_indexes[i]  = 0;
			adc2_next_write_indexes[i] = 0;
			adc2_user_buffers[i]       = k_malloc(sizeof(uint16_t) * CHANNELS_BUFFERS_SIZE);
		}
	}
}

void data_dispatch_do_dispatch(uint8_t adc_num, uint16_t* dma_buffer)
{
	if (adc_num == 1)
	{
		for (int current_channel = 0 ; current_channel < enabled_channels_count[0] ; current_channel++)
		{
			_circular_buffer_copy(dma_buffer,      adc1_channels_buffers[current_channel],
			                      current_channel, adc1_next_write_indexes[current_channel]
			                     );

			_increment_circular_index(&adc1_next_write_indexes[current_channel], CHANNELS_BUFFERS_SIZE);
		}
	}
	else if (adc_num == 2)
	{
		for (int current_channel = 0 ; current_channel < enabled_channels_count[1] ; current_channel++)
		{
			_circular_buffer_copy(dma_buffer,      adc2_channels_buffers[current_channel],
			                      current_channel, adc2_next_write_indexes[current_channel]
			                     );

			_increment_circular_index(&adc2_next_write_indexes[current_channel], CHANNELS_BUFFERS_SIZE);
		}
	}
}


/////
// Accessors

uint16_t* data_dispatch_get_acquired_values(uint8_t adc_number, uint8_t channel_rank, uint32_t* number_of_values_acquired)
{
	if (adc_number == 1)
	{
		// Get number of available values
		uint32_t number_of_values = _get_values_available_count_in_buffer(adc1_next_read_indexes[channel_rank],
		                                                                  adc1_next_write_indexes[channel_rank],
		                                                                  CHANNELS_BUFFERS_SIZE
		                                                                 );

		for (uint32_t i = 0 ; i < number_of_values ; i++)
		{
			adc1_user_buffers[channel_rank][i] = _get_next_value_from_buffer(adc1_channels_buffers[channel_rank],
			                                                                &adc1_next_read_indexes[channel_rank],
			                                                                CHANNELS_BUFFERS_SIZE
			                                                               );
		}

		*number_of_values_acquired = number_of_values;
		return adc1_user_buffers[channel_rank];
	}
	else if (adc_number == 2)
	{
		// Get number of available values
		uint32_t number_of_values = _get_values_available_count_in_buffer(adc2_next_read_indexes[channel_rank],
		                                                                  adc2_next_write_indexes[channel_rank],
		                                                                  CHANNELS_BUFFERS_SIZE
		                                                                 );

		for (uint32_t i = 0 ; i < number_of_values ; i++)
		{
			adc2_user_buffers[channel_rank][i] = _get_next_value_from_buffer(adc2_channels_buffers[channel_rank],
			                                                                 &adc2_next_read_indexes[channel_rank],
			                                                                CHANNELS_BUFFERS_SIZE
			                                                                );
		}

		*number_of_values_acquired = number_of_values;
		return adc2_user_buffers[channel_rank];
	}
	else
	{
		*number_of_values_acquired = 0;
		return NULL;
	}
}
