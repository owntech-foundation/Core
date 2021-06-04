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
 * @author  Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef DMA_H_
#define DMA_H_


// Zephyr
#include <zephyr.h>


#ifdef __cplusplus
extern "C" {
#endif


// Public functions
void dma_init();

uint16_t* dma_get_dma1_buffer();
uint16_t* dma_get_dma2_buffer();

size_t dma_get_dma1_buffer_size();
size_t dma_get_dma2_buffer_size();

// Public variables: these variables handle
// the number of readings stored in the
// buffer at a specific time. They are
// incremented by 1 by the DMA transfer
// interrupts and can be decremented by
// a task reading the buffers.
// Each unit in these counts represent
// a half-buffer amount of readings.
extern atomic_t dma1_readings_count;
extern atomic_t dma2_readings_count;


#ifdef __cplusplus
}
#endif

#endif // DMA_H_
