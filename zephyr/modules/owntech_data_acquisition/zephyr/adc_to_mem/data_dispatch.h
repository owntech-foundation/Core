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
 *
 * @brief Data dispatch is intended at dispatching ADCs
 * acquired data from DMA buffers to per-channel buffers.
 * User can then request the data of a specific channel.
 *
 * It uses double-buffering, holding 2 buffers for each
 * enabled channel of each ADC, one being filled and one
 * made available to the user.
 */

#ifndef DATA_DISPATCH_H_
#define DATA_DISPATCH_H_


// Stdlib
#include <stdint.h>


const uint16_t PEEK_NO_VALUE = 0xFFFF;

/**
 * Dispatch method
 */
typedef enum {task, interrupt} dispatch_t;

/**
 * Init function to be called first.
 */
void data_dispatch_init(dispatch_t dispatch_method);

/**
 * Dispatch function: gets the readings and store them
 * in per-channel arrays. This functon is called by DMA
 * callback when the DMA has filled one of its buffers.
 *
 * @param adc_number Number of the ADC from which data comes.
 */
void data_dispatch_do_dispatch(uint8_t adc_number);

/**
 * Function to proceed to all chanels dispatch when
 * it is done at uninterruptible task start.
 */
void data_dispatch_do_full_dispatch();

/**
 * Obtain data for a specific channel.
 * The data is provided as an array of values
 * and the count of data in this buffer is returned
 * as an output parameter.
 *
 * @param  adc_number Number of the ADC from which to
 *         obtain data.
 * @param  channel_rank Rank of the channel from which
 *         to obtain data.
 * @param  number_of_values_acquired Output parameter:
 *         address to a variable that will be updated
 *         by the function with the data count.
 * @return Buffer containing the available data.
 *         Note that the returned buffer is invalidated
 *         by further calls to the function with same
 *         adc number/channel rank.
 */
uint16_t* data_dispatch_get_acquired_values(uint8_t adc_number, uint8_t channel_rank, uint32_t& number_of_values_acquired);

/**
 * Peek data for a specific channel:
 * obtain the latest value from the channel without
 * removing it from the buffer.
 *
 * @param  adc_number Number of the ADC from which to
 *         obtain data.
 * @param  channel_rank Rank of the channel from which
 *         to obtain data.
 * @return Latest available value from the buffer.
 *         Note that if no value has been acquired,
 *         returned value will be 0.
 */
uint16_t data_dispatch_peek_acquired_value(uint8_t adc_number, uint8_t channel_rank);


#endif // DATA_DISPATCH_H_
