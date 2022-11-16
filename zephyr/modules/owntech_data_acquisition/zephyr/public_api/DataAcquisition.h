/*
 * Copyright (c) 2022 LAAS-CNRS
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
 * @date   2022
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
	static void setChannnelAssignment(uint8_t adc_number, const char* channel_name, uint8_t channel_rank);

public:

	/**
	 * This functions starts the acquisition chain. It must be called
	 * after all module configuration has been carried out. No ADC
	 * configuration change is allowed after module has been started.
	 */
	static void start();

	/**
	 * Check if the module is already started.
	 * For auto-spawning threads, please make sure
	 * the module has already been started before
	 * trying to access measures.
	 */
	static bool started();


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
	 * NOTE 2: All function buffers are independent.
	 * NOTE 3: When using these functions, the user is responsible for
	 *         data conversion. Use convert***() functions for this purpose.
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
	static uint16_t* getV1LowRawValues(uint32_t& number_of_values_acquired);
	static uint16_t* getV2LowRawValues(uint32_t& number_of_values_acquired);
	static uint16_t* getVHighRawValues(uint32_t& number_of_values_acquired);
	static uint16_t* getI1LowRawValues(uint32_t& number_of_values_acquired);
	static uint16_t* getI2LowRawValues(uint32_t& number_of_values_acquired);
	static uint16_t* getIHighRawValues(uint32_t& number_of_values_acquired);
	static uint16_t* getTemperatureRawValues(uint32_t& number_of_values_acquired);
	static uint16_t* getExtraRawValues(uint32_t& number_of_values_acquired);
	static uint16_t* getAnalogCommRawValues(uint32_t& number_of_values_acquired);

	/**
	 * Functions to access the latest value available from a channel expressed
	 * in the relevant unit for the data: Volts, Amperes, Degree Celcius.
	 * These functions will not touch anything in the buffer, and thus can
	 * be called safely at any time.
	 *
	 * @return Latest available value available from the given channel.
	 */
	static float32_t peekV1Low();
	static float32_t peekV2Low();
	static float32_t peekVHigh();
	static float32_t peekI1Low();
	static float32_t peekI2Low();
	static float32_t peekIHigh();
	static float32_t peekTemperature();
	static float32_t peekExtra();

	/**
	 * These functions return the latest acquired measure expressed
	 * in the relevant unit for the data: Volts, Amperes, Degree Celcius.
	 *
	 * @return Latest acquired measure for the channel.
	 *
	 * NOTE: When using these functions for a channel, you loose the
	 *       ability to access raw values using get***RawValues() functions,
	 *       as get***() functions clear the buffers on each call.
	 */
	static float32_t getV1Low();
	static float32_t getV2Low();
	static float32_t getVHigh();
	static float32_t getI1Low();
	static float32_t getI2Low();
	static float32_t getIHigh();
	static float32_t getTemperature();
	static float32_t getExtra();
	static float32_t getAnalogComm();

	/**
	 * Use these functions to convert values obtained using
	 * get***RawValues() functions to relevant unit for the data:
	 * Volts, Amperes, Degree Celcius.
	 */
	static float32_t convertV1Low(uint16_t raw_value);
	static float32_t convertV2Low(uint16_t raw_value);
	static float32_t convertVHigh(uint16_t raw_value);
	static float32_t convertI1Low(uint16_t raw_value);
	static float32_t convertI2Low(uint16_t raw_value);
	static float32_t convertIHigh(uint16_t raw_value);
	static float32_t convertTemperature(uint16_t raw_value);
	static float32_t convertExtra(uint16_t raw_value);
	static float32_t convertAnalogComm(uint16_t raw_value);

	/**
	 * Use these functions to tweak the conversion values for
	 * a specific sensor if default values are not accurate enough.
	 */
	static void setV1LowParameters(float32_t gain, float32_t offset);
	static void setI1LowParameters(float32_t gain, float32_t offset);
	static void setV2LowParameters(float32_t gain, float32_t offset);
	static void setI2LowParameters(float32_t gain, float32_t offset);
	static void setVHighParameters(float32_t gain, float32_t offset);
	static void setIHighParameters(float32_t gain, float32_t offset);
	static void setTemperatureParameters(float32_t gain, float32_t offset);
	static void setExtraParameters(float32_t gain, float32_t offset);
	static void setAnalogCommParameters(float32_t gain, float32_t offset);


private:
	typedef struct
	{
		uint8_t adc_number;
		uint8_t channel_rank;
	} channel_assignment_t;

private:
	static bool is_started;

	static channel_assignment_t v1_low_assignement;
	static channel_assignment_t v2_low_assignement;
	static channel_assignment_t v_high_assignement;
	static channel_assignment_t i1_low_assignement;
	static channel_assignment_t i2_low_assignement;
	static channel_assignment_t i_high_assignement;
	static channel_assignment_t temp_sensor_assignement;
	static channel_assignment_t extra_sensor_assignement;
	static channel_assignment_t analog_comm_assignement;

};


/////
// Public object to interact with the class

extern DataAcquisition dataAcquisition;



#endif // DATAACQUISITION_H_
