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
 * @author Thomas Walter <thomas.walter@laas.fr>
 */


#ifndef DATAACQUISITION_H_
#define DATAACQUISITION_H_


// Stdlib
#include <stdint.h>

// ARM CMSIS library
#include <arm_math.h>

// Current module private functions
#include "../src/data_conversion.h"


/////
// Public definitions

typedef enum
{
	on_dma_interrupt,
	at_uninterruptible_task_start
} dispatch_method_t;

// Define "no value" as an impossible, out of range value
const float32_t NO_VALUE = -10000;

const uint8_t DATA_IS_OK = 0;
const uint8_t DATA_IS_OLD = 1;
const uint8_t DATA_IS_MISSING = 2;

/////
// Static class definition

class DataAcquisition
{
public:

	/**
	 * This function is used to configure all ADC channels in default configuration.
	 * Channels will be attributed as follows:
	 * ADC1 -   V1_LOW      ADC2 -  I1_LOW
	 *          V2_LOW              I2_LOW
	 *          V_HIGH              I_HIGH
	 *
	 * This function must be called BEFORE ADC is started.
	 */
	static int8_t configureAdcChannels(uint8_t adc_number, const char* channel_list[], uint8_t channel_count);

	/**
	 * @brief  This function is used to configure the channels to be
	 *         enabled on a given ADC.
	 *
	 * @param  adc_number Number of the ADC on which channel configuration is
	 *         to be done.
	 * @param  channel_list List of channels to configure. This is a list of
	 *         names as defined in the device tree (field `label`). The order
	 *         of the names in the array sets the acquisition ranks (order in
	 *         which the channels are acquired).
	 * @param  channel_count Number of channels defined in `channel_list`.
	 * @return 0 is everything went well,
	 *         ECHANNOTFOUND if at least one of the channels
	 *           is not available in the given ADC. Available channels are the
	 *           ones defined in the device tree.
	 */
	static void configureAdcDefaultAllMeasurements();

	/**
	 * @brief This functions starts the acquisition chain.
	 *
	 * @note If your code uses an uninterruptible task, you do not need to
	 *       start Data Acquisition manually, it will automatically be started
	 *       at the same time as the task as their internal behavior are
	 *       intrinsically linked.
	 *
	 * @note Data Acquisition must be started only after ADC module configuration
	 *       has been fully carried out. No ADC configuration change is allowed
	 *       after module has been started. If you're not sure how to initialize
	 *       ADCs, just use the Hardware Configuration module API:
	 *       hwConfig.configureAdcDefaultAllMeasurements()
	 *
	 * @note Data Acquisition must be started before accessing any dataAcquisition.get*()
	 *       or dataAcquisition.peek*() function. Other Data Acquisition functions
	 *       are safe to use before starting the module.
	 *
	 * @param dispatch_method Indicates when the dispatch should be done.
	 *        Dispatch makes data from ADCs available to dataAcquisition.get*()
	 *        functions, thus available to the user.
	 *        You should not worry too much about this parameter, as if you
	 *        call this function manually, the default value is what you want,
	 *        so just call the function without any parameter.
	 *        By default, Data Acquisition is started automatically when
	 *        the uninterruptible task is started, with dispatch method
	 *        set to at_uninterruptible_task_start. However, if you do not
	 *        use an uninterrptible task in your application, default parameter
	 *        on_dma_interrupt is the correct value.
	 *        If for some reason you have an uninterruptible task in your code,
	 *        but still want the dispatch to be done on DMA interrupt,
	 *        you need to call this function prior to starting the task.
	 *        Note that using DMA interrupts will consume a non-negligible
	 *        amount of processor time and it is not advised.
	 *
	 * @return 0 if everything went well, -1 if there was an error.
	 *         Error is triggered when dispatch method is set to
	 *         uninterruptible task start, but the task has not been
	 *         defined yet. Another source of error is trying to start
	 *         Data Acquisition after it has already been started.
	 */
	int8_t start(dispatch_method_t dispatch_method = on_dma_interrupt);

	/**
	 * @brief Checks if the module is already started.
	 *
	 * For auto-spawning threads, this allows to make sure the module
	 * has already been started before trying to access measures.
	 *
	 * If you don't use (or don't know what are) auto-spawning threads,
	 * just make sure calls to any dataAcquisition.get*() or dataAcquisition.peek*()
	 * function occur after the uninterruptible task is started, or
	 * Data Acquisition is manually started, and ignore this function.
	 *
	 * @return true is the module has been started, false otherwise.
	 */
	bool started();


