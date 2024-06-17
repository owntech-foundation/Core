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
 * SPDX-License-Identifier: LGPL-2.1
 */

/**
 * @date   2023
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Thomas Walter <thomas.walter@laas.fr>
 */


#ifndef DATAAPI_H_
#define DATAAPI_H_


// Stdlib
#include <stdint.h>

// Zephyr
#include <zephyr/kernel.h>

// ARM CMSIS library
#include <arm_math.h>

// Current module private functions
#include "../src/data_conversion.h"

#define ADC_1 1
#define ADC_2 2
#define ADC_3 3
#define ADC_4 4

#define ERROR_CHANNEL_OFF -5
#define ERROR_CHANNEL_NOT_FOUND -5000

typedef enum : uint8_t
{
	gain = 1,
	offset = 2

} parameter_t;

/////
// Type definitions

enum class DispatchMethod_t
{
	on_dma_interrupt,
	externally_triggered
};


//////
// Constants definitions

static const uint8_t ADC_COUNT        = 5;
static const uint8_t CHANNELS_PER_ADC = 19;

// Define "no value" as an impossible, out of range value
const float32_t NO_VALUE = -10000;

const uint8_t DATA_IS_OK      = 0;
const uint8_t DATA_IS_OLD     = 1;
const uint8_t DATA_IS_MISSING = 2;

/////
// Static class definition

class DataAPI
{
	// Allow SensorsAPI class to access private members
	friend class SensorsAPI;

public:

	/**
	 * @brief This function is used to enable acquisition on a Spin PIN with
	 *        a given ADC.
	 *
	 * @note  Not any pin can be used for acquisiton: the pin must be linked
	 *        to a channel of the given ADC. Refer to Spin pinout image for
	 *        PIN/ADC relations.
	 *
	 * @note  This function must be called *before* ADC is started.
	 *
	 * @param adc_number Number of the ADC on which acquisition is to be done.
	 * @param pin_num Number of the Spin pin to acquire.
	 *
	 * @return 0 if acquisition was correctly enabled, -1 if there was an error.
	 */
	int8_t enableAcquisition(uint8_t adc_num, uint8_t pin_num);

	/**
	 * @brief This functions manually starts the acquisition chain.
	 *
	 * @note  If your code uses an uninterruptible task, you do not need to
	 *        start Data Acquisition manually, it will automatically be started
	 *        at the same time as the task as their internal behavior are
	 *        intrinsically linked.
	 *        If for some reason you have an uninterruptible task in your code,
	 *        but do not want the Scheduling module to be in charge of Data
	 *        Acquisition, you need to indicate it when starting the uninterruptible task.
	 *        In that case, Data Acquisition must be manually started using this
	 *        function. Note that in taht case, dispatch will use DMA interrupts
	 *        which consumes a non-negligible amount of processor time and it is not advised.
	 *
	 * @note  Data Acquisition must be started only after ADC module configuration
	 *        has been fully carried out. No ADC configuration change is allowed
	 *        after module has been started. If you're using the Twist shield and
	 *        are not sure how to initialize ADCs, you can use
	 *        data.enableTwistDefaultChannels() for that purpose.
	 *
	 * @note  Data Acquisition must be started before accessing any data.get*()
	 *        or data.peek*() function. Other Data Acquisition functions
	 *        are safe to use before starting the module.
	 *
	 * @return 0 if everything went well, -1 if there was an error.
	 *         Error is triggered when dispatch method is set to
	 *         be external, but the repetition value has not provided.
	 *         Another source of error is trying to start
	 *         Data Acquisition after it has already been started.
	 */
	int8_t start();

	/**
	 * @brief Checks if the module is already started.
	 *
	 *        For auto-spawning threads, this allows to make sure the module
	 *        has already been started before trying to access measures.
	 *
	 *        If you don't use (or don't know what are) auto-spawning threads,
	 *        just make sure calls to any data.get*() or data.peek*()
	 *        function occur after the uninterruptible task is started, or
	 *        Data Acquisition is manually started, and ignore this function.
	 *
	 * @return true is the module has been started, false otherwise.
	 */
	bool started();

	/**
	 * @brief Sets the dispatch method of the module.
	 *
	 *        Dispatch makes data from ADCs available to data.get*()
	 *        functions, thus available to the user.
	 *        By default, dispatch is done on interrupt when DMA buffer is
	 *        filled. However, if using the uninterruptible task from the
	 *        scheduling module, Scheduling will take care of dispatch itself
	 *        for better performances. This function is by this module to
	 *        indicate that dispatch is triggered externally.
	 *
	 * @note  End-user should not worry about this function, which
	 *        is used internally by the Scheduling module.
	 *
	 * @param dispatch_method Indicates when the dispatch should be done
	 *        (default value: DispatchMethod_t::on_dma_interrupt)
	 */
	void setDispatchMethod(DispatchMethod_t dispatch_method);

