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
 * @author Antoine Boche <antoine.boche@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Thomas Walter <thomas.walter@laas.fr>
 */

/////
// Include

// OwnTech API
#include "DataAcquisition.h"

// Current file header
#include "data_conversion.h"


/////
// Local variables

static float32_t gains[ADC_COUNT][CHANNELS_PER_ADC]   = {0};
static float32_t offsets[ADC_COUNT][CHANNELS_PER_ADC] = {0};


/////
// Public Functions

void data_conversion_init()
{
	for (int adc_index = 0 ; adc_index < ADC_COUNT ; adc_index++)
	{
		for (int channel_index = 0 ; channel_index < CHANNELS_PER_ADC ; channel_index++)
		{
			if (gains[adc_index][channel_index] == 0)
			{
				// Some channels may already have a default gain value
				// from device tree. Only initialize those which don't.
				gains[adc_index][channel_index] = 1;
			}
		}
	}
}

float32_t data_conversion_convert_raw_value(uint8_t adc_num, uint8_t channel_num, uint16_t raw_value)
{
	return (raw_value*gains[adc_num][channel_num]) + offsets[adc_num][channel_num];
}

void data_conversion_set_conversion_parameters(uint8_t adc_num, uint8_t channel_num, float32_t gain, float32_t offset)
{
	gains[adc_num][channel_num]   = gain;
	offsets[adc_num][channel_num] = offset;
}
