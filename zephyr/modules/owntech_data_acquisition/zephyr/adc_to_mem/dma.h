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


#ifdef __cplusplus
extern "C" {
#endif


/**
 * This function configures a channel from DMA 1
 * to transfer measures  from an ADC to buffers,
 * then starts the channels.
 * It must only be called after all the ADCs configuration
 * has been carried out, as it uses its channels
 * configuration to determine the size of the buffers.
 *
 * @param adc_number Number of the ADC to acquire measures from.
 * @param enable_double_buffering Boolean indicating whether
 *        double buffering is to be activated.
 *        - If false, the buffer will be treated as a single buffer with
 *          interrupt being triggered only when it is full.
 *        - If true the buffer will be treated as two consecutive sub-buffers,
 *          with interrupt being triggered as soon as a sub-buffer is full.
 * @param buffer Pointer to buffer.
 * @param buffer_size Size of the buffer in bytes.
 */
void dma_configure_adc_acquisition(uint8_t adc_number, bool enable_double_buffering, uint16_t* buffer, size_t buffer_size);


#ifdef __cplusplus
}
#endif

#endif // DMA_H_
