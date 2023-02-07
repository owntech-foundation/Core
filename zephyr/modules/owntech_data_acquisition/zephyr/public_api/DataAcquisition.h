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
// Static class definition

class DataAcquisition
{
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

public:

	/**
	 * This functions starts the acquisition chain. It must be called
	 * after ADC module configuration has been fully carried out. No ADC
	 * configuration change is allowed after module has been started.
	 * If you're not sure how to initialize ADC, just use the Hardware
	 * Configuration module API: hwConfig.configureAdcDefaultAllMeasurements()
	 *
	 * NOTE: This function must be called before accessing any dataAcquisition.get*()
	 * or dataAcquisition.peek*() function. Other functions are safe to
	 * use before starting the module.
	 *
	 * @return 0 if everything went well, -1 if there was an error.
	 *         Error is triggered when dispatch method is set to
	 *         uninterruptible task start, but the task has not been
	 *         defined yet.
	 */
	void start();

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
	 */
	float32_t peekV1Low();
	float32_t peekV2Low();
	float32_t peekVHigh();
	float32_t peekI1Low();
	float32_t peekI2Low();
	float32_t peekIHigh();
	float32_t peekTemperature();
	float32_t peekExtra();

	/**
	 * These functions return the latest acquired measure expressed
	 * in the relevant unit for the data: Volts, Amperes, Degree Celcius.
	 *
	 * NOTE: When using these functions for a channel, you loose the
	 *       ability to access raw values using dataAcquisition.get*RawValues()
	 *       functions, as dataAcquisition.get*() functions clear the buffers
	 *       on each call.
	 *
	 * @return Latest acquired measure for the channel.
	 */
	float32_t getV1Low();
	float32_t getV2Low();
	float32_t getVHigh();
	float32_t getI1Low();
	float32_t getI2Low();
	float32_t getIHigh();
	float32_t getTemperature();
	float32_t getExtra();
	float32_t getAnalogComm();

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
