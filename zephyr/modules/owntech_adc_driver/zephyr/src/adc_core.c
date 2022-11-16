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
#include <stdint.h>

// Zephyr
#include <zephyr.h>

// STM32 LL
#include <stm32_ll_bus.h>

// Owntech API
#include "adc_helper.h"


/////
// Private functions

/**
 * ADC wake-up.
 * Refer to RM 21.4.6
 */
static void _adc_core_wakeup(uint8_t adc_num)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	// Disable deep power down
	LL_ADC_DisableDeepPowerDown(adc);

	// Enable internal regulator
	LL_ADC_EnableInternalRegulator(adc);

	// Wait for ADC voltage regulator start-up time
	k_busy_wait(LL_ADC_DELAY_INTERNAL_REGUL_STAB_US);
}

/**
 * ADC calibration.
 * Refer to RM 21.4.8
 */
static void _adc_core_calibrate(uint8_t adc_num)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	// Single ended calibration
	LL_ADC_StartCalibration(adc, LL_ADC_SINGLE_ENDED);
	while ( LL_ADC_IsCalibrationOnGoing(adc) ) { /* Wait */ }

	// Seems to require an additionnal delay between calibrations
	// TODO: undocumented???
	k_busy_wait(10);

	// Differential ended calibration
	LL_ADC_StartCalibration(adc, LL_ADC_DIFFERENTIAL_ENDED);
	while ( LL_ADC_IsCalibrationOnGoing(adc) ) { /* Wait */ }
}


/////
// Public API

void adc_core_set_dual_mode(uint8_t dual_mode)
{
	if (dual_mode != 0)
	{
		LL_ADC_SetMultimode(ADC12_COMMON, LL_ADC_MULTI_DUAL_REG_SIMULT);
	}
	else
	{
		LL_ADC_SetMultimode(ADC12_COMMON, LL_ADC_MULTI_INDEPENDENT);
	}
}

void adc_core_enable(uint8_t adc_num)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	// Enable ADC and wait for it to be ready
	LL_ADC_ClearFlag_ADRDY(adc);
	LL_ADC_Enable(adc);
	while (LL_ADC_IsActiveFlag_ADRDY(adc) == 0) { /* Wait */ }
}

void adc_core_start(uint8_t adc_num)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	LL_ADC_REG_StartConversion(adc);
}

void adc_core_configure_dma_mode(uint8_t adc_num)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	LL_ADC_REG_SetDMATransfer(adc, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
}

void adc_core_configure_trigger_source(uint8_t adc_num, uint32_t external_trigger_edge, uint32_t trigger_source)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	// Set trigger edge
	LL_ADC_REG_SetTriggerEdge(adc, external_trigger_edge);

	// Set trigger source
	LL_ADC_REG_SetTriggerSource(adc, trigger_source);
}

void adc_core_configure_discontinuous_mode(uint8_t adc_num, uint32_t discontinuous_count)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	uint32_t discontinuous_mode;

	switch(discontinuous_count)
	{
		case 1:
			discontinuous_mode = LL_ADC_REG_SEQ_DISCONT_1RANK;
			break;
		case 2:
			discontinuous_mode = LL_ADC_REG_SEQ_DISCONT_2RANKS;
			break;
		case 3:
			discontinuous_mode = LL_ADC_REG_SEQ_DISCONT_3RANKS;
			break;
		case 4:
			discontinuous_mode = LL_ADC_REG_SEQ_DISCONT_4RANKS;
			break;
		case 5:
			discontinuous_mode = LL_ADC_REG_SEQ_DISCONT_5RANKS;
			break;
		case 6:
			discontinuous_mode = LL_ADC_REG_SEQ_DISCONT_6RANKS;
			break;
		case 7:
			discontinuous_mode = LL_ADC_REG_SEQ_DISCONT_7RANKS;
			break;
		case 8:
			discontinuous_mode = LL_ADC_REG_SEQ_DISCONT_8RANKS;
			break;
		default:
			discontinuous_mode = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	}

	LL_ADC_REG_SetSequencerDiscont(adc, discontinuous_mode);
}

void adc_core_init()
{
	// Enable ADCs clocks
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);

	// Wake-up ADCs
	_adc_core_wakeup(1);
	_adc_core_wakeup(2);
	_adc_core_wakeup(3);
	_adc_core_wakeup(4);

	// Set common clock between ADC 1 and ADC 2
	// Refer to RM 21.4.3 and 21.7.2
	LL_ADC_SetCommonClock(ADC12_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV4);
	LL_ADC_SetCommonClock(ADC345_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV4);

	// Calibrate ADCs
	_adc_core_calibrate(1);
	_adc_core_calibrate(2);
	_adc_core_calibrate(3);
	_adc_core_calibrate(4);
}