	/**
	 * @brief  Gets the dispatch method of the module.
	 *
	 * @note   End-user should not worry about this function, which
	 *         is used internally by the Scheduling module.
	 *
	 * @return Dispatch method indicatinng when the dispatch is done.
	 */
	DispatchMethod_t getDispatchMethod();

	/**
	 * @brief Indicates the repetition count between two external dispatches
	 *        when it is handled externally by the Scheduling module. This value
	 *        is used to calibrate buffers sizes.
	 *
	 * @note  End-user should not worry about this function, which
	 *        is used internally by the Scheduling module.
	 *
	 * @param repetition Number of repetitions between two calls of
	 *        dispatch. Used to calibrate buffers sizes.
	 */
	void setRepetitionsBetweenDispatches(uint32_t repetition);

	/**
	 * @brief Triggers an acquisition on a given ADC. Each channel configured
	 *        on this ADC will be acquired one after the other until all
	 *        configured channels have been acquired.
	 *
	 * @note  This function can't be called before the at least one channel
	 *        is enabled on the ADC and the DataAPI module is started,
	 *        either explicitly or by starting the Uninterruptible task.
	 *
	 * @param adc_num Number of the ADC on which to acquire channels.
	 */
	void triggerAcquisition(uint8_t adc_num);


	/////
	// Accessor API

	/**
	 * @brief Function to access the acquired data for specified pin.
	 *        This function provides a buffer in which all data that
	 *        have been acquired since last call are stored. The count
	 *        of these values is returned as an output parameter: the
	 *        user has to define a variable and pass it as the parameter
	 *        of the function. The variable will be updated with the
	 *        number of values that are available in the buffer.
	 *
	 * @note  This function can't be called before the pin is enabled.
	 *        The DataAPI module must have been started, either
	 *        explicitly or by starting the Uninterruptible task.
	 *
	 * @note  When calling this function, it invalidates the buffer
	 *        returned by a previous call to the same function.
	 *        However, different channels buffers are independent
	 *        from each other.
	 *
	 * @note When using this functions, the user is responsible for data
	 *       conversion. Use matching data.convert*() function
	 *       for this purpose.
	 *
	 * @note When using this function, DO NOT use the function to get the
	 *       latest converted value for the same channel as this function
	 *       will clear the buffer and disregard all values but the latest.
	 *
	 * @param adc_num Number of the ADC from which to obtain values.
	 * @param pin_num Number of the pin from which to obtain values.
	 * @param number_of_values_acquired Pass an uint32_t variable.
	 *        This variable will be updated with the number of values that
	 *        are present in the returned buffer.
	 *
	 * @return Pointer to a buffer in which the acquired values are stored.
	 *         If number_of_values_acquired is 0, do not try to access the
	 *         buffer as it may be nullptr.
	 */
	uint16_t* getRawValues(uint8_t adc_num, uint8_t pin_num, uint32_t& number_of_values_acquired);

	/**
	 * @brief Function to access the latest value available from a pin,
	 *        expressed in the relevant unit for the data: Volts, Amperes, or
	 *        Degree Celcius. This function will not touch anything in the
	 *        buffer, and thus can be called safely at any time after the
	 *        module has been started.
	 *
	 * @note  This function can't be called before the pin is enabled.
	 *        The DataAPI module must have been started, either
	 *        explicitly or by starting the Uninterruptible task.
	 *
	 * @param adc_num Number of the ADC from which to obtain value.
	 * @param pin_num Number of the pin from which to obtain values.
	 * @return Latest available value available from the given channel.
	 *         If there was no value acquired in this channel yet,
	 *         return value is NO_VALUE.
	 */
	float32_t peek(uint8_t adc_num, uint8_t pin_num);

