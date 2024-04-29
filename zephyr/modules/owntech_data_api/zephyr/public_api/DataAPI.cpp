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
#include "DataAPI.h"

// OwnTech Power API
#include "SpinAPI.h"

// Current module private functions
#include "../src/data_dispatch.h"
#ifdef CONFIG_SHIELD_TWIST
#include "../src/shield_channels.h"
#endif


/////
// Public object to interact with the class

DataAPI data;


/////
// Public functions accessible only when using Twist

#ifdef CONFIG_SHIELD_TWIST

int8_t DataAPI::enableShieldChannel(uint8_t adc_num, channel_t channel_name)
{
	shield_channels_enable_adc_channel(adc_num, channel_name);
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel_name);
	return this->enableChannel(channel_info.adc_num, channel_info.channel_num);
}

void DataAPI::enableTwistDefaultChannels()
{
	spin.adc.configureTriggerSource(1, hrtim_ev1);
	spin.adc.configureTriggerSource(2, hrtim_ev3);
	spin.adc.configureTriggerSource(3, software);
	spin.adc.configureTriggerSource(4, software);
	spin.adc.configureTriggerSource(5, software);

	spin.adc.configureDiscontinuousMode(1,1);
	spin.adc.configureDiscontinuousMode(2, 1);

	this->enableShieldChannel(1, I1_LOW);
	this->enableShieldChannel(1, V1_LOW);
	this->enableShieldChannel(1, V_HIGH);

	this->enableShieldChannel(2, I2_LOW);
	this->enableShieldChannel(2, V2_LOW);
	this->enableShieldChannel(2, I_HIGH);
}

uint16_t* DataAPI::getRawValues(channel_t channel, uint32_t& number_of_values_acquired)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return this->getChannelRawValues(channel_info.adc_num, channel_info.channel_num, number_of_values_acquired);
}

float32_t DataAPI::peek(channel_t channel)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return this->peekChannel(channel_info.adc_num, channel_info.channel_num);
}

float32_t DataAPI::getLatest(channel_t channel, uint8_t* dataValid)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return this->getChannelLatest(channel_info.adc_num, channel_info.channel_num, dataValid);
}

float32_t DataAPI::convert(channel_t channel, uint16_t raw_value)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return data_conversion_convert_raw_value(channel_info.adc_num, channel_info.channel_num, raw_value);
}

void DataAPI::setParameters(channel_t channel, float32_t gain, float32_t offset)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	data_conversion_set_conversion_parameters_linear(channel_info.adc_num, channel_info.channel_num, gain, offset);
}

float32_t DataAPI::retrieveStoredParameterValue(channel_t channel, parameter_t parameter_name)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return data_conversion_get_parameter(channel_info.adc_num,channel_info.channel_num, parameter_name);

}

conversion_type_t DataAPI::retrieveStoredConversionType(channel_t channel)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return data_conversion_get_conversion_type(channel_info.adc_num,channel_info.channel_num);
}

int8_t DataAPI::retrieveParametersFromMemory(channel_t channel)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return data_conversion_retrieve_channel_parameters_from_nvs(channel_info.adc_num, channel_info.channel_num);
}

int8_t DataAPI::storeParametersInMemory(channel_t channel)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return data_conversion_store_channel_parameters_in_nvs(channel_info.adc_num, channel_info.channel_num);
}

void DataAPI::setTwistChannelsUserCalibrationFactors()
{
	shield_channels_set_user_acquisition_parameters();
}


#endif // CONFIG_SHIELD_TWIST


/////
// Public functions

int8_t DataAPI::enableAcquisition(uint8_t adc_num, uint8_t pin_num)
{
	uint8_t channel_num = this->getChannelNumber(adc_num, pin_num);
	if (channel_num == 0)
		return -1;

	return this->enableChannel(adc_num, channel_num);
}

int8_t DataAPI::start()
{
	if (this->is_started == true)
		return -1;

	// Initialize conversion
	data_conversion_init();

	// Initialize data dispatch
	switch (this->dispatch_method)
	{
		case DispatchMethod_t::on_dma_interrupt:
			// Dispatch is handled automatically by Data Dispatch on interrupt
			data_dispatch_init(interrupt, 0);
			break;
		case DispatchMethod_t::externally_triggered:
			// Dispatch is triggered by an external call
			if (this->repetition_count_between_dispatches == 0)
				return -1;

			data_dispatch_init(task, this->repetition_count_between_dispatches);
	}

	// Launch ADC conversion
	spin.adc.startAllAdcs();

	this->is_started = true;

	return 0;
}

bool DataAPI::started()
{
	return this->is_started;
}

void DataAPI::setDispatchMethod(DispatchMethod_t dispatch_method)
{
	this->dispatch_method = dispatch_method;
}