	/////
	// Accessor API

	///@{
	/**
	 * @brief Function to access the acquired data for specified channel.
	 *        This function provides a buffer in which all data that
	 *        have been acquired since last call are stored. The count
	 *        of these values is returned as an output parameter: the
	 *        user has to define a variable and pass it as the parameter
	 *        of the function. The variable will be updated with the
	 *        number of values that are available in the buffer.
	 *
	 * @note When calling this function, it invalidates the buffer
	 *       returned by a previous call to the same function.
	 *       However, different channels buffers are independent
	 *       from each other.
	 *
	 * @note When using this functions, the user is responsible for data
	 *       conversion. Use matching dataAcquisition.convert*() function
	 *       for this purpose.
	 *
	 * @note When using this function, DO NOT use the function to get the
	 *       latest converted value for the same channel as this function
	 *       will clear the buffer and disregard all values but the latest.
	 *
	 * @param  number_of_values_acquired Pass an uint32_t variable.
	 *         This variable will be updated with the number of values that
	 *         are present in the returned buffer.
	 *
	 * @return Pointer to a buffer in which the acquired values are stored.
	 *         If number_of_values_acquired is 0, do not try to access the
	 *         buffer as it may be nullptr.
	 */
	uint16_t* getV1LowRawValues(uint32_t& number_of_values_acquired);
	uint16_t* getV2LowRawValues(uint32_t& number_of_values_acquired);
	uint16_t* getVHighRawValues(uint32_t& number_of_values_acquired);
	uint16_t* getI1LowRawValues(uint32_t& number_of_values_acquired);
	uint16_t* getI2LowRawValues(uint32_t& number_of_values_acquired);
	uint16_t* getIHighRawValues(uint32_t& number_of_values_acquired);
	uint16_t* getTemperatureRawValues(uint32_t& number_of_values_acquired);
	uint16_t* getExtraRawValues(uint32_t& number_of_values_acquired);
	uint16_t* getAnalogCommRawValues(uint32_t& number_of_values_acquired);
	///@}

	///@{
	/**
	 * @brief Function to access the latest value available from the channel,
	 *        expressed in the relevant unit for the data: Volts, Amperes, or
	 *        Degree Celcius. This function will not touch anything in the
	 *        buffer, and thus can be called safely at any time after the
	 *        module has been started.
	 *
	 * @return Latest available value available from the given channel.
	 *         If there was no value acquired in this channel yet,
	 *         return value is NO_VALUE.
	 */
	float32_t peekV1Low();
	float32_t peekV2Low();
	float32_t peekVHigh();
	float32_t peekI1Low();
	float32_t peekI2Low();
	float32_t peekIHigh();
	float32_t peekTemperature();
	float32_t peekExtra();
	float32_t peekAnalogComm();
	///@}

	///@{
	/**
	 * @brief This function returns the latest acquired measure expressed
	 *        in the relevant unit for the channel: Volts, Amperes, or
	 *        Degree Celcius.
	 *
	 * @note When using this functions, you loose the ability to access raw
	 *       values using dataAcquisition.get*RawValues() function for the
	 *       matching channel, as dataAcquisition.get*() function clears the
	 *       buffer on each call.
	 *
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
	float32_t getV1Low(uint8_t* dataValid = nullptr);
	float32_t getV2Low(uint8_t* dataValid = nullptr);
	float32_t getVHigh(uint8_t* dataValid = nullptr);
	float32_t getI1Low(uint8_t* dataValid = nullptr);
	float32_t getI2Low(uint8_t* dataValid = nullptr);
	float32_t getIHigh(uint8_t* dataValid = nullptr);
	float32_t getTemperature(uint8_t* dataValid = nullptr);
	float32_t getExtra(uint8_t* dataValid = nullptr);
	float32_t getAnalogComm(uint8_t* dataValid = nullptr);
	///@}

	///@{
	/**
	 * @brief Use this function to convert values obtained using matching
	 *        dataAcquisition.get*RawValues() function to relevant
	 *        unit for the data: Volts, Amperes, or Degree Celcius.
	 *
	 * @param raw_value Raw value obtained from the channel buffer.
	 *
	 * @return Converted value in the relevant unit.
	 */
	float32_t convertV1Low(uint16_t raw_value);
	float32_t convertV2Low(uint16_t raw_value);
	float32_t convertVHigh(uint16_t raw_value);
	float32_t convertI1Low(uint16_t raw_value);
	float32_t convertI2Low(uint16_t raw_value);
	float32_t convertIHigh(uint16_t raw_value);
	float32_t convertTemperature(uint16_t raw_value);
	float32_t convertExtra(uint16_t raw_value);
	float32_t convertAnalogComm(uint16_t raw_value);
	///@}

