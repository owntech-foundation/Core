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
 * @author Thomas Walter <thomas.walter@laas.fr>
 */


// Stdlib
#include <string.h>

// Current class header
#include "DataAcquisition.h"

// OwnTech Power API
#include "HardwareConfiguration.h"
#include "scheduling_internal.h"

// Current module private functions
#include "../src/data_dispatch.h"
#include "../src/adc_channels.h"


/////
// Public object to interact with the class

DataAcquisition dataAcquisition;


/////
// Public configuration functions

int8_t DataAcquisition::configureAdcChannels(uint8_t adc_number, const char* channel_list[], uint8_t channel_count)
{
	return adc_channels_configure_adc_channels(adc_number, channel_list, channel_count);
}

void DataAcquisition::configureAdcDefaultAllMeasurements()
{
	configure_adc_default_all_measurements();
}

int8_t DataAcquisition::start(dispatch_method_t dispatch_method)
{
	if (this->is_started == true)
		return -1;

	if (dispatch_method == dispatch_method_t::at_uninterruptible_task_start)
	{
		if (scheduling_get_uninterruptible_synchronous_task_interrupt_source() == scheduling_interrupt_source_t::source_uninitialized)
		{
			return -1;
		}

		scheduling_set_data_dispatch_at_task_start(true);
	}

	for (uint8_t adc_num = 1 ; adc_num <= 4 ; adc_num++)
	{
		uint8_t channel_rank = 0;

		while (1)
		{
			const char* channel_name = adc_channels_get_channel_name(adc_num, channel_rank);

			if (channel_name != nullptr)
			{
				if (strcmp(channel_name, "V1_LOW") == 0)
				{
					_setAssignment(v1_low_assignement, adc_num, channel_rank);
				}
				else if (strcmp(channel_name, "V2_LOW") == 0)
				{
					_setAssignment(v2_low_assignement, adc_num, channel_rank);
				}
				else if (strcmp(channel_name, "V_HIGH") == 0)
				{
					_setAssignment(v_high_assignement, adc_num, channel_rank);
				}
				else if (strcmp(channel_name, "I1_LOW") == 0)
				{
					_setAssignment(i1_low_assignement, adc_num, channel_rank);
				}
				else if (strcmp(channel_name, "I2_LOW") == 0)
				{
					_setAssignment(i2_low_assignement, adc_num, channel_rank);
				}
				else if (strcmp(channel_name, "I_HIGH") == 0)
				{
					_setAssignment(i_high_assignement, adc_num, channel_rank);
				}
				else if (strcmp(channel_name, "TEMP_SENSOR") == 0)
				{
					_setAssignment(temp_sensor_assignement, adc_num, channel_rank);
				}
				else if (strcmp(channel_name, "EXTRA_MEAS") == 0)
				{
					_setAssignment(extra_sensor_assignement, adc_num, channel_rank);
				}
				else if (strcmp(channel_name, "ANALOG_COMM") == 0)
				{
					_setAssignment(analog_comm_assignement, adc_num, channel_rank);
				}

				channel_rank++;
			}
			else
			{
				break;
			}
		}
	}

	// Initialize data dispatch
	dispatch_t dispatch_type = (dispatch_method == on_dma_interrupt) ? interrupt : task;
	data_dispatch_init(dispatch_type);

	// Launch ADC conversion
	hwConfig.adcStart();

	this->is_started = true;

	return 0;
}

bool DataAcquisition::started()
{
	return this->is_started;
}


/////
// Public accessors

// Get raw values

uint16_t* DataAcquisition::getV1LowRawValues(uint32_t& number_of_values_acquired)
{
	return _getRawValues(v1_low_assignement, number_of_values_acquired);
}

uint16_t* DataAcquisition::getV2LowRawValues(uint32_t& number_of_values_acquired)
{
	return _getRawValues(v2_low_assignement, number_of_values_acquired);
}

uint16_t* DataAcquisition::getVHighRawValues(uint32_t& number_of_values_acquired)
{
	return _getRawValues(v_high_assignement, number_of_values_acquired);
}

uint16_t* DataAcquisition::getI1LowRawValues(uint32_t& number_of_values_acquired)
{
	return _getRawValues(i1_low_assignement, number_of_values_acquired);
}