DispatchMethod_t DataAPI::getDispatchMethod()
{
	return this->dispatch_method;
}

void DataAPI::setRepetitionsBetweenDispatches(uint32_t repetition)
{
	this->repetition_count_between_dispatches = repetition;
}

void DataAPI::triggerAcquisition(uint8_t adc_num)
{
	uint8_t enabled_channels = spin.adc.getEnabledChannelsCount(adc_num);
	spin.adc.triggerSoftwareConversion(adc_num, enabled_channels);

}

uint16_t* DataAPI::getRawValues(uint8_t adc_num, uint8_t pin_num, uint32_t& number_of_values_acquired)
{
	uint8_t channel_num = this->getChannelNumber(adc_num, pin_num);
	if (channel_num == 0)
	{
		number_of_values_acquired = 0;
		return nullptr;
	}

	return this->getChannelRawValues(adc_num, channel_num, number_of_values_acquired);
}

float32_t DataAPI::peek(uint8_t adc_num, uint8_t pin_num)
{
	uint8_t channel_num = this->getChannelNumber(adc_num, pin_num);
	if (channel_num == 0)
	{
		return NO_VALUE;
	}

	return this->peekChannel(adc_num, channel_num);
}

float32_t DataAPI::getLatest(uint8_t adc_num, uint8_t pin_num, uint8_t* dataValid)
{
	uint8_t channel_num = this->getChannelNumber(adc_num, pin_num);
	if (channel_num == 0)
	{
		if (dataValid != nullptr)
		{
			*dataValid = DATA_IS_MISSING;
		}
		return NO_VALUE;
	}

	return this->getChannelLatest(adc_num, channel_num, dataValid);
}

float32_t DataAPI::convert(uint8_t adc_num, uint8_t pin_num, uint16_t raw_value)
{
	uint8_t channel_num = this->getChannelNumber(adc_num, pin_num);
	if (channel_num == 0)
	{
		return ERROR_CHANNEL_NOT_FOUND;
	}

	return data_conversion_convert_raw_value(adc_num, channel_num, raw_value);
}

void DataAPI::setParameters(uint8_t adc_num, uint8_t pin_num, float32_t gain, float32_t offset)
{
	uint8_t channel_num = this->getChannelNumber(adc_num, pin_num);
	if (channel_num == 0)
	{
		return;
	}

	data_conversion_set_conversion_parameters_linear(adc_num, channel_num, gain, offset);
}


int8_t DataAPI::storeParametersInMemory(uint8_t adc_num, uint8_t pin_num)
{
	uint8_t channel_num = this->getChannelNumber(adc_num, pin_num);
	if (channel_num == 0)
	{
		return ERROR_CHANNEL_NOT_FOUND;
	}

	return data_conversion_store_channel_parameters_in_nvs(adc_num, channel_num);
}


int8_t DataAPI::retrieveParametersFromMemory(uint8_t adc_num, uint8_t pin_num)
{
	uint8_t channel_num = this->getChannelNumber(adc_num, pin_num);
	if (channel_num == 0)
	{
		return ERROR_CHANNEL_NOT_FOUND;
	}

	return data_conversion_retrieve_channel_parameters_from_nvs(adc_num, channel_num);
}

float32_t DataAPI::retrieveStoredParameterValue(uint8_t adc_num, uint8_t pin_num, parameter_t parameter_name)
{
	uint8_t channel_num = this->getChannelNumber(adc_num, pin_num);
	if (channel_num == 0)
	{
		return ERROR_CHANNEL_NOT_FOUND;
	}

	return data_conversion_get_parameter(adc_num,channel_num, parameter_name);

}

conversion_type_t DataAPI::retrieveStoredConversionType(uint8_t adc_num, uint8_t pin_num)
{
	uint8_t channel_num = this->getChannelNumber(adc_num, pin_num);
	if (channel_num == 0)
	{
		return no_channel_error;
	}

	return data_conversion_get_conversion_type(adc_num,channel_num);
}




/////
// Private functions

int8_t DataAPI::enableChannel(uint8_t adc_num, uint8_t channel_num)
{
	if (this->is_started == true)
		return -1;

	if ( (adc_num == 0) || (adc_num > ADC_COUNT) )
		return -1;

	if ( (channel_num == 0) || (channel_num > CHANNELS_PER_ADC) )
		return -1;

	// Enable channel
	spin.adc.enableDma(adc_num, true);
	spin.adc.enableChannel(adc_num, channel_num);



	// Remember rank
	uint8_t adc_index = adc_num-1;
	uint8_t channel_index = channel_num-1;
	this->current_rank[adc_index]++;
	this->channels_ranks[adc_index][channel_index] = this->current_rank[adc_index];

	return 0;
}

