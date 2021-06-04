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
#include "../dma/dma.h"
#include "../adc/adc_channels.h"


/////
// General data

#define CHANNELS_BUFFERS_SIZE 32

// Number of channels in each ADC
static uint8_t channels_in_adc1_count;
static uint8_t channels_in_adc2_count;

// Number of readings a DMA buffer can handle
static size_t dma1_buffer_size;
static size_t dma2_buffer_size;

// Number of readings a *half-buffer* can handle
static size_t dma1_half_buffer_size;
static size_t dma2_half_buffer_size;

// Number of *per-channel* readings a *half-buffer* can handle
static uint32_t dma1_half_buffer_capacity;
static uint32_t dma2_half_buffer_capacity;

/////
// DMA buffers (in)

static uint16_t* dma1_buffer = NULL;
static uint16_t* dma2_buffer = NULL;

static uint32_t dma1_next_read_index = 0;
static uint32_t dma2_next_read_index = 0;

/////
// Per-channel buffers (out)

// Bidimentionnal arrays: each of these variables is an array
// of per-channel arrays holding the readings
static uint16_t** adc1_channels_buffers = NULL;
static uint16_t** adc2_channels_buffers = NULL;

// Arrays to store the latest and next indexes in each channel array
static uint32_t* adc1_next_read_indexes  = NULL;
static uint32_t* adc2_next_read_indexes  = NULL;
static uint32_t* adc1_next_write_indexes = NULL;
static uint32_t* adc2_next_write_indexes = NULL;

/////
// Meta-data
static uint32_t readings_missed_in_adc1 = 0;
static uint32_t readings_missed_in_adc2 = 0;


/////
// Private functions

void _increment_circular_index(uint32_t* buffer_index, uint32_t buffer_size)
{
    (*buffer_index) =  ( (*buffer_index) < (buffer_size-1) ) ? ( (*buffer_index) + 1 ) : 0;
}

void _circular_buffer_copy(uint16_t* src_buffer,                 uint16_t* dest_buffer,
                           uint32_t* src_buffer_next_read_index, uint32_t* dest_buffer_next_write_index,
                           uint32_t  src_buffer_size,            uint32_t  dest_buffer_size
                          )
{
    dest_buffer[*dest_buffer_next_write_index] = src_buffer[*src_buffer_next_read_index];

    _increment_circular_index(src_buffer_next_read_index,   src_buffer_size);
    _increment_circular_index(dest_buffer_next_write_index, dest_buffer_size);
}

uint32_t _get_values_available_count_in_buffer(uint32_t next_read_index, uint32_t next_write_index, uint32_t buffer_size)
{
    if (next_read_index == next_write_index)
        return 0;

    uint32_t i_next_write_index = (next_write_index < next_read_index) ? (next_write_index + buffer_size) : next_write_index;
    return i_next_write_index - next_read_index;
}

uint16_t _get_next_value_from_buffer(uint16_t* buffer, uint32_t* next_read_index, uint32_t buffer_size)
{
    uint16_t value = buffer[*next_read_index];
    _increment_circular_index(next_read_index, buffer_size);
    return value;
}


/////
// Public API

void data_dispatch_init()
{
    dma1_buffer = dma_get_dma1_buffer();
    dma2_buffer = dma_get_dma2_buffer();

    dma1_buffer_size = dma_get_dma1_buffer_size();
    dma2_buffer_size = dma_get_dma2_buffer_size();

    dma1_half_buffer_size = dma1_buffer_size/2;
    dma2_half_buffer_size = dma2_buffer_size/2;

    channels_in_adc1_count = adc_channels_get_channels_count(1);
    channels_in_adc2_count = adc_channels_get_channels_count(2);

    dma1_half_buffer_capacity = dma1_half_buffer_size/channels_in_adc1_count;
    dma2_half_buffer_capacity = dma2_half_buffer_size/channels_in_adc2_count;

    adc1_channels_buffers   = malloc(sizeof(uint16_t*) * channels_in_adc1_count);
    adc1_next_read_indexes  = malloc(sizeof(uint32_t)  * channels_in_adc1_count);
    adc1_next_write_indexes = malloc(sizeof(uint32_t)  * channels_in_adc1_count);
    for (int i = 0 ; i < channels_in_adc1_count ; i++)
    {
        adc1_channels_buffers[i]   = malloc(sizeof(uint16_t) * CHANNELS_BUFFERS_SIZE);
        adc1_next_read_indexes[i]  = 0;
        adc1_next_write_indexes[i] = 0;
    }

    adc2_channels_buffers   = malloc(sizeof(uint16_t*) * channels_in_adc2_count);
    adc2_next_read_indexes  = malloc(sizeof(uint32_t)  * channels_in_adc2_count);
    adc2_next_write_indexes = malloc(sizeof(uint32_t)  * channels_in_adc2_count);
    for (int i = 0 ; i < channels_in_adc2_count ; i++)
    {
        adc2_channels_buffers[i]   = malloc(sizeof(uint16_t) * CHANNELS_BUFFERS_SIZE);
        adc2_next_read_indexes[i]  = 0;
        adc2_next_write_indexes[i] = 0;
    }
}

