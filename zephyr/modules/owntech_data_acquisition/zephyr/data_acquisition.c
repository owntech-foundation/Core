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

// OwnTech Power API
#include "dma/dma.h"
#include "adc/adc.h"
#include "data_dispatch/data_dispatch.h"

#include "data_acquisition.h"


/////
// Local types

typedef struct
{
	uint8_t adc_number;
	uint8_t channel_rank;
} channel_assignment_t;


/////
// Local variables

static uint8_t data_acquisition_initialized = 0;
static uint8_t data_acquisition_started = 0;

static channel_assignment_t v1_low_assignement = {0};
static channel_assignment_t v2_low_assignement = {0};
static channel_assignment_t v_high_assignement = {0};
static channel_assignment_t i1_low_assignement = {0};
static channel_assignment_t i2_low_assignement = {0};
static channel_assignment_t i_high_assignement = {0};
static channel_assignment_t temp_sensor_assignement = {0};


/////
// Public API

int8_t data_acquisition_init()
{
	if (data_acquisition_initialized == 0)
	{
		adc_init();
		data_acquisition_initialized = 1;
		return 0;
	}
	else
	{
		return EALREADYINIT;
	}
}

int8_t data_acquisition_set_adc12_dual_mode(uint8_t dual_mode)
{
	if ( (data_acquisition_initialized == 1) && (data_acquisition_started == 0) )
	{
		adc_set_dual_mode(dual_mode);
		return 0;
	}
	else if (data_acquisition_initialized == 0)
	{
		return EUNINITIALIZED;
	}
	else
	{
		return EALREADYSTARTED;
	}
}

int8_t data_acquisition_configure_adc_channels(uint8_t adc_number, char* channel_list[], uint8_t channel_count)
{
	if ( (data_acquisition_initialized == 1) && (data_acquisition_started == 0) )
	{
		int8_t result = adc_configure_adc_channels(adc_number, channel_list, channel_count);
		if (result == 0)
		{
			for (int i = 0 ; i < channel_count ; i++)
			{
				if (strcmp(channel_list[i], "V1_LOW") == 0)
				{
					v1_low_assignement.adc_number = adc_number;
					v1_low_assignement.channel_rank = i;
				}
				else if (strcmp(channel_list[i], "V2_LOW") == 0)
				{
					v2_low_assignement.adc_number = adc_number;
					v2_low_assignement.channel_rank = i;
				}
				else if (strcmp(channel_list[i], "V_HIGH") == 0)
				{
					v_high_assignement.adc_number = adc_number;
					v_high_assignement.channel_rank = i;
				}
				else if (strcmp(channel_list[i], "I1_LOW") == 0)
				{
					i1_low_assignement.adc_number = adc_number;
					i1_low_assignement.channel_rank = i;
				}
				else if (strcmp(channel_list[i], "I2_LOW") == 0)
				{
					i2_low_assignement.adc_number = adc_number;
					i2_low_assignement.channel_rank = i;
				}
				else if (strcmp(channel_list[i], "I_HIGH") == 0)
				{
					i_high_assignement.adc_number = adc_number;
					i_high_assignement.channel_rank = i;
				}
				else if (strcmp(channel_list[i], "TEMP_SENSOR") == 0)
				{
					temp_sensor_assignement.adc_number = adc_number;
					temp_sensor_assignement.channel_rank = i;
				}
			}
		}
		return result;
	}
	else if (data_acquisition_initialized == 0)
	{
		return EUNINITIALIZED;
	}
	else
	{
		return EALREADYSTARTED;
	}
}

int8_t data_acquisition_configure_adc_trigger_source(uint8_t adc_number, uint32_t trigger_source)
{
	if ( (data_acquisition_initialized == 1) && (data_acquisition_started == 0) )
	{
		adc_configure_trigger_source(adc_number, trigger_source);
		return 0;
	}
	else if (data_acquisition_initialized == 0)
	{
		return EUNINITIALIZED;
	}
	else
	{
		return EALREADYSTARTED;
	}
}


int8_t data_acquisition_start()
{
	if ( (data_acquisition_initialized == 1) && (data_acquisition_started == 0) )
	{
		// DMAs
		dma_configure_and_start(2);

		// Initialize data dispatch
		data_dispatch_init(2);

		// Launch ADC conversion
		adc_start();

		data_acquisition_started = 1;

		return 0;
	}
	else if (data_acquisition_initialized == 0)
	{
		return EUNINITIALIZED;
	}
	else
	{
		return EALREADYSTARTED;
	}
}

char* data_acquisition_get_channel_name(uint8_t adc_number, uint8_t channel_rank)
{
	return adc_get_channel_name(adc_number, channel_rank);
}

uint16_t* data_acquisition_get_v1_low_values(uint32_t* number_of_values_acquired)
{
	return data_dispatch_get_acquired_values(v1_low_assignement.adc_number, v1_low_assignement.channel_rank, number_of_values_acquired);
}

uint16_t* data_acquisition_get_v2_low_values(uint32_t* number_of_values_acquired)
{
	return data_dispatch_get_acquired_values(v2_low_assignement.adc_number, v2_low_assignement.channel_rank, number_of_values_acquired);
}

uint16_t* data_acquisition_get_v_high_values(uint32_t* number_of_values_acquired)
{
	return data_dispatch_get_acquired_values(v_high_assignement.adc_number, v_high_assignement.channel_rank, number_of_values_acquired);
}

uint16_t* data_acquisition_get_i1_low_values(uint32_t* number_of_values_acquired)
{
	return data_dispatch_get_acquired_values(i1_low_assignement.adc_number, i1_low_assignement.channel_rank, number_of_values_acquired);
}

uint16_t* data_acquisition_get_i2_low_values(uint32_t* number_of_values_acquired)
{
	return data_dispatch_get_acquired_values(i2_low_assignement.adc_number, i2_low_assignement.channel_rank, number_of_values_acquired);
}

uint16_t* data_acquisition_get_i_high_values(uint32_t* number_of_values_acquired)
{
	return data_dispatch_get_acquired_values(i_high_assignement.adc_number, i_high_assignement.channel_rank, number_of_values_acquired);
}

uint16_t* data_acquisition_get_temp_sensor_values(uint32_t* number_of_values_acquired)
{
	return data_dispatch_get_acquired_values(temp_sensor_assignement.adc_number, temp_sensor_assignement.channel_rank, number_of_values_acquired);
}
