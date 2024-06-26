/*
 * Copyright (c) 2022-2024 LAAS-CNRS
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

// Other modules includes
#include "adc.h"

// Current module private functions
#include "./data/data_conversion.h"


/////
// Type definitions

typedef enum : uint8_t
{
	gain = 1,
	offset = 2

} parameter_t;

typedef enum : int8_t
{
	UNKNOWN_ADC = -1,
	DEFAULT_ADC = 0,
	ADC_1 = 1,
	ADC_2 = 2,
	ADC_3 = 3,
	ADC_4 = 4,
	ADC_5 = 5
} adc_t;

enum class DispatchMethod_t
{
	on_dma_interrupt,
	externally_triggered
};

//////
// Constants definitions

static const uint8_t ADC_COUNT = 5;
static const uint8_t PIN_COUNT = 59;
static const uint8_t CHANNELS_PER_ADC = 19;

// Define "no value" as an impossible, out of range value
const float32_t NO_VALUE = -10000;
#define ERROR_CHANNEL_OFF -5
#define ERROR_CHANNEL_NOT_FOUND -2

const uint8_t DATA_IS_OK      = 0;
const uint8_t DATA_IS_OLD     = 1;
const uint8_t DATA_IS_MISSING = 2;

/////
// Static class definition

class DataAPI
{
	// Allow specific extenal members to access private members of this class
	friend class SensorsAPI;
	friend void user_task_proxy();
	friend void scheduling_start_uninterruptible_synchronous_task(bool);

public:

	/**
	 * @brief This function is used to enable acquisition on a Spin PIN with
	 *        a given ADC.
	 *
	 * @note  Not any pin can be used for acquisiton: the pin must be linked
	 *        to a channel of the given ADC. Refer to Spin pinout image for
	 *        PIN/ADC relations.
	 *
	 * @note  This function must be called *before* Data API is started.
	 *
	 * @param[in] pin_number Number of the Spin pin on which to enable acquisition.
	 * @param[in] adc_number Number of the ADC on which acquisition is to be done.
	 *            This parameter can be omitted, in which case:
	 *            - If only one ADC is linked to the given pin, it will be selected.
	 *            - If two ADCs are available for acquisition on the given pin, the
	 *            ADC with the lowest number will be selected.
	 *
	 * @return 0 if acquisition was correctly enabled, -1 if there was an error.
	 *         Errors generally indicate that the given pin is not linked to and
	 *         ADC, and thus can not be used for acquisition. If the adc_number
	 *         parameter was explicitely provided, it can also indicate that the
	 *         given ADC is not available for acquisition on the provided pin.
	 */
	int8_t enableAcquisition(uint8_t pin_number, adc_t adc_number = DEFAULT_ADC);

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
	 * @brief Stops the module if it is started.
	 *
	 * @return 0 if everything went well, -1 if there was an error.
	 *         Error is triggered when trying to stop Data API
	 *         while it was not started.
	 */
	int8_t stop();

	/**
	 * @brief Triggers an acquisition on a given ADC. Each channel configured
	 *        on this ADC will be acquired one after the other until all
	 *        configured channels have been acquired.
	 *
	 * @note  This function can't be called before the at least one channel
	 *        is enabled on the ADC and the DataAPI module is started,
	 *        either explicitly or by starting the Uninterruptible task.
	 *
	 * @param[in] adc_number Number of the ADC on which to acquire channels.
	 */
	void triggerAcquisition(adc_t adc_number);

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
	 *       conversion. Use data.convertValue() function for this purpose.
	 *
	 * @note When using this function, DO NOT use the function to get the
	 *       latest converted value for the same channel as this function
	 *       will clear the buffer and disregard all values but the latest.
	 *
	 * @param[in]  pin_number Number of the pin from which to obtain values.
	 * @param[out] number_of_values_acquired Pass an uint32_t variable.
	 *        This variable will be updated with the number of values that
	 *        are present in the returned buffer.
	 *
	 * @return Pointer to a buffer in which the acquired values are stored.
	 *         If number_of_values_acquired is 0, do not try to access the
	 *         buffer as it may be nullptr.
	 */
	uint16_t* getRawValues(uint8_t pin_number, uint32_t& number_of_values_acquired);

	/**
	 * @brief Function to access the acquired data for specified pin.
	 *        This function converts all values that have been acquired
	 *        since last call are stored and provide an array containing
	 *        all of them. The count of these values is returned as an
	 *        output parameter: the user has to define a variable and pass
	 *        it as the parameter of the function. The variable will be
	 *        updated with the number of values that are available in the buffer.
	 *
	 * @warning This is an expensive function. Calling this function trigger
	 *          the conversion of all values acquired since the last call.
	 *          If only the lastet value is required, it is advised to call
	 *          getLatestValue() instead. If multiple values are required,
	 *          but not all, it is advised to call getRawValues() instead,
	 *          then explicitely convert required values using convertValue().
	 *
	 * @note  This function can't be called before the pin is enabled.
	 *        The DataAPI module must have been started, either
	 *        explicitly or by starting the Uninterruptible task.
	 *
	 * @note  When calling this function, it invalidates the array
	 *        returned by a previous call to the same function.
	 *        However, different channels buffers are independent
	 *        from each other.
	 *
	 * @param[in]  pin_number Number of the pin from which to obtain values.
	 * @param[out] number_of_values_acquired Pass an uint32_t variable.
	 *        This variable will be updated with the number of values that
	 *        are present in the returned buffer.
	 *
	 * @return Pointer to an array in which the acquired values are stored.
	 *         If number_of_values_acquired is 0, do not try to access the
	 *         buffer as it may be nullptr.
	 */
	float32_t* getValues(uint8_t pin_number, uint32_t& number_of_values_acquired);

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
	 * @param[in] pin_number Number of the pin from which to obtain values.
	 *
	 * @return Latest available value available from the given channel.
	 *         If there was no value acquired in this channel yet,
	 *         return value is NO_VALUE.
	 */
	float32_t peekLatestValue(uint8_t pin_number);

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
	 *        values using data.getRawValues() function for the
	 *        matching channel, as data.getLatestValue() function clears the
	 *        buffer on each call.
	 *
	 * @param[in]  pin_number Number of the pin from which to obtain values.
	 * @param[out] dataValid Pointer to an uint8_t variable. This parameter is
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
	float32_t getLatestValue(uint8_t pin_number, uint8_t* dataValid = nullptr);

	/**
	 * @brief Use this function to convert values obtained using matching
	 *        data.getRawValues() function to relevant
	 *        unit for the data: Volts, Amperes, or Degree Celcius.
	 *
	 * @note  This function can't be called before the pin is enabled.
	 *
	 * @param[in] pin_number Number of the pin from which to obtain values.
	 * @param[in] raw_value Raw value obtained from the channel buffer.
	 *
	 * @return Converted value in the relevant unit. If there is an error, returns -5000.
	 */
	float32_t convertValue(uint8_t pin_number, uint16_t raw_value);

	/**
	 * @brief Use this function to tweak the conversion values for the
	 *        channel if default values are not accurate enough.
	 *
	 * @note  This function can't be called before the pin is enabled.
	 *        The DataAPI module must not have been started, neither
	 *        explicitly nor by starting the Uninterruptible task.
	 *
	 * @param[in] pin_number Number of the pin from which to obtain values.
	 * @param[in] gain Gain to be applied (multiplied) to the channel raw value.
	 * @param[in] offset Offset to be applied (added) to the channel value
	 *        after gain has been applied.
	 */
	void setConversionParameters(uint8_t pin_number, float32_t gain, float32_t offset);

	/**
	 * @brief Use this function to get the current conversion parameteres for the chosen channel .
	 *
	 * @note  This function can't be called before the channel is enabled.
	 *
	 * @param[in] pin_number Number of the pin from which to obtain values.
	 * @param[in] parameter_name Paramater to be retreived: `gain` or `offset`.
	 *
	 * @return Returns the value of the parameter. Returns -5000 if the channel is not active.
	 */
	float32_t getConversionParameterValue(uint8_t pin_number, parameter_t parameter_name);

	/**
	 * @brief Use this function to get the current conversion type for the chosen channel.
	 *
	 * @note  This function can't be called before the channel is enabled.
	 *
	 * @param[in] pin_number Number of the pin from which to obtain values.
	 *
	 * @return Returns the type of convertion of the given pin. Returns -5 if the channel is not active.
	 */
	conversion_type_t getConversionParameterType(uint8_t pin_number);

	/**
	 * @brief Store the currently configured conversion parameters of a given channel in persistent memory.
	 *
	 * @param[in] pin_number SPIN pin number
	 *
	 * @return 0 if parameters were correctly stored, negative value if there was an error:
	 * 			-1: There was an error,
	 * 			-5000: pin not found.
	 */
	int8_t storeConversionParametersInMemory(uint8_t pin_number);

	/**
	 * @brief Retrieved previously configured conversion parameters from persistent memory.
	 *
	 * @param[in] pin_number SPIN pin number
	 *
	 * @return 0 if parameters were correcly retrieved, negative value if there was an error:
	 *         -1: persistent memory is empty
	 *         -2: persistent memory contains data, but its version doesn't match current version
	 *         -3: data in persistent memory is corrupted
	 *         -4: persistent memory contains data, but not for the requested pin
	 *         -5000: pin not found.
	 */
	int8_t retrieveConversionParametersFromMemory(uint8_t pin_number);

	/**
	 * @brief Set the discontinuous count for an ADC.
	 *        By default, ADCs are not in discontinuous mode.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @note This is an advanced function that requires to understand the
	 *       way the ADC work. Only for use if you explicitely requires it.
	 *
	 * @param[in] adc_number Number of the ADC to configure.
	 * @param[in] discontinuous_count Number of channels to acquire on each
	 *            trigger event. 0 to disable discontinuous mode (default).
	 */
	void configureDiscontinuousMode(adc_t adc_number, uint32_t dicontinuous_count);

	/**
	 * @brief Change the trigger source of an ADC.
	 *        By default, triggger source for ADC 1/2 is on HRTIM1,
	 *        and ADC 3/4 is software-triggered.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param[in] adc_number Number of the ADC to configure
	 * @param[in] trigger_source Source of the trigger
	 */
	void configureTriggerSource(adc_t adc_number, adc_ev_src_t trigger_source);