/**
 * This function gets the values from ADC buffers
 * and place them in arrays depending on their origin.
 */
void data_dispatch_do_dispatch()
{
    // DMA 1
    uint32_t _dma1_count = atomic_set(&dma1_readings_count, 0);

    if (_dma1_count > 1)
    {
        uint32_t readings_missed = _dma1_count - 1;
        readings_missed_in_adc1 += readings_missed*dma1_half_buffer_capacity;
        // Catch up to the latest written half-buffer
        if ( (readings_missed % 2) == 1)
        {
            dma1_next_read_index = (dma1_next_read_index + dma1_half_buffer_size) % dma1_buffer_size;
        }
    }

    if (_dma1_count >= 1)
    {
        for (int readings_count = 0 ; readings_count < dma1_half_buffer_capacity ; readings_count++)
        {
            for (int channel_count = 0 ; channel_count < channels_in_adc1_count ; channel_count++)
            {
                _circular_buffer_copy(dma1_buffer,           adc1_channels_buffers[channel_count],
                                      &dma1_next_read_index, &adc1_next_write_indexes[channel_count],
                                      dma1_buffer_size,      CHANNELS_BUFFERS_SIZE
                                     );
            }
        }
    }

    // DMA 2
    uint32_t _dma2_count = atomic_set(&dma2_readings_count, 0);

    if (_dma2_count > 1)
    {
        uint32_t readings_missed = _dma2_count - 1;
        readings_missed_in_adc2 += readings_missed*dma2_half_buffer_capacity;
        // Catch up to the latest written half-buffer
        if ( (readings_missed % 2) == 1)
        {
            dma2_next_read_index = (dma2_next_read_index + dma2_half_buffer_size) % dma2_buffer_size;
        }
    }

    if (_dma2_count >= 1)
    {
        for (int readings_count = 0 ; readings_count < dma2_half_buffer_capacity ; readings_count++)
        {
            for (int channel_count = 0 ; channel_count < channels_in_adc2_count ; channel_count++)
            {
                _circular_buffer_copy(dma2_buffer,           adc2_channels_buffers[channel_count],
                                      &dma2_next_read_index, &adc2_next_write_indexes[channel_count],
                                      dma2_buffer_size,      CHANNELS_BUFFERS_SIZE
                                     );
            }
        }
    }
}


/////
// Accessors

char* data_dispatch_get_adc1_channel_name(uint8_t channel_rank)
{
    return adc_channels_get_channel_name(1, channel_rank);
}

char* data_dispatch_get_adc2_channel_name(uint8_t channel_rank)
{
    return adc_channels_get_channel_name(2, channel_rank);
}

uint32_t data_dispatch_get_values_available_in_adc1_channel(uint8_t channel_rank)
{
    return _get_values_available_count_in_buffer(adc1_next_read_indexes[channel_rank],
                                                 adc1_next_write_indexes[channel_rank],
                                                 CHANNELS_BUFFERS_SIZE
                                                );
}

uint32_t data_dispatch_get_values_available_in_adc2_channel(uint8_t channel_rank)
{
    return _get_values_available_count_in_buffer(adc2_next_read_indexes[channel_rank],
                                                 adc2_next_write_indexes[channel_rank],
                                                 CHANNELS_BUFFERS_SIZE
                                                );
}

uint16_t data_dispatch_get_next_value_from_adc1_channel(uint8_t channel_rank)
{
    return _get_next_value_from_buffer(adc1_channels_buffers[channel_rank],
                                       &adc1_next_read_indexes[channel_rank],
                                       CHANNELS_BUFFERS_SIZE
                                      );
}

uint16_t data_dispatch_get_next_value_from_adc2_channel(uint8_t channel_rank)
{
    return _get_next_value_from_buffer(adc2_channels_buffers[channel_rank],
                                       &adc2_next_read_indexes[channel_rank],
                                       CHANNELS_BUFFERS_SIZE
                                      );
}

uint32_t data_dispatch_how_many_adc1_reading_been_lost()
{
    if (readings_missed_in_adc1 > 0)
    {
        uint32_t error_count = readings_missed_in_adc1;
        readings_missed_in_adc1 = 0;
        return error_count;
    }
    else
    {
        return 0;
    }
}

uint32_t data_dispatch_how_many_adc2_reading_been_lost()
{
    if (readings_missed_in_adc2 > 0)
    {
        uint32_t error_count = readings_missed_in_adc2;
        readings_missed_in_adc2 = 0;
        return error_count;
    }
    else
    {
        return 0;
    }
}