	///@{
	/**
	 * @brief Use this function to tweak the conversion values for the
	 *        channel if default values are not accurate enough.
	 *
	 * @param gain Gain to be applied (multiplied) to the channel raw value.
	 *
	 * @param offset Offset to be applied (added) to the channel value
	 *        after gain has been applied.
	 */
	void setV1LowParameters(float32_t gain, float32_t offset);
	void setI1LowParameters(float32_t gain, float32_t offset);
	void setV2LowParameters(float32_t gain, float32_t offset);
	void setI2LowParameters(float32_t gain, float32_t offset);
	void setVHighParameters(float32_t gain, float32_t offset);
	void setIHighParameters(float32_t gain, float32_t offset);
	void setTemperatureParameters(float32_t gain, float32_t offset);
	void setExtraParameters(float32_t gain, float32_t offset);
	void setAnalogCommParameters(float32_t gain, float32_t offset);
	///@}

	/**
	 * @brief    Init default gain and offset structures for all ADCs and
	 * sets these default parameters.
	 */
	void setDefaultCalibrationFactors(void);

	/**
	 * @brief    Retrieve stored parameters from Flash memory and configure ADC parameters
	 */
	void setUserCalibrationFactors(void);

private:
	/**
	 * Internal types definitions.
	 */
	typedef float32_t(*conversion_function_t)(uint16_t);
	typedef void(*set_convert_params_function_t)(float32_t, float32_t);

	typedef struct
	{
		uint8_t adc_number;
		uint8_t channel_rank;
		conversion_function_t convert;
		set_convert_params_function_t set_parameters;
	} channel_assignment_t;

private:
	/**
	 * Helper functions to share code.
	 */
	void      _setAssignment(channel_assignment_t& assignment, uint8_t adc_number, uint8_t channel_rank);
	float32_t _getLatest(channel_assignment_t assignment, uint8_t* dataValid);
	uint16_t* _getRawValues(channel_assignment_t assignment, uint32_t& number_of_values_acquired);
	float32_t _peek(channel_assignment_t assignment);
	float32_t _convert(channel_assignment_t assignment, uint16_t raw_value);
	void      _setParameters(channel_assignment_t assignment, float32_t gain, float32_t offset);

private:
	bool is_started = false;

	channel_assignment_t v1_low_assignement       = {0, 0, data_conversion_convert_v1_low,      data_conversion_set_v1_low_parameters     };
	channel_assignment_t v2_low_assignement       = {0, 0, data_conversion_convert_v2_low,      data_conversion_set_v2_low_parameters     };
	channel_assignment_t v_high_assignement       = {0, 0, data_conversion_convert_v_high,      data_conversion_set_v_high_parameters     };
	channel_assignment_t i1_low_assignement       = {0, 0, data_conversion_convert_i1_low,      data_conversion_set_i1_low_parameters     };
	channel_assignment_t i2_low_assignement       = {0, 0, data_conversion_convert_i2_low,      data_conversion_set_i2_low_parameters     };
	channel_assignment_t i_high_assignement       = {0, 0, data_conversion_convert_i_high,      data_conversion_set_i_high_parameters     };
	channel_assignment_t temp_sensor_assignement  = {0, 0, data_conversion_convert_temp,        data_conversion_set_temp_parameters       };
	channel_assignment_t extra_sensor_assignement = {0, 0, data_conversion_convert_extra,       data_conversion_set_extra_parameters      };
	channel_assignment_t analog_comm_assignement  = {0, 0, data_conversion_convert_analog_comm, data_conversion_set_analog_comm_parameters};

};


/////
// Public object to interact with the class

extern DataAcquisition dataAcquisition;


#endif // DATAACQUISITION_H_
