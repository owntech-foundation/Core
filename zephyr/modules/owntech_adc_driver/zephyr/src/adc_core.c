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
 * @date 2023
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


// Stdlib
#include <stdint.h>

// Zephyr
#include <zephyr.h>

// STM32 LL
#include <stm32_ll_bus.h>


/////
// Constants

#define NUMBER_OF_ADCS 4


/////
// Helper functions

ADC_TypeDef* _get_adc_by_number(uint8_t adc_number)
{
	ADC_TypeDef* adc = NULL;

	if (adc_number == 1)
		adc = ADC1;
	else if (adc_number == 2)
		adc = ADC2;
	else if (adc_number == 3)
		adc = ADC3;
	else if (adc_number == 4)
		adc = ADC4;

	return adc;
}
/**
 * Function to convert ADC decimal rank to litteral.
 * Sadly, there seems to be no equivalent to
 * __LL_ADC_DECIMAL_NB_TO_CHANNEL() for ranks...
 */
uint32_t _adc_decimal_nb_to_rank(uint8_t decimal_rank)
{
	uint32_t ll_rank;
	switch (decimal_rank)
	{
		case 1:
			ll_rank = LL_ADC_REG_RANK_1;
			break;
		case 2:
			ll_rank = LL_ADC_REG_RANK_2;
			break;
		case 3:
			ll_rank = LL_ADC_REG_RANK_3;
			break;
		case 4:
			ll_rank = LL_ADC_REG_RANK_4;
			break;
		case 5:
			ll_rank = LL_ADC_REG_RANK_5;
			break;
		case 6:
			ll_rank = LL_ADC_REG_RANK_6;
			break;
		case 7:
			ll_rank = LL_ADC_REG_RANK_7;
			break;
		case 8:
			ll_rank = LL_ADC_REG_RANK_8;
			break;
		case 9:
			ll_rank = LL_ADC_REG_RANK_9;
			break;
		case 10:
			ll_rank = LL_ADC_REG_RANK_10;
			break;
		case 11:
			ll_rank = LL_ADC_REG_RANK_11;
			break;
		case 12:
			ll_rank = LL_ADC_REG_RANK_12;
			break;
		case 13:
			ll_rank = LL_ADC_REG_RANK_13;
			break;
		case 14:
			ll_rank = LL_ADC_REG_RANK_14;
			break;
		case 15:
			ll_rank = LL_ADC_REG_RANK_15;
			break;
		case 16:
			ll_rank = LL_ADC_REG_RANK_16;
			break;
		default:
			ll_rank = 0;
	}
	return ll_rank;
}


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

void adc_core_enable(uint8_t adc_num)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	// Enable ADC and wait for it to be ready
	LL_ADC_ClearFlag_ADRDY(adc);
	LL_ADC_Enable(adc);
	while (LL_ADC_IsActiveFlag_ADRDY(adc) == 0) { /* Wait */ }
}

void adc_core_start(uint8_t adc_num, uint8_t sequence_length)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	// Set regular sequence length
	LL_ADC_REG_SetSequencerLength(adc, sequence_length - 1);

	// Go
	LL_ADC_REG_StartConversion(adc);
}

void adc_core_stop(uint8_t adc_num)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	LL_ADC_REG_StopConversion(adc);
}

void adc_core_configure_dma_mode(uint8_t adc_num, bool use_dma)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	if (use_dma == true)
	{
		LL_ADC_REG_SetDMATransfer(adc, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
	}
	else
	{
		LL_ADC_REG_SetDMATransfer(adc, LL_ADC_REG_DMA_TRANSFER_NONE);
	}
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


/**
 * ADC differential channel set-up:
 * Applies differential mode to specified channel.
 * Refer to RM 21.4.7
 */
void adc_core_set_channel_differential(uint8_t adc_num, uint8_t channel, bool enable_differential)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	uint32_t diff = (enable_differential == true) ? LL_ADC_DIFFERENTIAL_ENDED : LL_ADC_SINGLE_ENDED;
	uint32_t ll_channel = __LL_ADC_DECIMAL_NB_TO_CHANNEL(channel);

	LL_ADC_SetChannelSingleDiff(adc, ll_channel, diff);
}

void adc_core_configure_channel(uint8_t adc_num, uint8_t channel, uint8_t rank)
{
	ADC_TypeDef* adc = _get_adc_by_number(adc_num);

	uint32_t ll_channel = __LL_ADC_DECIMAL_NB_TO_CHANNEL(channel);
	uint32_t ll_rank = _adc_decimal_nb_to_rank(rank);

	// Set regular sequence
	LL_ADC_REG_SetSequencerRanks(adc, ll_rank, ll_channel);

	// Set channels sampling time

	/* 000: 2.5 ADC clock cycles
	 * 001: 6.5 ADC clock cycles
	 * 010: 12.5 ADC clock cycles
	 * 011: 24.5 ADC clock cycles
	 * 100: 47.5 ADC clock cycles
	 * 101: 92.5 ADC clock cycles
	 * 110: 247.5 ADC clock cycles
	 * 111: 640.5 ADC clock cycles
	 */
	/* Vrefint minimum sampling time : 4us
	 */
	/* Vts minimum sampling time : 5us
	 */
	/* For 0b110:
	 * Tadc_clk = 1 / 42.5 MHz = 23.5 ns
	 * Tsar = 12.5 * Tadc_clk = 293.75 ns
	 * Tsmpl = 247.5 * Tadc_clk = 5816.25 ns
	 * Tconv = Tsmpl + Tsar = 6.11 us
	 * -> Fconv up to 163.6 KSPS for 1 channel per ADC
	 * Fconv up to 27.2 KSPS with the 6 channels actally
	 * used on the ADC1
	 *
	 * For 0b001 (ok for voltage):
	 * Tadc_clk = 1 / 42.5 MHz = 23.5 ns
	 * Tsar = 12.5 * Tadc_clk = 293.75 ns
	 * Tsmpl = 6.5 * Tadc_clk = 152.75 ns
	 * Tconv = Tsmpl + Tsar = 446.4 ns
	 * -> Fconv up to 2239 KSPS for 1 channel per ADC
	 * Fconv up to 373 KSPS with the 6 channels actally
	 * used on the ADC1
	 *
	 * For 0b101 (ok for current):
	 * Tadc_clk = 1 / 42.5 MHz = 23.5 ns
	 * Tsar = 12.5 * Tadc_clk = 293.75 ns
	 * Tsmpl = 92.5 * Tadc_clk = 2173.75 ns
	 * Tconv = Tsmpl + Tsar = 2.47 µs
	 * -> Fconv up to 404 KSPS for 1 channel per ADC
	 * Fconv up to 134 KSPS for 3 channels actally
	 * used on each ADC
	 */
	LL_ADC_SetChannelSamplingTime(adc, ll_channel, LL_ADC_SAMPLINGTIME_12CYCLES_5);
}

void adc_core_init()
{
	static bool initialized = false;

	if (initialized == false)
	{
		// Enable ADCs clocks
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);

		// Wake-up ADCs
		for (int i = 1 ; i <= NUMBER_OF_ADCS ; i++)
		{
			_adc_core_wakeup(i);
		}

		// Set common clock between ADC 1 and ADC 2
		// Refer to RM 21.4.3 and 21.7.2
		LL_ADC_SetCommonClock(ADC12_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV4);
		LL_ADC_SetCommonClock(ADC345_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV4);

		// Calibrate ADCs
		for (int i = 1 ; i <= NUMBER_OF_ADCS ; i++)
		{
			_adc_core_calibrate(i);
		}

		initialized = true;
	}
}