uint16_t* DataAcquisition::getI2LowRawValues(uint32_t& number_of_values_acquired)
{
	return _getRawValues(i2_low_assignement, number_of_values_acquired);
}

uint16_t* DataAcquisition::getIHighRawValues(uint32_t& number_of_values_acquired)
{
	return _getRawValues(i_high_assignement, number_of_values_acquired);
}

uint16_t* DataAcquisition::getTemperatureRawValues(uint32_t& number_of_values_acquired)
{
	return _getRawValues(temp_sensor_assignement, number_of_values_acquired);
}

uint16_t* DataAcquisition::getExtraRawValues(uint32_t& number_of_values_acquired)
{
	return _getRawValues(extra_sensor_assignement, number_of_values_acquired);
}

uint16_t* DataAcquisition::getAnalogCommRawValues(uint32_t& number_of_values_acquired)
{
	return _getRawValues(analog_comm_assignement, number_of_values_acquired);
}

// Peek

float32_t DataAcquisition::peekV1Low()
{
	return _peek(v1_low_assignement);
}

float32_t DataAcquisition::peekV2Low()
{
	return _peek(v2_low_assignement);
}

float32_t DataAcquisition::peekVHigh()
{
	return _peek(v_high_assignement);
}

float32_t DataAcquisition::peekI1Low()
{
	return _peek(i1_low_assignement);
}

float32_t DataAcquisition::peekI2Low()
{
	return _peek(i2_low_assignement);
}

float32_t DataAcquisition::peekIHigh()
{
	return _peek(i_high_assignement);
}

float32_t DataAcquisition::peekTemperature()
{
	return _peek(temp_sensor_assignement);
}

float32_t DataAcquisition::peekExtra()
{
	return _peek(extra_sensor_assignement);
}

float32_t DataAcquisition::peekAnalogComm()
{
	return _peek(analog_comm_assignement);
}

// Get latest value

float32_t DataAcquisition::getV1Low(uint8_t* dataValid)
{
	return _getLatest(v1_low_assignement, dataValid);
}

float32_t DataAcquisition::getV2Low(uint8_t* dataValid)
{
	return _getLatest(v2_low_assignement, dataValid);
}

float32_t DataAcquisition::getVHigh(uint8_t* dataValid)
{
	return _getLatest(v_high_assignement, dataValid);
}

float32_t DataAcquisition::getI1Low(uint8_t* dataValid)
{
	return _getLatest(i1_low_assignement, dataValid);
}

float32_t DataAcquisition::getI2Low(uint8_t* dataValid)
{
	return _getLatest(i2_low_assignement, dataValid);
}

float32_t DataAcquisition::getIHigh(uint8_t* dataValid)
{
	return _getLatest(i_high_assignement, dataValid);
}

float32_t DataAcquisition::getTemperature(uint8_t* dataValid)
{
	return _getLatest(temp_sensor_assignement, dataValid);
}

float32_t DataAcquisition::getExtra(uint8_t* dataValid)
{
	return _getLatest(extra_sensor_assignement, dataValid);
}

float32_t DataAcquisition::getAnalogComm(uint8_t* dataValid)
{
	return _getLatest(analog_comm_assignement, dataValid);
}

// Convertion

float32_t DataAcquisition::convertV1Low(uint16_t raw_value)
{
	return _convert(v1_low_assignement, raw_value);
}

float32_t DataAcquisition::convertV2Low(uint16_t raw_value)
{
	return _convert(v2_low_assignement, raw_value);
}

float32_t DataAcquisition::convertVHigh(uint16_t raw_value)
{
	return _convert(v_high_assignement, raw_value);
}

float32_t DataAcquisition::convertI1Low(uint16_t raw_value)
{
	return _convert(i1_low_assignement, raw_value);
}

float32_t DataAcquisition::convertI2Low(uint16_t raw_value)
{
	return _convert(i2_low_assignement, raw_value);
}

float32_t DataAcquisition::convertIHigh(uint16_t raw_value)
{
	return _convert(i_high_assignement, raw_value);
}

float32_t DataAcquisition::convertTemperature(uint16_t raw_value)
{
	return _convert(temp_sensor_assignement, raw_value);
}

float32_t DataAcquisition::convertExtra(uint16_t raw_value)
{
	return _convert(extra_sensor_assignement, raw_value);
}

float32_t DataAcquisition::convertAnalogComm(uint16_t raw_value)
{
	return _convert(analog_comm_assignement, raw_value);
}