uint16_t* DataAPI::getChannelRawValues(uint8_t adc_num, uint8_t channel_num, uint32_t& number_of_values_acquired)
{
	if (this->is_started == false)
	{
		number_of_values_acquired = 0;
		return nullptr;
	}

	uint8_t channel_rank = this->getChannelRank(adc_num, channel_num);
	if (channel_rank == 0)
	{
		number_of_values_acquired = 0;
		return nullptr;
	}

	return data_dispatch_get_acquired_values(adc_num, channel_rank, number_of_values_acquired);
}

float32_t DataAPI::peekChannel(uint8_t adc_num, uint8_t channel_num)
{
	if (this->is_started == false)
	{
		return NO_VALUE;
	}

	uint8_t channel_rank = this->getChannelRank(adc_num, channel_num);
	if (channel_rank == 0)
	{
		return NO_VALUE;
	}

	uint16_t raw_value = data_dispatch_peek_acquired_value(adc_num, channel_rank);
	if (raw_value == PEEK_NO_VALUE)
	{
		return NO_VALUE;
	}

	return data_conversion_convert_raw_value(adc_num, channel_num, raw_value);
}

float32_t DataAPI::getChannelLatest(uint8_t adc_num, uint8_t channel_num, uint8_t* dataValid)
{
	if (this->is_started == false)
	{
		if (dataValid != nullptr)
		{
			*dataValid = DATA_IS_MISSING;
		}
		return NO_VALUE;
	}

	uint8_t channel_rank = this->getChannelRank(adc_num, channel_num);
	if (channel_rank == 0)
	{
		if (dataValid != nullptr)
		{
			*dataValid = DATA_IS_MISSING;
		}
		return NO_VALUE;
	}

	uint32_t data_count;
	uint16_t* buffer = data_dispatch_get_acquired_values(adc_num, channel_rank, data_count);

	if (data_count > 0)
	{
		uint16_t raw_value = buffer[data_count - 1];
		if (dataValid != nullptr)
		{
			*dataValid = DATA_IS_OK;
		}
		return data_conversion_convert_raw_value(adc_num, channel_num, raw_value);
	}
	else
	{
		uint16_t raw_value = data_dispatch_peek_acquired_value(adc_num, channel_rank);

		float32_t peekValue;
		if (raw_value != PEEK_NO_VALUE)
		{
			data_conversion_convert_raw_value(adc_num, channel_num, raw_value);
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

uint8_t DataAPI::getChannelRank(uint8_t adc_num, uint8_t channel_num)
{
	if ( (adc_num > ADC_COUNT) || (channel_num > CHANNELS_PER_ADC) )
		return 0;

	uint8_t adc_index = adc_num-1;
	uint8_t channel_index = channel_num-1;

	return this->channels_ranks[adc_index][channel_index];
}

uint8_t DataAPI::getChannelNumber(uint8_t adc_num, uint8_t twist_pin)
{
	switch (adc_num)
	{
	case 1:
		switch (twist_pin)
		{
			case 1:
				return 14;
				break;
			case 2:
				return 11;
				break;
			case 5:
				return 5;
				break;
			case 24:
				return 6;
				break;
			case 25:
				return 7;
				break;
			case 26:
				return 8;
				break;
			case 27:
				return 9;
				break;
			case 29:
				return 1;
				break;
			case 30:
				return 2;
				break;
			case 31:
				return 5;
				break;
			case 37:
				return 12;
				break;
			case 50:
				return 3;
				break;
			case 51:
				return 4;
				break;
			default:
				return 0;
				break;
		}
		break;
	case 2:
		switch (twist_pin)
		{
			case 1:
				return 14;
				break;
			case 6:
				return 15;
				break;
			case 24:
				return 6;
				break;
			case 25:
				return 7;
				break;
			case 26:
				return 8;
				break;
			case 27:
				return 9;
				break;
			case 29:
				return 1;
				break;
			case 30:
				return 2;
				break;
			case 32:
				return 13;
				break;
			case 34:
				return 3;
				break;
			case 35:
				return 5;
				break;
			case 42:
				return 12;
				break;
			case 43:
				return 11;
				break;
			case 44:
				return 4;
				break;
			case 45:
				return 17;
				break;
			default:
				return 0;
				break;
		}
		break;
	case 3:
		switch (twist_pin)
		{
			case 4:
				return 5;
				break;
			case 31:
				return 12;
				break;
			case 37:
				return 1;
				break;
			default:
				return 0;
				break;
		}
		break;
	case 4:
		switch (twist_pin)
		{
			case 2:
				return 3;
				break;
			case 5:
				return 4;
				break;
			case 6:
				return 5;
				break;
			default:
				return 0;
				break;
		}
		break;
	case 5:
		switch (twist_pin)
		{
			case 12:
				return 1;
				break;
			case 14:
				return 2;
				break;
			default:
				return 0;
				break;
		}
		break;
	default:
		return 0;
		break;
	}
}
