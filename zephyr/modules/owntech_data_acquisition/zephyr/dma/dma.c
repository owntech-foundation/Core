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
 * @brief  DMA configuration for OwnTech application
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

// Stdlib
#include <stdlib.h>

// Zephyr
#include <drivers/dma.h>

// STM32
#include <stm32g4xx_ll_dma.h>

// OwnTech API
#include "../adc/adc_channels.h"

// Current file header
#include "dma.h"


/////
// DT definitions
#define DMA1_NODELABEL DT_NODELABEL(dma1)
#define DMA1_LABEL     DT_PROP(DMA1_NODELABEL, label)

#define DMA2_NODELABEL DT_NODELABEL(dma2)
#define DMA2_LABEL     DT_PROP(DMA2_NODELABEL, label)

// This is the size of the buffer, in terms of readings
// *per channel*. E.g. if 3 channels in an ADC,
// the size of the buffer will be 3*DMA_BUFFER_SIZE
// This value *must be* a multiple of 2 to allow
// half-transfer interrupts correct handling.
#define DMA_BUFFER_SIZE 2

/////
// Variables
atomic_t dma1_readings_count = ATOMIC_INIT(0);
atomic_t dma2_readings_count = ATOMIC_INIT(0);

static uint16_t* dma1_buffer = NULL;
static uint16_t* dma2_buffer = NULL;

static size_t buffer1_size;
static size_t buffer2_size;


/////
// DMA callbacks
// These callbacks are called twice per buffer filling:
// when buffer is half-filled and when buffer is filled.

void _dma1_callback(const struct device* dev, void* user_data, uint32_t channel, int status)
{
    atomic_inc(&dma1_readings_count);
}

void _dma2_callback(const struct device* dev, void* user_data, uint32_t channel, int status)
{
    atomic_inc(&dma2_readings_count);
}


/////
// DMA inits

static void _dma1_init()
{
    // Prepare buffers
    buffer1_size = adc_channels_get_channels_count(1) * DMA_BUFFER_SIZE;
    dma1_buffer = malloc(sizeof(uint16_t) * buffer1_size);

    // Configure DMA
    struct dma_block_config dma1_block_config = {0};
    struct dma_config       dma1_config       = {0};

    dma1_block_config.source_address   = (uint32_t)(&(ADC1->DR)); // Source: ADC 1
    dma1_block_config.dest_address     = (uint32_t)dma1_buffer;   // Dest: array in mem
    dma1_block_config.block_size       = 2 * buffer1_size;        // Buffer size in bytes
    dma1_block_config.source_addr_adj  = DMA_ADDR_ADJ_NO_CHANGE;  // Source: no increment in DMA register
    dma1_block_config.dest_addr_adj    = DMA_ADDR_ADJ_INCREMENT;  // Dest: increment in memory
    dma1_block_config.source_reload_en = 1;                       // Reload initial address after block
    dma1_block_config.dest_reload_en   = 1;                       // Reload initial address after block

    dma1_config.dma_slot            = LL_DMAMUX_REQ_ADC1;   // Source: ADC 1
    dma1_config.channel_direction   = PERIPHERAL_TO_MEMORY; // From periph to mem
    dma1_config.source_data_size    = 2;                    // 2 bytes
    dma1_config.dest_data_size      = 2;                    // 2 bytes
    dma1_config.source_burst_length = 1;                    // No burst
    dma1_config.dest_burst_length   = 1;                    // No burst
    dma1_config.block_count         = 1;                    // 1 block
    dma1_config.head_block          = &dma1_block_config;   // Above block config
    dma1_config.dma_callback        = _dma1_callback;       // Callback

    const struct device* _dma1 = device_get_binding(DMA1_LABEL);
    dma_config(_dma1, 1, &dma1_config);
    // Half-transfer interrupt is not handled by Zephyr driver:
    // enable it manually.
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_1);
}

static void _dma2_init()
{
    // Prepare buffers
    buffer2_size = adc_channels_get_channels_count(2) * DMA_BUFFER_SIZE;
    dma2_buffer = malloc(sizeof(uint16_t) * buffer2_size);

    // Configure DMA
    struct dma_block_config dma2_block_config = {0};
    struct dma_config       dma2_config       = {0};

    dma2_block_config.source_address   = (uint32_t)(&(ADC2->DR)); // Source: ADC 2
    dma2_block_config.dest_address     = (uint32_t)dma2_buffer;   // Dest: array in mem
    dma2_block_config.block_size       = 2 * buffer2_size;        // Buffer size in bytes
    dma2_block_config.source_addr_adj  = DMA_ADDR_ADJ_NO_CHANGE;  // Source: no increment in DMA register
    dma2_block_config.dest_addr_adj    = DMA_ADDR_ADJ_INCREMENT;  // Dest: increment in memory
    dma2_block_config.source_reload_en = 1;                       // Reload initial address after block
    dma2_block_config.dest_reload_en   = 1;                       // Reload initial address after block

    dma2_config.dma_slot            = LL_DMAMUX_REQ_ADC2;   // Source: ADC 2
    dma2_config.channel_direction   = PERIPHERAL_TO_MEMORY; // From periph to mem
    dma2_config.source_data_size    = 2;                    // 2 bytes
    dma2_config.dest_data_size      = 2;                    // 2 bytes
    dma2_config.source_burst_length = 1;                    // No burst
    dma2_config.dest_burst_length   = 1;                    // No burst
    dma2_config.block_count         = 1;                    // 1 block
    dma2_config.head_block          = &dma2_block_config;   // Above block config
    dma2_config.dma_callback        = _dma2_callback;       // Callback

    const struct device* _dma2 = device_get_binding(DMA2_LABEL);
    dma_config(_dma2, 1, &dma2_config);
    // Half-transfer interrupt is not handled by Zephyr driver:
    // enable it manually.
    LL_DMA_EnableIT_HT(DMA2, LL_DMA_CHANNEL_1);
}


/////
// Public API

/**
 * DMA initialization procedure.
 */
void dma_init()
{
    __ASSERT( (DMA_BUFFER_SIZE%2 == 0), "WARNING: DMA_BUFFER_SIZE macro must be an even value.");

    // DMA1
    _dma1_init();
    const struct device* _dma1 = device_get_binding(DMA1_LABEL);
    dma_start(_dma1, 1);

    // DMA2
    _dma2_init();
    const struct device* _dma2 = device_get_binding(DMA2_LABEL);
    dma_start(_dma2, 1);
}

uint16_t* dma_get_dma1_buffer()
{
    return dma1_buffer;
}

uint16_t* dma_get_dma2_buffer()
{
    return dma2_buffer;
}

size_t dma_get_dma1_buffer_size()
{
    return buffer1_size;
}

size_t dma_get_dma2_buffer_size()
{
    return buffer2_size;
}