// Parameter setters

void DataAcquisition::setV1LowParameters(float32_t gain, float32_t offset)
{
	_setParameters(v1_low_assignement, gain, offset);
}

void DataAcquisition::setV2LowParameters(float32_t gain, float32_t offset)
{
	_setParameters(v2_low_assignement, gain, offset);
}

void DataAcquisition::setVHighParameters(float32_t gain, float32_t offset)
{
	_setParameters(v_high_assignement, gain, offset);
}

void DataAcquisition::setI1LowParameters(float32_t gain, float32_t offset)
{
	_setParameters(i1_low_assignement, gain, offset);
}

void DataAcquisition::setI2LowParameters(float32_t gain, float32_t offset)
{
	_setParameters(i2_low_assignement, gain, offset);
}

void DataAcquisition::setIHighParameters(float32_t gain, float32_t offset)
{
	_setParameters(i_high_assignement, gain, offset);
}

void DataAcquisition::setTemperatureParameters(float32_t gain, float32_t offset)
{
	_setParameters(temp_sensor_assignement, gain, offset);
}

void DataAcquisition::setExtraParameters(float32_t gain, float32_t offset)
{
	_setParameters(extra_sensor_assignement, gain, offset);
}

void DataAcquisition::setAnalogCommParameters(float32_t gain, float32_t offset)
{
	_setParameters(analog_comm_assignement, gain, offset);
}

// Private helper functions

void DataAcquisition::_setAssignment(channel_assignment_t& assignment, uint8_t adc_number, uint8_t channel_rank)
{
	assignment.adc_number   = adc_number;
	assignment.channel_rank = channel_rank;
}

float32_t DataAcquisition::_getLatest(channel_assignment_t assignment, uint8_t* dataValid)
{
	if (this->is_started == false)
	{
		if (dataValid != nullptr)
		{
			*dataValid = DATA_IS_MISSING;
		}
		return NO_VALUE;
	}

	uint32_t data_count;
	uint16_t* buffer = data_dispatch_get_acquired_values(assignment.adc_number, assignment.channel_rank, data_count);

	if (data_count > 0)
	{
		uint16_t raw_value = buffer[data_count - 1];
		if (dataValid != nullptr)
		{
			*dataValid = DATA_IS_OK;
		}
		return assignment.convert(raw_value);
	}
	else
	{
		uint16_t rawValue = data_dispatch_peek_acquired_value(assignment.adc_number, assignment.channel_rank);

		float32_t peekValue;
		if (rawValue != PEEK_NO_VALUE)
		{
			peekValue = assignment.convert(rawValue);
		}
		else
		{
			peekValue = NO_VALUE;
		}

		if (dataValid != nullptr)
		{
			if (peekValue != NO_VALUE)
			{
				*dataValid = DATA_IS_OLD;
			}
			else
			{
				*dataValid = DATA_IS_MISSING;
			}
		}
		return peekValue;
	}
}

uint16_t* DataAcquisition::_getRawValues(channel_assignment_t assignment, uint32_t& number_of_values_acquired)
{
	if (this->is_started == true)
	{
		return data_dispatch_get_acquired_values(assignment.adc_number, assignment.channel_rank, number_of_values_acquired);
	}
	else
	{
		number_of_values_acquired = 0;
		return nullptr;
	}
}

float32_t DataAcquisition::_peek(channel_assignment_t assignment)
{
	if (this->is_started == true)
	{
		uint16_t rawValue = data_dispatch_peek_acquired_value(assignment.adc_number, assignment.channel_rank);
		if (rawValue != PEEK_NO_VALUE)
		{
			return assignment.convert(rawValue);
		}
		else
		{
			return NO_VALUE;
		}
	}
	else
	{
		return NO_VALUE;
	}
}

float32_t DataAcquisition::_convert(channel_assignment_t assignment, uint16_t raw_value)
{
	return assignment.convert(raw_value);
}

void DataAcquisition::_setParameters(channel_assignment_t assignment, float32_t gain, float32_t offset)
{
	assignment.set_parameters(gain, offset);
}

void DataAcquisition::setDefaultCalibrationFactors(void)
{
	set_default_acquisition_parameters();
}

void DataAcquisition::setUserCalibrationFactors(void)
{
	set_user_acquisition_parameters();
}