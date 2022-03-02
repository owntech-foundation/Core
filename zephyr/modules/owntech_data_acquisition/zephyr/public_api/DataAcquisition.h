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
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef DATAACQUISITION_H_
#define DATAACQUISITION_H_


// Stdlib
#include <stdint.h>

// ARM CMSIS library
#include <arm_math.h>


/////
// Public enums
typedef enum
{
	hrtim1,
	software
} adc_src_t;


/////
// Static class definition

class DataAcquisition
{

private:

	/**
	 * This function initializes the Data Acquisition module.
	 */
	static void initialize();

	/**
	 *
	 */
	static void setChannnelAssignment(uint8_t adc_number, const char* channel_name, uint8_t channel_rank);

public:

	/**
	 * Use this function to set ADC 1 and ADC 2 in dual mode.
	 * By default, ADC 1 and 2 are not in dual mode.
	 *
	 * This function must be called BEFORE dataAcquisition.start().
	 *
	 * @param  dual_mode Status of the dual mode:
	 *         true to enable,
	 *         false to disable.
	 * @return 0 is everything went well,
	 *         EALREADYSTARTED if the module has already been started.
	 */
	static int8_t configureAdc12DualMode(uint8_t dual_mode);

	/**
	 * This function is used to configure the channels to be
	 * enabled on a given ADC.
	 *
	 * This function must be called BEFORE dataAcquisition.start().
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
	 *         EALREADYSTARTED if the module has already been started.
	 */
	static int8_t configureAdcChannels(uint8_t adc_number, const char* channel_list[], uint8_t channel_count);

	/**
	 * This function is used to change the trigger source of an ADC.
	 * By default, triggger source for ADC 1 and ADC 2 is on HRTIM1,
	 * and ADC 3 is software-triggered.
	 *
	 * This function must be called BEFORE dataAcquisition.start().
	 *
	 * @param  adc_number Number of the ADC to configure
	 * @param  trigger_source Source of the trigger
	 * @return 0 is everything went well,
	 *         EALREADYSTARTED if the module has already been started.
	 */
	static int8_t configureAdcTriggerSource(uint8_t adc_number, adc_src_t trigger_source);

	/**
	 * This function is used to configure all ADC channels in default configuration.
	 * Channels will be attributed as follows:
 	 * ADC1 -   V1_LOW      ADC2 -  I1_LOW
 	 *          V2_LOW              I2_LOW
 	 *          V_HIGH              I_HIGH
	 *
	 * This function must be called BEFORE dataAcquisition.start().
	 *
	 * @return 0 is everything went well,
	 *         EALREADYSTARTED if the module has already been started.
	 */
	static int8_t configureAdcDefaultAllMeasurements();

	/**
	 * This functions starts the acquisition chain. It must be called
	 * after all module configuration has been carried out. No
	 * configuration change is allowed after module has been started.
	 *
	 * @return 0 is everything went well,
	 *         EUNITITIALIZED if the module has not been initialized,
	 *         EALREADYSTARTED if the module has already been started,
	 *         ECHANUNCONF if the channel configuration has not been done.
	 */
	static int8_t start();


	/////
	// Accessor API

	/**
	 * This function returns the name of an enabled channel.
	 *
	 * This function must be called AFTER data_acquisition_init()
	 * and AFTER data_acquisition_configure_adc_channels() function
	 * has been called for this channel.
	 *
	 * @param  adc_number Number of the ADC
	 * @param  channel_rank Rank of the ADC channel to query.
	 *         Rank ranges from 0 to (number of enabled channels)-1
	 * @return Name of the channel as defined in the device tree, or
	 *         NULL if channel configuration has not been made or
	 *         channel_rank is over (number of enabled channels)-1.
	 */
	static const char* getChannelName(uint8_t adc_number, uint8_t channel_rank);

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

	/**
	 * These functions return the latest acquired measure expressed
	 * in the relevant unit for the data: Volts, Amperes, Degree Celcius.
	 *
	 * @return Latest acquired measure for the channel.
	 *
	 * NOTE: When using these functions for a channel, you loose the
	 *       abiility to access raw values using get***RawValues() functions,
	 *       as get***() functions clear the buffers on each call.
	 */
	static float32_t getV1Low();
	static float32_t getV2Low();
	static float32_t getVHigh();
	static float32_t getI1Low();
	static float32_t getI2Low();
	static float32_t getIHigh();
	static float32_t getTemperature();

	/**
	 * Use these functions to convert values obtained using
	 * get***RawValues() functions to relevant unit for the data:
	 * Volts, Amperes, Degree Celcius.
	 */
	static float32_t convertV1Low(uint16_t raw_value);
	static float32_t convertV2Low(uint16_t raw_value);
	static float32_t convertI1Low(uint16_t raw_value);
	static float32_t convertVHigh(uint16_t raw_value);
	static float32_t convertI2Low(uint16_t raw_value);
	static float32_t convertIHigh(uint16_t raw_value);
	static float32_t convertTemperature(uint16_t raw_value);

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


private:
	typedef struct
	{
		uint8_t adc_number;
		uint8_t channel_rank;
	} channel_assignment_t;

private:
	static uint8_t initialized;
	static uint8_t channels_configured;
	static uint8_t started;

	static channel_assignment_t v1_low_assignement;
	static channel_assignment_t v2_low_assignement;
	static channel_assignment_t v_high_assignement;
	static channel_assignment_t i1_low_assignement;
	static channel_assignment_t i2_low_assignement;
	static channel_assignment_t i_high_assignement;
	static channel_assignment_t temp_sensor_assignement;

};


/////
// Public object to interact with the class

extern DataAcquisition dataAcquisition;



#endif // DATAACQUISITION_H_
