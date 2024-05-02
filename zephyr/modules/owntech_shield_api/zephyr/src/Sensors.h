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


#ifndef SENSORS_H_
#define SENSORS_H_


// Stdlib
#include <stdint.h>

// Zephyr
#include <zephyr/kernel.h>

// ARM CMSIS library
#include <arm_math.h>

// Other modules public API
#include "DataAPI.h"


/////
// Device-tree related macro

#define SENSOR_TOKEN(node_id) DT_STRING_TOKEN(node_id, sensor_name),


/////
// Type definitions

typedef enum
{
	UNDEFINED_CHANNEL = 0,
	DT_FOREACH_STATUS_OKAY(shield_sensors, SENSOR_TOKEN)
} channel_t;


/////
// Static class definition

class SensorsAPI
{
public:

	/**
	 * @brief This function is used to enable a channel on a given ADC using
	 *        its name on a shield, rather than the ADC channel number. This
	 *        function requires the presence of an "adc-channels" node in the
	 *        shield device-tree.
	 *
	 * @note  This function must be called *before* ADC is started.
	 *
	 * @param adc_number Number of the ADC on which channel is to be enabled.
	 * @param channel_name Name of the channel using enumeration channel_t.
	 *
	 * @return 0 if channel was correctly enabled, -1 if there was an error.
	 */
	int8_t enableShieldChannel(uint8_t adc_num, channel_t channel_name);

	/**
	 * @brief Function to access the acquired data for specified channel.
	 *        This function provides a buffer in which all data that
	 *        have been acquired since last call are stored. The count
	 *        of these values is returned as an output parameter: the
	 *        user has to define a variable and pass it as the parameter
	 *        of the function. The variable will be updated with the
	 *        number of values that are available in the buffer.
	 *
	 * @note  This function can't be called before the channel is enabled
	 *        and the DataAPI module is started, either explicitly
	 *        or by starting the Uninterruptible task.
	 *
	 * @note  When calling this function, it invalidates the buffer
	 *        returned by a previous call to the same function.
	 *        However, different channels buffers are independent
	 *        from each other.
	 *
	 * @note  When using this functions, the user is responsible for data
	 *        conversion. Use matching data.convert*() function
	 *        for this purpose.
	 *
	 * @note  When using this function, DO NOT use the function to get the
	 *        latest converted value for the same channel as this function
	 *        will clear the buffer and disregard all values but the latest.
	 *
	 * @param channel Name of the shield channel from which to obtain values.
	 * @param number_of_values_acquired Pass an uint32_t variable.
	 *        This variable will be updated with the number of values that
	 *        are present in the returned buffer.
	 *
	 * @return Pointer to a buffer in which the acquired values are stored.
	 *         If number_of_values_acquired is 0, do not try to access the
	 *         buffer as it may be nullptr.
	 */
	uint16_t* getRawValues(channel_t channel, uint32_t& number_of_values_acquired);

	/**
	 * @brief Function to access the latest value available from the channel,
	 *        expressed in the relevant unit for the data: Volts, Amperes, or
	 *        Degree Celcius. This function will not touch anything in the
	 *        buffer, and thus can be called safely at any time after the
	 *        module has been started.
	 *
	 * @note  This function can't be called before the channel is enabled
	 *        and the DataAPI module is started, either explicitly
	 *        or by starting the Uninterruptible task.
	 *
	 * @param channel Name of the shield channel from which to obtain value.
	 *
	 * @return Latest available value available from the given channel.
	 *         If there was no value acquired in this channel yet,
	 *         return value is NO_VALUE.
	 */
	float32_t peek(channel_t channel);

	/**
	 * @brief This function returns the latest acquired measure expressed
	 *        in the relevant unit for the channel: Volts, Amperes, or
	 *        Degree Celcius.
	 *
	 * @note  This function can't be called before the channel is enabled
	 *        and the DataAPI module is started, either explicitly
	 *        or by starting the Uninterruptible task.
	 *
	 * @note  When using this functions, you loose the ability to access raw
	 *        values using data.get*RawValues() function for the
	 *        matching channel, as data.get*() function clears the
	 *        buffer on each call.
	 *
	 * @param channel Name of the shield channel from which to obtain value.
	 * @param dataValid Pointer to an uint8_t variable. This parameter is
	 *        facultative. If this parameter is provided, it will be updated
	 *        to indicate information about data. Possible values for this
	 *        parameter will be:
	 *        - DATA_IS_OK if returned data is a newly acquired data,
	 *        - DATA_IS_OLD if returned data has already been provided before
	 *        (no new data available since latest time this function was called),
	 *        - DATA_IS_MISSING if returned data is NO_VALUE.
	 *
	 * @return Latest acquired measure for the channel.
	 *         If no value was acquired in this channel yet, return value is NO_VALUE.
	 *
	 */
	float32_t getLatest(channel_t channel, uint8_t* dataValid = nullptr);

