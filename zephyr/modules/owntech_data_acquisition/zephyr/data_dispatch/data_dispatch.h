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
 *
 * @brief This is a quick and dirty dispatch of ADC
 * data in per-DMA channel independent buffers.
 */

#ifndef DATA_DISPATCH_H_
#define DATA_DISPATCH_H_


// Stdlib
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


// Init function to be called first
void data_dispatch_init();

// Dispatch function: gets the readings and store them in per-channel arrays
void data_dispatch_do_dispatch();

// Get the name of a specific channel
char* data_dispatch_get_adc1_channel_name(uint8_t channel_rank);
char* data_dispatch_get_adc2_channel_name(uint8_t channel_rank);

// Get number of readings available for a specific channel
uint32_t data_dispatch_get_values_available_in_adc1_channel(uint8_t channel_rank);
uint32_t data_dispatch_get_values_available_in_adc2_channel(uint8_t channel_rank);

// Pop next value from the buffer of a specific channel
// WARNING: calling one of these function when values_available is 0
// will break the behavior by desynchronizing the buffer indexes
uint16_t data_dispatch_get_next_value_from_adc1_channel(uint8_t channel_rank);
uint16_t data_dispatch_get_next_value_from_adc2_channel(uint8_t channel_rank);

// Error function: tells how many values have been missed since
// last call of the function (error count is reset at each call)
uint32_t data_dispatch_how_many_adc1_reading_been_lost();
uint32_t data_dispatch_how_many_adc2_reading_been_lost();


#ifdef __cplusplus
}
#endif

#endif // DATA_DISPATCH_H_