	/**
	 * @brief This function returns the latest acquired measure expressed
	 *        in the relevant unit for the channel: Volts, Amperes, or
	 *        Degree Celcius.
	 *
	 * @note  This function can't be called before the pin is enabled.
	 *        The DataAPI module must have been started, either
	 *        explicitly or by starting the Uninterruptible task.
	 *
	 * @note  When using this functions, you loose the ability to access raw
	 *        values using data.get*RawValues() function for the
	 *        matching channel, as data.get*() function clears the
	 *        buffer on each call.
	 *
	 * @param adc_num Number of the ADC from which to obtain value.
	 * @param pin_num Number of the pin from which to obtain values.
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
	float32_t getLatest(uint8_t adc_num, uint8_t pin_num, uint8_t* dataValid = nullptr);

	/**
	 * @brief Use this function to convert values obtained using matching
	 *        data.get*RawValues() function to relevant
	 *        unit for the data: Volts, Amperes, or Degree Celcius.
	 *
	 * @note  This function can't be called before the pin is enabled.
	 *
	 * @param adc_num Number of the ADC from which the value originates.
	 * @param pin_num Number of the pin from which to obtain values.
	 * @param raw_value Raw value obtained from the channel buffer.
	 *
	 * @return Converted value in the relevant unit. If there is an error, returns -5000.
	 */
	float32_t convert(uint8_t adc_num, uint8_t pin_num, uint16_t raw_value);

	/**
	 * @brief Use this function to tweak the conversion values for the
	 *        channel if default values are not accurate enough.
	 *
	 * @note  This function can't be called before the pin is enabled.
	 *        The DataAPI module must not have been started, neither
	 *        explicitly nor by starting the Uninterruptible task.
	 *
	 * @param adc_num Number of the ADC to set conversion values.
	 * @param pin_num Number of the pin from which to obtain values.
	 * @param gain Gain to be applied (multiplied) to the channel raw value.
	 * @param offset Offset to be applied (added) to the channel value
	 *        after gain has been applied.
	 */
	void setParameters(uint8_t adc_num, uint8_t pin_num, float32_t gain, float32_t offset);

	/**
	 * @brief Use this function to get the current conversion parameteres for the chosen channel .
	 *
	 * @note  This function can't be called before the channel is enabled.
	 *
	 * @param channel Name of the shield channel to get a conversion parameter.
	 * @param parameter_name Paramater to be retreived: `gain` or `offset`.
	 *
	 * @return Returns the value of the parameter. Returns -5000 if the channel is not active.
	 */
	float32_t retrieveStoredParameterValue(uint8_t adc_num, uint8_t pin_num, parameter_t parameter_name);

	/**
	 * @brief Use this function to get the current conversion type for the chosen channel.
	 *
	 * @note  This function can't be called before the channel is enabled.
	 *
	 * @param channel Name of the shield channel to get a conversion parameter.
	 *
	 * @return Returns the type of convertion of the given pin. Returns -5 if the channel is not active.
	 */
	conversion_type_t retrieveStoredConversionType(uint8_t adc_num, uint8_t pin_num);


	/**
	 * @brief Store the currently configured conversion parameters of a given channel in NVS.
	 *
	 * @param[in] adc_num       ADC number
	 * @param[in] pin_num   	SPIN pin number
	 *
	 * @return 0 if parameters were correcly stored, negative value if there was an error:
	 * 			-1: There was an error,
	 * 			-5000: Channel not found.
	 */
	int8_t storeParametersInMemory(uint8_t adc_num, uint8_t pin_num);

	/**
	 * @brief Retreived previously configured conversion parameters from NVS.
	 *
	 * @param[in] adc_num       ADC number
	 * @param[in] pin_num   	SPIN pin number
	 *
	 * @return 0 if parameters were correcly retreived, negative value if there was an error:
	 *         -1: NVS is empty
	 *         -2: NVS contains data, but their version doesn't match current version
	 *         -3: NVS data is corrupted
	 *         -4: NVS contains data, but not for the requested channel
	 *         -5000: Channel not found.
	 */
	int8_t retrieveParametersFromMemory(uint8_t adc_num, uint8_t pin_num);


private:
	static int8_t enableChannel(uint8_t adc_num, uint8_t channel_num);
	static uint16_t* getChannelRawValues(uint8_t adc_num, uint8_t channel_num, uint32_t& number_of_values_acquired);
	static float32_t peekChannel(uint8_t adc_num, uint8_t channel_num);
	static float32_t getChannelLatest(uint8_t adc_num, uint8_t channel_num, uint8_t* dataValid = nullptr);
	static uint8_t getChannelRank(uint8_t adc_num, uint8_t channel_num);
	static uint8_t getChannelNumber(uint8_t adc_num, uint8_t twist_pin);

private:
	static bool is_started;
	static uint8_t channels_ranks[ADC_COUNT][CHANNELS_PER_ADC];
	static uint8_t current_rank[ADC_COUNT];
	static DispatchMethod_t dispatch_method;
	static uint32_t repetition_count_between_dispatches;

};


/////
// Public object to interact with the class

extern DataAPI data;


#endif // DATAAPI_H_
