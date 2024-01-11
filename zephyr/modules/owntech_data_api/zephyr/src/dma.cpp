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
 */


// Stdlib
#include <stdint.h>

// Zephyr
#include <zephyr/kernel.h>
#include <zephyr/drivers/dma.h>

// STM32
#include <stm32_ll_dma.h>

// Current module private functions
#include "data_dispatch.h"


/////
// DT definition

static const struct device* dma1 = DEVICE_DT_GET(DT_NODELABEL(dma1));


/////
// Local variables

static const uint32_t source_registers[5] =
{
	(uint32_t)(&(ADC1->DR)),
	(uint32_t)(&(ADC2->DR)),
	(uint32_t)(&(ADC3->DR)),
	(uint32_t)(&(ADC4->DR)),
	(uint32_t)(&(ADC5->DR))
};

static const uint32_t source_triggers[5] =
{
	LL_DMAMUX_REQ_ADC1,
	LL_DMAMUX_REQ_ADC2,
	LL_DMAMUX_REQ_ADC3,
	LL_DMAMUX_REQ_ADC4,
	LL_DMAMUX_REQ_ADC5
};

static size_t buffers_sizes[5] = {0};


/////
// Private API

/**
 * DMA callback
 * This callback is called on DMA interrupt.
 * For ADCs with enabled interrupt, is will be called
 * twice: when buffer is half-filled and when buffer is filled.
 * For other ADCs, it will never be called.
 */
static void _dma_callback(const struct device* dev, void* user_data, uint32_t dma_channel, int status)
{
	UNUSED(dev);
	UNUSED(user_data);
	UNUSED(status);

	// DMA channel value comes raw from LL (starts at 0): convert to number
	uint8_t channel_number = dma_channel + 1;
	data_dispatch_do_dispatch(channel_number);
}


/////
// Public API

void dma_configure_adc_acquisition(uint8_t adc_number, bool disable_interrupts, uint16_t* buffer, size_t buffer_size)
{
	// Check environment
	if (device_is_ready(dma1) == false)
		return;

	uint8_t dma_index = adc_number - 1;
	uint32_t buffer_size_bytes = (uint32_t) buffer_size * sizeof(uint16_t);
	buffers_sizes[dma_index] = buffer_size;

	// Configure DMA
	struct dma_block_config dma_block_config_s = {0};
	dma_block_config_s.source_address   = source_registers[dma_index]; // Source: ADC DR register
	dma_block_config_s.dest_address     = (uint32_t)buffer;            // Dest: buffer in memory
	dma_block_config_s.block_size       = buffer_size_bytes;           // Buffer size in bytes
	dma_block_config_s.source_addr_adj  = DMA_ADDR_ADJ_NO_CHANGE;      // Source: no increment in ADC register
	dma_block_config_s.dest_addr_adj    = DMA_ADDR_ADJ_INCREMENT;      // Dest: increment in memory
	dma_block_config_s.dest_reload_en   = 1;                           // Reload destination address on block completion
	dma_block_config_s.source_reload_en = 1;                           // Reload source address on block completion; Enables Half-transfer interrupt

	struct dma_config dma_config_s = {0};
	dma_config_s.dma_slot            = source_triggers[dma_index]; // Trigger source: ADC
	dma_config_s.channel_direction   = PERIPHERAL_TO_MEMORY;       // From periph to mem
	dma_config_s.source_data_size    = 2;                          // Source: 2 bytes (uint16_t)
	dma_config_s.dest_data_size      = 2;                          // Dest: 2 bytes (uint16_t)
	dma_config_s.source_burst_length = 1;                          // Source: No burst
	dma_config_s.dest_burst_length   = 1;                          // Dest: No burst
	dma_config_s.block_count         = 1;                          // 1 block
	dma_config_s.head_block          = &dma_block_config_s;        // Block config as defined above
	dma_config_s.dma_callback        = _dma_callback;              // DMA interrupt callback

	dma_config(dma1, adc_number, &dma_config_s);

	if (disable_interrupts == true)
	{
		LL_DMA_DisableIT_HT(DMA1, dma_index);
		LL_DMA_DisableIT_TC(DMA1, dma_index);
	}

	dma_start(dma1, adc_number);
}

uint32_t dma_get_retreived_data_count(uint8_t adc_number)
{
	// Permanent variable
	// -1 is equivalent to (buffer size - 1) in modulo arithmetics
	static int32_t previous_dma_latest_data_pointers[5] = {-1, -1, -1, -1, -1};

	// Get data
	uint32_t dma_index = adc_number - 1;
	uint32_t dma_remaining_data = LL_DMA_GetDataLength(DMA1, dma_index);
	int32_t previous_dma_latest_data_pointer = previous_dma_latest_data_pointers[dma_index];

	// Compute pointers
	int32_t dma_next_data_pointer = buffers_sizes[dma_index] - dma_remaining_data;
	int32_t dma_latest_data_pointer = dma_next_data_pointer - 1;

	int32_t corrected_dma_pointer = dma_latest_data_pointer;
	if (dma_latest_data_pointer < previous_dma_latest_data_pointer)
	{
		corrected_dma_pointer += buffers_sizes[dma_index];
	}

	uint32_t retreived_data = corrected_dma_pointer - previous_dma_latest_data_pointer;
	previous_dma_latest_data_pointers[dma_index] = dma_latest_data_pointer;

	return retreived_data;
}
