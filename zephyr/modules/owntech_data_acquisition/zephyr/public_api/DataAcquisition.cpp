/*
 * Copyright (c) 2022-2023 LAAS-CNRS
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
 * @author Luiz Villa <luiz.villa@laas.fr>
 */


// Stdlib
#include <string.h>

// Current class header
#include "DataAcquisition.h"

// OwnTech Power API
#include "adc.h"

// Current module private functions
#include "../adc_to_mem/data_dispatch.h"
#include "../data_conversion/data_conversion.h"


/////
// Public object to interact with the class

DataAcquisition dataAcquisition;


/////
// Public static configuration functions

void DataAcquisition::setChannnelAssignment(uint8_t adc_number, const char* channel_name, uint8_t channel_rank)
{
	if (strcmp(channel_name, "V1_LOW") == 0)
	{
		v1_low_assignement.adc_number   = adc_number;
		v1_low_assignement.channel_rank = channel_rank;
	}
	else if (strcmp(channel_name, "V2_LOW") == 0)
	{
		v2_low_assignement.adc_number   = adc_number;
		v2_low_assignement.channel_rank = channel_rank;
	}
	else if (strcmp(channel_name, "V_HIGH") == 0)
	{
		v_high_assignement.adc_number   = adc_number;
		v_high_assignement.channel_rank = channel_rank;
	}
	else if (strcmp(channel_name, "I1_LOW") == 0)
	{
		i1_low_assignement.adc_number   = adc_number;
		i1_low_assignement.channel_rank = channel_rank;
	}
	else if (strcmp(channel_name, "I2_LOW") == 0)
	{
		i2_low_assignement.adc_number   = adc_number;
		i2_low_assignement.channel_rank = channel_rank;
	}
	else if (strcmp(channel_name, "I_HIGH") == 0)
	{
		i_high_assignement.adc_number   = adc_number;
		i_high_assignement.channel_rank = channel_rank;
	}
	else if (strcmp(channel_name, "TEMP_SENSOR") == 0)
	{
		temp_sensor_assignement.adc_number   = adc_number;
		temp_sensor_assignement.channel_rank = channel_rank;
	}
	else if (strcmp(channel_name, "EXTRA_MEAS") == 0)
	{
		extra_sensor_assignement.adc_number   = adc_number;
		extra_sensor_assignement.channel_rank = channel_rank;
	}
	else if (strcmp(channel_name, "ANALOG_COMM") == 0)
	{
		analog_comm_assignement.adc_number   = adc_number;
		analog_comm_assignement.channel_rank = channel_rank;
	}
}

void DataAcquisition::start()
{
	for (uint8_t adc_num = 1 ; adc_num <= 4 ; adc_num++)
	{
		uint8_t channel_rank = 0;

		while (1)
		{
			const char* channel_name = adc_get_channel_name(adc_num, channel_rank);

			if (channel_name != NULL)
			{
				setChannnelAssignment(adc_num, channel_name, channel_rank);
				channel_rank++;
			}
			else
			{
				break;
			}
		}
	}

	// Initialize data dispatch
	data_dispatch_init();

	// Launch ADC conversion
	adc_start();

	this->is_started = true;
}

bool DataAcquisition::started()
{
	return this->is_started;
}


/////
// Public static accessors

uint16_t* DataAcquisition::getV1LowRawValues(uint32_t& number_of_values_acquired)
{
	if (this->is_started == true)
	{
		return data_dispatch_get_acquired_values(v1_low_assignement.adc_number, v1_low_assignement.channel_rank, &number_of_values_acquired);
	}
	else
	{
		number_of_values_acquired = 0;
		return nullptr;
	}
}

uint16_t* DataAcquisition::getV2LowRawValues(uint32_t& number_of_values_acquired)
{
	if (this->is_started == true)
	{
		return data_dispatch_get_acquired_values(v2_low_assignement.adc_number, v2_low_assignement.channel_rank, &number_of_values_acquired);
	}
	else
	{
		number_of_values_acquired = 0;
		return nullptr;
	}
}

uint16_t* DataAcquisition::getVHighRawValues(uint32_t& number_of_values_acquired)
{
	if (this->is_started == true)
	{
		return data_dispatch_get_acquired_values(v_high_assignement.adc_number, v_high_assignement.channel_rank, &number_of_values_acquired);
	}
	else
	{
		number_of_values_acquired = 0;
		return nullptr;
	}
}