	/**
	 * @brief Use this function to convert values obtained using matching
	 *        data.get*RawValues() function to relevant
	 *        unit for the data: Volts, Amperes, or Degree Celcius.
	 *
	 * @note  This function can't be called before the channel is enabled.
	 *
	 * @param channel Name of the shield channel from which the value originates
	 * @param raw_value Raw value obtained from which the value originates
	 *
	 * @return Converted value in the relevant unit. Returns -5000 if the channel is not active.
	 */
	float32_t convert(channel_t channel, uint16_t raw_value);

	/**
	 * @brief Use this function to tweak the conversion values for the
	 *        channel if default values are not accurate enough.
	 *
	 * @note  This function can't be called before the channel is enabled.
	 *        The DataAPI must not have been started, neither explicitly
	 *        nor by starting the Uninterruptible task.
	 *
	 * @param channel Name of the shield channel to set conversion values.
	 * @param gain Gain to be applied (multiplied) to the channel raw value.
	 * @param offset Offset to be applied (added) to the channel value
	 *        after gain has been applied.
	 */
	void setParameters(channel_t channel, float32_t gain, float32_t offset);

	/**
	 * @brief Use this function to get the current conversion parameteres for the chosen channel .
	 *
	 * @note  This function can't be called before the channel is enabled.
	 *
	 * @param channel Name of the shield channel to get a conversion parameter.
	 * @param parameter_name Paramater to be retreived: `gain` or `offset`.
	 */
	float32_t retrieveStoredParameterValue(channel_t channel, parameter_t parameter_name);

	/**
	 * @brief Use this function to get the current conversion type for the chosen channel.
	 *
	 * @note  This function can't be called before the channel is enabled.
	 *
	 * @param channel Name of the shield channel to get a conversion parameter.
	 */
	conversion_type_t retrieveStoredConversionType(channel_t channel);

	/**
	 * @brief Use this function to write the gain and offset parameters of the board to is non-volatile memory.
	 *
	 * @note  This function should be called after updating the parameters using setParameters.
	 *
	 * @param channel Name of the shield channel to save the values.
	 */
	int8_t storeParametersInMemory(channel_t channel);

	/**
	 * @brief Use this function to read the gain and offset parameters of the board to is non-volatile memory.
	 *
	 * @param channel Name of the shield channel to save the values.
	 */
	int8_t retrieveParametersFromMemory(channel_t channel);

#ifdef CONFIG_SHIELD_TWIST

	/**
	 * @brief This function is used to enable acquisition of all voltage/current
	 *        channels on the Twist shield.
	 *        Channels are attributed as follows:
	 *        ADC1: - I1_LOW      ADC2: - I2_LOW
	 *              - V1_LOW            - V2_LOW
	 *              - V_HIGH            - I_HIGH
	 *
	 * @note  This function will configure ADC 1 and 2 to be automatically
	 *        triggered by the HRTIM, so the board must be configured as
	 *        a power converted to enable HRTIM events.
	 *        All other ADCs remain software triggered, thus will only be
	 *        acquired when triggerAcquisition() is called.
	 *
	 * @note  This function must be called *before* ADC is started.
	 */
	void enableTwistDefaultChannels();

	/**
	 * @brief Retrieve stored parameters from Flash memory and configure ADC parameters
	 *
	 * @note  This function requires Console to interact with the user.
	 *        You must first call console_init() before calling this function.
	 *
	 * @note  This function can't be called before the *all* Twist channels have
	 *        been enabled (you can use enableTwistDefaultChannels() for that
	 *        purpose). The DataAPI must not have been started, neither
	 *        explicitly nor by starting the Uninterruptible task.
	 */
	void setTwistChannelsUserCalibrationFactors();
#endif

};

#endif // SENSORS_H_
