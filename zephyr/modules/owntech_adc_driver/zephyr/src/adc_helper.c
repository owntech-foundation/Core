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
#include <string.h>
#include <stdint.h>

// Zephyr
#include <zephyr.h>


/**
 * Helper function to retreive device from name in device tree.
 */
ADC_TypeDef* _get_adc_by_name(char* name)
{
	ADC_TypeDef* adc = NULL;

	if ( strcmp(name, "ADC_1") == 0)
		adc = ADC1;
	else if ( strcmp(name, "ADC_2") == 0)
		adc = ADC2;
	else if ( strcmp(name, "ADC_3") == 0)
		adc = ADC3;
	else if ( strcmp(name, "ADC_4") == 0)
		adc = ADC4;

	return adc;
}

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

uint8_t _get_adc_number_by_name(char* name)
{
	uint8_t adc_number = 0;

	if ( strcmp(name, "ADC_1") == 0)
		adc_number = 1;
	else if ( strcmp(name, "ADC_2") == 0)
		adc_number = 2;
	else if ( strcmp(name, "ADC_3") == 0)
		adc_number = 3;
	else if ( strcmp(name, "ADC_4") == 0)
		adc_number = 4;

	return adc_number;
}

/**
 * Function to convert ADC decimal rank to litteral.
 * Sadly, there seems to be no equivalent to
 * __LL_ADC_DECIMAL_NB_TO_CHANNEL() for ranks...
 */
uint32_t adc_decimal_nb_to_rank(uint8_t decimal_rank)
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
