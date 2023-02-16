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
 */


#ifndef DATAACQUISITION_H_
#define DATAACQUISITION_H_


// Stdlib
#include <stdint.h>

// ARM CMSIS library
#include <arm_math.h>


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
	 * This functions starts the acquisition chain. It must be called
	 * after ADC module configuration has been fully carried out. No ADC
	 * configuration change is allowed after module has been started.
	 * If you're not sure how to initialize ADC, just use the Hardware
	 * Configuration module API: hwConfig.configureAdcDefaultAllMeasurements()
	 *
	 * NOTE 1: If your code uses an uninterruptible task, you do not need to start
	 * Data Acquisition manually, it will automatically be started at the same
	 * time as the task as their internal behavior are intrinsically linked.
	 *
	 * NOTE 2: Data Acquisition must be started before accessing any
	 * dataAcquisition.get*() or dataAcquisition.peek*() function.
	 * Other Data Acquisition functions are safe to use before starting
	 * the module.
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
	 *        but stoill want the dispatch to be done on DMA interrupt,
	 *        you need to call this function prior to starting the task.
	 *        Note that using DMA interrupts will consume a non-negligible
	 *        amount of processor time and it is not advised.
	 *
	 * @return 0 if everything went well, -1 if there was an error.
	 *         Error is triggered when dispatch method is set to
	 *         uninterruptible task start, but the task has not been
	 *         defined yet.
	 */
	int8_t start(dispatch_method_t dispatch_method = on_dma_interrupt);

	/**
	 * Check if the module is already started.
	 *
	 * For auto-spawning threads, this allows to make sure the module
	 * has already been started before trying to access measures.
	 *
	 * If you don't use (or don't know what are) auto-spawning threads,
	 * just make sure you call dataAcquisition.start() before accessing
	 * any dataAcquisition.get*() or dataAcquisition.peek*() function,
	 * and ignore this one.
	 */
	bool started();


	/////
	// Accessor API

	/**
	 * Functions to access the acquired data for each channel.
	 * Each function provides a buffer in which all data that
	 * have been acquired since last call are stored. The count
	 * of these values is returned as an output parameter: the
	 * user has to define a variable and pass a reference to this
	 * variable as the parameter of the function. The variable
	 * will be updated with the number of values that are available
	 * in the buffer.
	 *
	 * NOTE 1: When calling one of these functions, it invalidates
	 *         the buffer returned by a previous call to the same function.
	 * NOTE 2: All function buffers are independent from each other.
	 * NOTE 3: When using these functions, the user is responsible for data
	 *         conversion. Use dataAcquisition.convert*() functions for this
	 *         purpose.
	 * NOTE 4: When using these functions for a channel, DO NOT use the
	 *         functions to get the latest converted value for the same channel
	 *         as these functions clear the buffer and disregard all values
	 *         but the latest.
	 *
	 * @param  number_of_values_acquired Pass an uint32_t variable.
	 *         This variable will be updated with the number of values that
	 *         have been acquired for this channel.
	 * @return Pointer to a buffer in which the acquired values are stored.
	 *         If number_of_values_acquired is 0, do not try to access the
	 *         buffer as it may be NULL.
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

	/**
	 * Functions to access the latest value available from a channel expressed
	 * in the relevant unit for the data: Volts, Amperes, Degree Celcius.
	 * These functions will not touch anything in the buffer, and thus can
	 * be called safely at any time.
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

	/**
	 * These functions return the latest acquired measure expressed
	 * in the relevant unit for the data: Volts, Amperes, Degree Celcius.
	 *
	 * NOTE: When using these functions for a channel, you loose the
	 *       ability to access raw values using dataAcquisition.get*RawValues()
	 *       functions, as dataAcquisition.get*() functions clear the buffers
	 *       on each call.
	 *
	 * @param dataValid Pointer to an uint8_t variable. This parameter is
	 *        facultative. If this parameter is provided, it will be updated
	 *        to indicate information about data. Possible values for this
	 *        parameter will be: DATA_IS_OK if returned data is a newly acquired
	 *        data, DATA_IS_OLD if returned data has already been provided before
	 *        (no new data available since latest time this function was called),
	 *        DATA_IS_MISSING if returned data is NO_VALUE.
	 * @return Latest acquired measure for the channel.
	 *         If no value was acquired in this channel yet, return value is NO_VALUE.
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

	/**
	 * Use these functions to convert values obtained using
	 * dataAcquisition.get*RawValues() functions to relevant
	 * unit for the data: Volts, Amperes, Degree Celcius.
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

	/**
	 * Use these functions to tweak the conversion values for
	 * a specific sensor if default values are not accurate enough.
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

private:
	typedef struct
	{
		uint8_t adc_number;
		uint8_t channel_rank;
	} channel_assignment_t;

private:
	/**
	 * This function is used to indicate to the DataAcquisition module
	 * what the underlying ADC channel configuration is.
	 *
	 * @param adc_number ADC number
	 * @param channel_name Channel name
	 * @param channel_rannk Channel rank
	 */
	void setChannnelAssignment(uint8_t adc_number, const char* channel_name, uint8_t channel_rank);

	float32_t _getChannel(channel_assignment_t assignment, float32_t(*convert)(uint16_t), uint8_t* dataValid);
	uint16_t* _getRawValues(channel_assignment_t assignment, uint32_t& number_of_values_acquired);
	float32_t _peek(channel_assignment_t assignment, float32_t(*convert)(uint16_t));

private:
	bool is_started = false;

	channel_assignment_t v1_low_assignement       = {0};
	channel_assignment_t v2_low_assignement       = {0};
	channel_assignment_t v_high_assignement       = {0};
	channel_assignment_t i1_low_assignement       = {0};
	channel_assignment_t i2_low_assignement       = {0};
	channel_assignment_t i_high_assignement       = {0};
	channel_assignment_t temp_sensor_assignement  = {0};
	channel_assignment_t extra_sensor_assignement = {0};
	channel_assignment_t analog_comm_assignement  = {0};

};


/////
// Public object to interact with the class

extern DataAcquisition dataAcquisition;


#endif // DATAACQUISITION_H_
