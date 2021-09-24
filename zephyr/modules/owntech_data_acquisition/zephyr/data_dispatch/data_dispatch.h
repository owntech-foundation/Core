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


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


// Init function to be called first
void data_dispatch_init();

// Dispatch function: gets the readings and store them in per-channel arrays
void data_dispatch_do_dispatch(uint8_t adc_num, uint16_t* dma_buffer);

// Obtain buffer for a specific channel
uint16_t* data_dispatch_get_acquired_values(uint8_t adc_number, uint8_t channel_rank, uint32_t* number_of_values_acquired);

#ifdef __cplusplus
}
#endif

#endif // DATA_DISPATCH_H_