private:
	static void initializeAllAdcs();
	static int8_t enableChannel(adc_t adc_number, uint8_t channel_num);
	static void disableChannel(adc_t adc_number, uint8_t channel);
	static uint16_t* getChannelRawValues(adc_t adc_number, uint8_t channel_num, uint32_t& number_of_values_acquired);
	static float32_t* getChannelValues(adc_t adc_number, uint8_t channel_num, uint32_t& number_of_values_acquired);
	static float32_t peekChannel(adc_t adc_number, uint8_t channel_num);
	static float32_t getChannelLatest(adc_t adc_number, uint8_t channel_num, uint8_t* dataValid = nullptr);
	static uint8_t getChannelRank(adc_t adc_number, uint8_t channel_num);
	static uint8_t getChannelNumber(adc_t adc_number, uint8_t twist_pin);
	static adc_t getDefaultAdcForPin(uint8_t pin_number);
	static adc_t getCurrentAdcForPin(uint8_t pin_number);

	// Private members accessed by external friend members
	static void setRepetitionsBetweenDispatches(uint32_t repetition);
	static void setDispatchMethod(DispatchMethod_t dispatch_method);
	static void doFullDispatch();

private:
	static bool is_started;
	static bool adcInitialized;
	static uint8_t channels_ranks[ADC_COUNT][CHANNELS_PER_ADC];
	static uint8_t current_rank[ADC_COUNT];
	static DispatchMethod_t dispatch_method;
	static uint32_t repetition_count_between_dispatches;
	static adc_t current_adc[PIN_COUNT];
	static float32_t*** converted_values_buffer;

};

#endif // DATAAPI_H_
