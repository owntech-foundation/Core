/*
 * Copyright (c) 2021-present LAAS-CNRS
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
 * SPDX-License-Identifier: LGPL-2.1
 */

/*
 * @date   2023
 * @author Antoine Boche <antoine.boche@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Thomas Walter <thomas.walter@laas.fr>
 */

/**
 *  Includes
 */

/* OwnTech API */
#include "../DataAPI.h"

/* Current file header */
#include "data_conversion.h"
#include "nvs_storage.h"


/**
 *  Local Variables
 */

static const uint8_t max_parameters_count = 2;

static conversion_type_t conversion_types[ADC_COUNT][CHANNELS_PER_ADC];
static float32_t* conversion_parameters[ADC_COUNT][CHANNELS_PER_ADC];

/* voltage reference from ADC */
#define VREF 2.048f
/* ADC resolution */
#define QUANTUM_MAX 4096.0f
/* Input voltage in the voltage divider */
#define Vin_divider 3.3f

/**
 * Private functions
 */

static uint8_t _data_conversion_get_parameters_count(conversion_type_t type)
{
	uint8_t parameters_count = 0;

	switch(type)
	{
		case conversion_linear:
			/* Param 0 = gain */
			/* Param 1 = offset */
			parameters_count = 2;
			break;
		case conversion_therm:
			/* Param 0 = R0 */
			/* Param 1 = B */
			/* Param 2 = RDIV */
			/* Param 3 = T0 */
			parameters_count = 4;
		case no_channel_error:
			break;
	}

	return parameters_count;
}

/* Public functions */

void data_conversion_init()
{
	/* Make sure all channels have conversion parameters */
	for (int adc_index = 0 ; adc_index < ADC_COUNT ; adc_index++)
	{
		for (int channel_index = 0 ;
			 channel_index < CHANNELS_PER_ADC ;
			 channel_index++)
		{
			if (conversion_parameters[adc_index][channel_index] == nullptr)
			{
				uint8_t param_count =
					_data_conversion_get_parameters_count(
						conversion_types[adc_index][channel_index]
					);

				conversion_parameters[adc_index][channel_index] =
					(float32_t*)k_malloc(param_count*sizeof(float32_t));

				switch(conversion_types[adc_index][channel_index])
				{
					case conversion_linear:
						/* For linear conversion, set default gain to 1
						 * and default offset to 0 */
						conversion_parameters[adc_index][channel_index][0]= 1;
						conversion_parameters[adc_index][channel_index][1]= 0;
						break;
					case conversion_therm:
						/* For therm conversion, set all parameters to 1
						 * by default */
						conversion_parameters[adc_index][channel_index][0]= 1;
						conversion_parameters[adc_index][channel_index][1]= 1;
						conversion_parameters[adc_index][channel_index][2]= 1;
						conversion_parameters[adc_index][channel_index][3]= 1;
						break;
					case no_channel_error:
						break;
				}
			}
		}
	}
}

float32_t data_conversion_convert_raw_value(uint8_t adc_num,
											uint8_t channel_num,
											uint16_t raw_value)
{
	uint8_t adc_index     = adc_num - 1;
	uint8_t channel_index = channel_num - 1;

	switch(conversion_types[adc_index][channel_index])
	{
		case conversion_linear:
			return (raw_value *
					conversion_parameters[adc_index][channel_index][0]) +
					(conversion_parameters[adc_index][channel_index][1]);
			break;
		case conversion_therm:
		{
			/* Retrieves the parameters for the thermo resistor */
			float32_t local_r0 =
					conversion_parameters[adc_index][channel_index][0];

			float32_t local_b =
					conversion_parameters[adc_index][channel_index][1];

			float32_t local_rdiv =
					conversion_parameters[adc_index][channel_index][2];

			float32_t local_t0 =
					conversion_parameters[adc_index][channel_index][3];

			/* converts raw values into voltage */
			float32_t V_adc = (raw_value/QUANTUM_MAX)*VREF;

			/* uses a bridge divider equation
			 * to estimate the sensor resistance */
			float32_t R_t = (V_adc/(Vin_divider - V_adc)) * local_rdiv;

			/* original equation R = exp(B*(1/T - 1/T0)) */
			float32_t T =
				local_t0 /
				( 1 + (float32_t)log(R_t/local_r0) * (local_t0/local_b));

			/* returns value in degree Celsius */
			return (T - 273.15f);
			break;
		}			break;
		case no_channel_error:
			return ERROR_CHANNEL_NOT_FOUND;
			break;
		default:
			return ERROR_CHANNEL_NOT_FOUND;
			break;
	}
}

void data_conversion_set_conversion_parameters_linear(
		uint8_t adc_num,
		uint8_t channel_num,
		float32_t gain,
		float32_t offset)
{
	uint8_t adc_index     = adc_num - 1;
	uint8_t channel_index = channel_num - 1;

	conversion_types[adc_index][channel_index] = conversion_linear;
	if (conversion_parameters[adc_index][channel_index] != nullptr)
	{
		k_free(conversion_parameters[adc_index][channel_index]);
	}

	conversion_parameters[adc_index][channel_index] =
							(float32_t*)k_malloc(2*sizeof(float32_t));

	conversion_parameters[adc_index][channel_index][0] = gain;
	conversion_parameters[adc_index][channel_index][1] = offset;
}