uint16_t* DataAcquisition::getI1LowRawValues(uint32_t& number_of_values_acquired)
{
	if (this->is_started == true)
	{
		return data_dispatch_get_acquired_values(i1_low_assignement.adc_number, i1_low_assignement.channel_rank, &number_of_values_acquired);
	}
	else
	{
		number_of_values_acquired = 0;
		return nullptr;
	}
}

uint16_t* DataAcquisition::getI2LowRawValues(uint32_t& number_of_values_acquired)
{
	if (this->is_started == true)
	{
		return data_dispatch_get_acquired_values(i2_low_assignement.adc_number, i2_low_assignement.channel_rank, &number_of_values_acquired);
	}
	else
	{
		number_of_values_acquired = 0;
		return nullptr;
	}
}

uint16_t* DataAcquisition::getIHighRawValues(uint32_t& number_of_values_acquired)
{
	if (this->is_started == true)
	{
		return data_dispatch_get_acquired_values(i_high_assignement.adc_number, i_high_assignement.channel_rank, &number_of_values_acquired);
	}
	else
	{
		number_of_values_acquired = 0;
		return nullptr;
	}
}

uint16_t* DataAcquisition::getTemperatureRawValues(uint32_t& number_of_values_acquired)
{
	if (this->is_started == true)
	{
		return data_dispatch_get_acquired_values(temp_sensor_assignement.adc_number, temp_sensor_assignement.channel_rank, &number_of_values_acquired);
	}
	else
	{
		number_of_values_acquired = 0;
		return nullptr;
	}
}

uint16_t* DataAcquisition::getExtraRawValues(uint32_t& number_of_values_acquired)
{
	if (this->is_started == true)
	{
		return data_dispatch_get_acquired_values(extra_sensor_assignement.adc_number, extra_sensor_assignement.channel_rank, &number_of_values_acquired);
	}
	else
	{
		number_of_values_acquired = 0;
		return nullptr;
	}
}

uint16_t* DataAcquisition::getAnalogCommRawValues(uint32_t& number_of_values_acquired)
{
	if (this->is_started == true)
	{
		return data_dispatch_get_acquired_values(analog_comm_assignement.adc_number, analog_comm_assignement.channel_rank, &number_of_values_acquired);
	}
	else
	{
		number_of_values_acquired = 0;
		return nullptr;
	}
}


float32_t DataAcquisition::peekV1Low()
{
	if (this->is_started == true)
	{
		uint16_t rawValue = data_dispatch_peek_acquired_value(v1_low_assignement.adc_number, v1_low_assignement.channel_rank);
		return data_conversion_convert_v1_low(rawValue);
	}
	else
	{
		return 0;
	}
}

float32_t DataAcquisition::peekV2Low()
{
	if (this->is_started == true)
	{
		uint16_t rawValue = data_dispatch_peek_acquired_value(v2_low_assignement.adc_number, v2_low_assignement.channel_rank);
		return data_conversion_convert_v2_low(rawValue);
	}
	else
	{
		return 0;
	}
}

float32_t DataAcquisition::peekVHigh()
{
	if (this->is_started == true)
	{
		uint16_t rawValue = data_dispatch_peek_acquired_value(v_high_assignement.adc_number, v_high_assignement.channel_rank);
		return data_conversion_convert_v_high(rawValue);
	}
	else
	{
		return 0;
	}
}

float32_t DataAcquisition::peekI1Low()
{
	if (this->is_started == true)
	{
		uint16_t rawValue = data_dispatch_peek_acquired_value(i1_low_assignement.adc_number, i1_low_assignement.channel_rank);
		return data_conversion_convert_i1_low(rawValue);
	}
	else
	{
		return 0;
	}
}

float32_t DataAcquisition::peekI2Low()
{
	if (this->is_started == true)
	{
		uint16_t rawValue = data_dispatch_peek_acquired_value(i2_low_assignement.adc_number, i2_low_assignement.channel_rank);
		return data_conversion_convert_i2_low(rawValue);
	}
	else
	{
		return 0;
	}
}

float32_t DataAcquisition::peekIHigh()
{
	if (this->is_started == true)
	{
		uint16_t rawValue = data_dispatch_peek_acquired_value(i_high_assignement.adc_number, i_high_assignement.channel_rank);
		return data_conversion_convert_i_high(rawValue);
	}
	else
	{
		return 0;
	}
}

