/*
 * Copyright (c) 2024 LAAS-CNRS
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

/**
 * @date   2024
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


// Current class header
#include "Sensors.h"

// Other modules public API
#include "SpinAPI.h"

// Current module private functions
#include "shield_channels.h"


/////
// Public functions accessible only when using Twist


int8_t SensorsAPI::enableShieldChannel(uint8_t adc_num, channel_t channel_name)
{
	shield_channels_enable_adc_channel(adc_num, channel_name);
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel_name);
	return DataAPI::enableChannel(channel_info.adc_num, channel_info.channel_num);
}

uint16_t* SensorsAPI::getRawValues(channel_t channel, uint32_t& number_of_values_acquired)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return DataAPI::getChannelRawValues(channel_info.adc_num, channel_info.channel_num, number_of_values_acquired);
}

float32_t SensorsAPI::peek(channel_t channel)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return DataAPI::peekChannel(channel_info.adc_num, channel_info.channel_num);
}

float32_t SensorsAPI::getLatest(channel_t channel, uint8_t* dataValid)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return DataAPI::getChannelLatest(channel_info.adc_num, channel_info.channel_num, dataValid);
}

float32_t SensorsAPI::convert(channel_t channel, uint16_t raw_value)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return data_conversion_convert_raw_value(channel_info.adc_num, channel_info.channel_num, raw_value);
}

void SensorsAPI::setParameters(channel_t channel, float32_t gain, float32_t offset)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	data_conversion_set_conversion_parameters_linear(channel_info.adc_num, channel_info.channel_num, gain, offset);
}

float32_t SensorsAPI::retrieveStoredParameterValue(channel_t channel, parameter_t parameter_name)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return data_conversion_get_parameter(channel_info.adc_num,channel_info.channel_num, parameter_name);

}

conversion_type_t SensorsAPI::retrieveStoredConversionType(channel_t channel)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return data_conversion_get_conversion_type(channel_info.adc_num,channel_info.channel_num);
}

int8_t SensorsAPI::retrieveParametersFromMemory(channel_t channel)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return data_conversion_retrieve_channel_parameters_from_nvs(channel_info.adc_num, channel_info.channel_num);
}

int8_t SensorsAPI::storeParametersInMemory(channel_t channel)
{
	channel_info_t channel_info = shield_channels_get_enabled_channel_info(channel);
	return data_conversion_store_channel_parameters_in_nvs(channel_info.adc_num, channel_info.channel_num);
}


#ifdef CONFIG_SHIELD_TWIST

void SensorsAPI::enableTwistDefaultChannels()
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

void SensorsAPI::setTwistChannelsUserCalibrationFactors()
{
	shield_channels_set_user_acquisition_parameters();
}

#endif
