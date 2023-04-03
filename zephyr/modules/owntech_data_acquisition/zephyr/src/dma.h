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
 * @brief  This file provides DMA configuration to automatically
 *         store ADC acquisitions in a provided buffer.
 *         DMA 1 is used for all acquisitions, with channel n
 *         acquiring values from ADC n.
 */

#ifndef DMA_H_
#define DMA_H_


// Stdlib
#include <stdint.h>


/**
 * @brief This function configures a channel from DMA 1
 * to transfer measures from an ADC to buffers,
 * then starts the channels.
 * It must only be called after all the ADCs configuration
 * has been carried out, as it uses its channels
 * configuration to determine the size of the buffers.
 *
 * @param adc_number Number of the ADC to acquire measures from.
 * @param disable_interrupts Boolean indicating whether interrupts
 *        shoud be disabled. Warning: this override Zephyr DMA
 *        driver defalt behavior.
 * @param buffer Pointer to buffer.
 * @param buffer_size Number of uint16_t words the buffer can contain.
 */
void dma_configure_adc_acquisition(uint8_t adc_number, bool disable_interrupts, uint16_t* buffer, size_t buffer_size);

/**
 * @brief Obtain the number of acquired data since
 *        last time this function was called.
 *
 * @param adc_number Number of the ADC.
 *
 * @return Number of acquired data modulo buffer size.
 */
size_t dma_get_retreived_data_count(uint8_t adc_number);


#endif // DMA_H_