float32_t DataAcquisition::peekTemperature()
{
	if (this->is_started == true)
	{
		uint16_t rawValue = data_dispatch_peek_acquired_value(temp_sensor_assignement.adc_number, temp_sensor_assignement.channel_rank);
		return data_conversion_convert_temp(rawValue);
	}
	else
	{
		return 0;
	}
}

float32_t DataAcquisition::peekExtra()
{
	if (this->is_started == true)
	{
		uint16_t rawValue = data_dispatch_peek_acquired_value(extra_sensor_assignement.adc_number, extra_sensor_assignement.channel_rank);
		return data_conversion_convert_extra(rawValue);
	}
	else
	{
		return 0;
	}
}

float32_t DataAcquisition::getV1Low()
{
	if (this->is_started == true)
	{
		uint32_t data_count;
		static float32_t converted_data = -10000; // Return an impossible value if no data has been acquired yet
		uint16_t* buffer = getV1LowRawValues(data_count);

		if (data_count > 0) // If data was received it gets converted
		{
			uint16_t raw_value = buffer[data_count - 1];
			converted_data = data_conversion_convert_v1_low(raw_value);
		}

		return converted_data;
	}
	else
	{
		return -10000; // Return an impossible value if no data has been acquired yet
	}
}

float32_t DataAcquisition::getV2Low()
{
	if (this->is_started == true)
	{
		uint32_t data_count;
		static float32_t converted_data = -10000; // Return an impossible value if no data has been acquired yet
		uint16_t* buffer = getV2LowRawValues(data_count);

		if (data_count > 0) // If data was received it gets converted
		{
			uint16_t raw_value = buffer[data_count - 1];
			converted_data = data_conversion_convert_v2_low(raw_value);
		}

		return converted_data;
	}
	else
	{
		return -10000; // Return an impossible value if no data has been acquired yet
	}
}

float32_t DataAcquisition::getVHigh()
{
	if (this->is_started == true)
	{
		uint32_t data_count;
		static float32_t converted_data = -10000; // Return an impossible value if no data has been acquired yet
		uint16_t* buffer = getVHighRawValues(data_count);

		if (data_count > 0) // If data was received it gets converted
		{
			uint16_t raw_value = buffer[data_count - 1];
			converted_data = data_conversion_convert_v_high(raw_value);
		}

		return converted_data;
	}
	else
	{
		return -10000; // Return an impossible value if no data has been acquired yet
	}
}

float32_t DataAcquisition::getI1Low()
{
	if (this->is_started == true)
	{
		uint32_t data_count;
		static float32_t converted_data = -10000; // Return an impossible value if no data has been acquired yet
		uint16_t* buffer = getI1LowRawValues(data_count);

		if (data_count > 0) // If data was received it gets converted
		{
			uint16_t raw_value = buffer[data_count - 1];
			converted_data = data_conversion_convert_i1_low(raw_value);
		}

		return converted_data;
	}
	else
	{
		return -10000; // Return an impossible value if no data has been acquired yet
	}
}

float32_t DataAcquisition::getI2Low()
{
	if (this->is_started == true)
	{
		uint32_t data_count;
		static float32_t converted_data = -10000; // Return an impossible value if no data has been acquired yet
		uint16_t* buffer = getI2LowRawValues(data_count);

		if (data_count > 0) // If data was received it gets converted
		{
			uint16_t raw_value = buffer[data_count - 1];
			converted_data = data_conversion_convert_i2_low(raw_value);
		}

		return converted_data;
	}
	else
	{
		return -10000; // Return an impossible value if no data has been acquired yet
	}
}

float32_t DataAcquisition::getIHigh()
{
	if (this->is_started == true)
	{
		uint32_t data_count;
		static float32_t converted_data = -10000; // Return an impossible value if no data has been acquired yet
		uint16_t* buffer = getIHighRawValues(data_count);

		if (data_count > 0) // If data was received it gets converted
		{
			uint16_t raw_value = buffer[data_count - 1];
			converted_data = data_conversion_convert_i_high(raw_value);
		}

		return converted_data;
	}
	else
	{
		return -10000; // Return an impossible value if no data has been acquired yet
	}
}

