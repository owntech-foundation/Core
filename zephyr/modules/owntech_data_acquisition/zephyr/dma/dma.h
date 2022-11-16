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
 * @brief  This file provides DMA configuration.
 */

#ifndef DMA_H_
#define DMA_H_


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * This function configures and starts the DMA.
 * It must only be called after all the ADCs configuration
 * has been carried out, as it uses its channels
 * configuration to determine the size of the buffers.
 *
 * @param adc_count Number of configured ADCs.
 */
void dma_configure_and_start(uint8_t adc_count);

// For debug purpose
uint16_t* dma_get_dma1_buffer();
uint16_t* dma_get_dma2_buffer();
uint16_t* dma_get_dma3_buffer();
uint16_t* dma_get_dma4_buffer();


#ifdef __cplusplus
}
#endif

#endif // DMA_H_