void data_conversion_set_conversion_parameters_therm(
		uint8_t adc_num,
		uint8_t channel_num,
		float32_t r0,
		float32_t b,
		float32_t rdiv,
		float32_t t0)
{
	uint8_t adc_index     = adc_num - 1;
	uint8_t channel_index = channel_num - 1;

	conversion_types[adc_index][channel_index] = conversion_therm;
	if (conversion_parameters[adc_index][channel_index] != nullptr)
	{
		k_free(conversion_parameters[adc_index][channel_index]);
	}

	conversion_parameters[adc_index][channel_index] =
									(float32_t*)k_malloc(4*sizeof(float32_t));

	conversion_parameters[adc_index][channel_index][0] = r0;
	conversion_parameters[adc_index][channel_index][1] = b;
	conversion_parameters[adc_index][channel_index][2] = rdiv;
	conversion_parameters[adc_index][channel_index][3] = t0;
}

conversion_type_t data_conversion_get_conversion_type(
					uint8_t adc_num,
					uint8_t channel_num)
{
	uint8_t adc_index     = adc_num - 1;
	uint8_t channel_index = channel_num - 1;

	return conversion_types[adc_index][channel_index];
}

float32_t data_conversion_get_parameter(
			uint8_t adc_num,
			uint8_t channel_num,
			uint8_t parameter_num)
{
	uint8_t adc_index       = adc_num - 1;
	uint8_t channel_index   = channel_num - 1;
	uint8_t parameter_index = parameter_num - 1;

	if (conversion_parameters[adc_index][channel_index] != nullptr)
	{
		uint8_t param_count =
					_data_conversion_get_parameters_count(
						conversion_types[adc_index][channel_index]
					);

		if (parameter_index < param_count)
		{
			return conversion_parameters[adc_index][channel_index][parameter_index];
		}
	}

	/* Default case: parameter was not found */
	return 0;
}

int8_t data_conversion_store_channel_parameters_in_nvs(uint8_t adc_num,
													   uint8_t channel_num)
{
	/**
	 * Handle non volatile memory used to store ADC parameters
	 * Flash partition reserved to user data storage is 4kB long
	 * We have to mount the file system then we extract one
	 * memory page that is 4kB long that contains the data
	 * and write the relevant addresses to store ADC parameters.
	 */

	uint8_t adc_index     = adc_num - 1;
	uint8_t channel_index = channel_num - 1;

	/**
	 * The data structure is as follows:
	 * - 1 byte indicating the channel descriptor string size
	 * - The channel descriptor string (should be max. 23 bytes in current version)
	 * - 1 byte indicating ADC number
	 * - 1 byte indicating channel number
	 * - 1 byte indicating number of conversion parameters
	 * - Array of conversion parameters, each using 4 bytes.
	 */


	uint8_t parameters_count =
				_data_conversion_get_parameters_count(
					conversion_types[adc_index][channel_index]
				);

	uint8_t* buffer =
			(uint8_t*)k_malloc(1 + 23 + 1 + 1 + 1 + 4*parameters_count);

	snprintk((char*)(&buffer[1]), 23, "Spin_ADC_%u_Channel_%u",
			 adc_num,
			 channel_num);

	uint8_t string_len = strlen((char*)(&buffer[1]));

	buffer[0] = string_len;
	/* Buffer [1 -> string_len] contains channel name (\0 will be overwritten) */
	buffer[string_len + 1] = adc_num;
	buffer[string_len + 2] = channel_num;
	buffer[string_len + 3] = conversion_types[adc_index][channel_index];
	for (int i = 0 ; i < parameters_count ; i++)
	{
		*((float32_t*)&buffer[string_len + 4 + 4*i]) =
							conversion_parameters[adc_index][channel_index][i];
	}

	uint16_t channel_ID = ADC_CALIBRATION | (adc_num&0x0F) << 4 | (channel_num&0x0F);

	int ns = nvs_storage_store_data(
				channel_ID, buffer,
				1 + string_len + 1 + 1 + 1 + 4*parameters_count
			);

	k_free(buffer);

	if (ns < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int8_t data_conversion_retrieve_channel_parameters_from_nvs(uint8_t adc_num,
															uint8_t channel_num)
{
	uint8_t adc_index     = adc_num - 1;
	uint8_t channel_index = channel_num - 1;

	/* Check that parameters currently stored in NVS are from the same version */
	uint16_t current_stored_version = nvs_storage_get_version_in_nvs();
	if (current_stored_version == 0)
	{
		return -1;
	}
	else if (current_stored_version != nvs_storage_get_current_version())
	{
		return -2;
	}

	uint16_t channel_ID =
				ADC_CALIBRATION | (adc_num&0x0F) << 4 | (channel_num&0x0F);

	int buffer_size = 1 + 23 + 1 + 1 + 1 + 4*max_parameters_count;
	uint8_t* buffer = (uint8_t*)k_malloc(buffer_size);

	int read_size = nvs_storage_retrieve_data(channel_ID, buffer, buffer_size);

	int ret = 0;
	if (read_size > 0)
	{
		uint8_t string_len = buffer[0];

		/* Check that all required values match */
		if (adc_num != buffer[string_len + 1])
		{
			ret = -3;
		}
		else if (channel_num != buffer[string_len + 2])
		{
			ret = -3;
		}
		else
		{
			conversion_type_t conversion_type =
								(conversion_type_t)buffer[string_len + 3];

			uint8_t parameters_count =
					_data_conversion_get_parameters_count(conversion_type);

			conversion_types[adc_index][channel_index] = conversion_type;

			if (conversion_parameters[adc_index][channel_index] != nullptr)
			{
				k_free(conversion_parameters[adc_index][channel_index]);
			}
			conversion_parameters[adc_index][channel_index] =
				(float32_t*)k_malloc(parameters_count*sizeof(float32_t));

			for (int i = 0 ; i < parameters_count ; i++)
			{
				conversion_parameters[adc_index][channel_index][i] =
								*((float32_t*)&buffer[string_len + 4 + 4*i]);
			}
		}
	}
	else
	{
		ret = -4;
	}

	k_free(buffer);
	return ret;
}