float32_t DataAcquisition::getTemperature()
{
	if (this->is_started == true)
	{
		uint32_t data_count;
		static float32_t converted_data = -10000; // Return an impossible value if no data has been acquired yet
		uint16_t* buffer = getTemperatureRawValues(data_count);

		if (data_count > 0) // If data was received it gets converted
		{
			uint16_t raw_value = buffer[data_count - 1];
			converted_data = data_conversion_convert_temp(raw_value);
		}

		return converted_data;
	}
	else
	{
		return -10000; // Return an impossible value if no data has been acquired yet
	}
}

float32_t DataAcquisition::getExtra()
{
	if (this->is_started == true)
	{
		uint32_t data_count;
		static float32_t converted_data = -10000; // Return an impossible value if no data has been acquired yet
		uint16_t* buffer = getExtraRawValues(data_count);

		if (data_count > 0) // If data was received it gets converted
		{
			uint16_t raw_value = buffer[data_count - 1];
			converted_data = data_conversion_convert_extra(raw_value);
		}

		return converted_data;
	}
	else
	{
		return -10000; // Return an impossible value if no data has been acquired yet
	}
}

float32_t DataAcquisition::getAnalogComm()
{
	if (this->is_started == true)
	{
		uint32_t data_count;
		static float32_t converted_data = -10000; // Return an impossible value if no data has been acquired yet
		uint16_t* buffer = getAnalogCommRawValues(data_count);

		if (data_count > 0) // If data was received it gets converted
		{
			uint16_t raw_value = buffer[data_count - 1];
			converted_data = data_conversion_convert_analog_comm(raw_value);
		}

		return converted_data;
	}
	else
	{
		return -10000; // Return an impossible value if no data has been acquired yet
	}
}

float32_t DataAcquisition::convertV1Low(uint16_t raw_value)
{
	return data_conversion_convert_v1_low(raw_value);
}

float32_t DataAcquisition::convertV2Low(uint16_t raw_value)
{
	return data_conversion_convert_v2_low(raw_value);
}

float32_t DataAcquisition::convertVHigh(uint16_t raw_value)
{
	return data_conversion_convert_v_high(raw_value);
}

float32_t DataAcquisition::convertI1Low(uint16_t raw_value)
{
	return data_conversion_convert_i1_low(raw_value);
}

float32_t DataAcquisition::convertI2Low(uint16_t raw_value)
{
	return data_conversion_convert_i2_low(raw_value);
}

float32_t DataAcquisition::convertIHigh(uint16_t raw_value)
{
	return data_conversion_convert_i_high(raw_value);
}

float32_t DataAcquisition::convertTemperature(uint16_t raw_value)
{
	return data_conversion_convert_temp(raw_value);
}

float32_t DataAcquisition::convertExtra(uint16_t raw_value)
{
	return data_conversion_convert_extra(raw_value);
}

float32_t DataAcquisition::convertAnalogComm(uint16_t raw_value)
{
	return data_conversion_convert_analog_comm(raw_value);
}


void DataAcquisition::setV1LowParameters(float32_t gain, float32_t offset)
{
	data_conversion_set_v1_low_parameters(gain, offset);
}

void DataAcquisition::setV2LowParameters(float32_t gain, float32_t offset)
{
	data_conversion_set_v2_low_parameters(gain, offset);
}

void DataAcquisition::setVHighParameters(float32_t gain, float32_t offset)
{
	data_conversion_set_v_high_parameters(gain, offset);
}

void DataAcquisition::setI1LowParameters(float32_t gain, float32_t offset)
{
	data_conversion_set_i1_low_parameters(gain, offset);
}

void DataAcquisition::setI2LowParameters(float32_t gain, float32_t offset)
{
	data_conversion_set_i2_low_parameters(gain, offset);
}

void DataAcquisition::setIHighParameters(float32_t gain, float32_t offset)
{
	data_conversion_set_i_high_parameters(gain, offset);
}

void DataAcquisition::setTemperatureParameters(float32_t gain, float32_t offset)
{
	data_conversion_set_temp_parameters(gain, offset);
}

void DataAcquisition::setExtraParameters(float32_t gain, float32_t offset)
{
	data_conversion_set_extra_parameters(gain, offset);
}

void DataAcquisition::setAnalogCommParameters(float32_t gain, float32_t offset)
{
	data_conversion_set_analog_comm_parameters(gain, offset);
}
