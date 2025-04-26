/*
 * Copyright (c) 2023-present LAAS-CNRS
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

/*
 * @date   2024
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 *
 * @brief  Shield sensors management from device tree.
 *         This class allows, for shields that define
 *         shield-sensor nodes in device tree, to
 *         automatically extract available sensors and
 *         manage them by name using an enumeration.
 */


#ifndef SENSORS_H_
#define SENSORS_H_


/* Stdlib */
#include <stdint.h>

/* Zephyr */
#include <zephyr/kernel.h>

/* ARM CMSIS library */
#include <arm_math.h>

/* Other modules public API */
#include "SpinAPI.h"

/* Device-tree related macro */

#define SENSOR_TOKEN(node_id) DT_STRING_TOKEN(node_id, sensor_name),


/* Type definitions */

typedef enum
{
	UNDEFINED_SENSOR = 0,
	DT_FOREACH_STATUS_OKAY(shield_sensors, SENSOR_TOKEN)
} sensor_t;

struct sensor_info_t
{
	sensor_info_t(adc_t adc_num, uint8_t channel_num, uint8_t pin_num)
	{
		this->adc_num     = adc_num;
		this->channel_num = channel_num;
		this->pin_num     = pin_num;
	}

	adc_t   adc_num;
	uint8_t channel_num;
	uint8_t pin_num;
};

#ifdef CONFIG_SHIELD_OWNVERTER
	typedef enum
	{
		TEMP_1 = 0,
		TEMP_2 = 1,
		TEMP_3 = 2
	} ownverter_temp_sensor_t;
#endif

/* Static class definition */

class SensorsAPI
{

/**
 *  Private types definitions
 */

private:
	typedef union
	{
		uint32_t  raw_value;
		float32_t float_value;
	} int2float;

	enum conv_type_string_t
	{
		LINEAR,
		THERMISTANCE
	};

	typedef struct
	{
		sensor_t           name;
		uint8_t            adc_number;
		uint8_t            channel_number;
		uint8_t            pin_number;
		bool               is_differential;
		uint32_t           adc_reg_addr; /* ADC addr is used to identify ADC */
		conv_type_string_t conversion_type;
		/* Default calibration parameters */
		int2float default_gain;
		int2float default_offset;
		int2float default_r0;
		int2float default_b;
		int2float default_rdiv;
		int2float default_t0;
	} sensor_dt_data_t;


public:

	/**
	 * @brief This function is used to enable a shield sensor for acquisition
	 *        by a given ADC.
	 *
	 * @note  This function requires the presence of an "shield-sensor" node
	 * 		  in the shield device-tree.
	 *
	 * @note  This function must be called `before` ADC is started.
	 *
	 * @param[in] sensor_name Name of the sensor using enumeration sensor_t.
	 * @param[in] adc_number The ADC which should be used for acquisition.
	 *
	 * @return 0 if the sensor was correctly enabled, negative value
	 * 		   if there was an error.
	 */
	int8_t enableSensor(sensor_t sensor_name, adc_t adc_number);

	/**
	 * @brief Function to access the acquired data for specified sensor.
	 * 
	 *        This function provides a buffer in which all data that
	 *        have been acquired since last call are stored. 
	 * 
	 * 		  The count of these values is returned as an output parameter.
	 *  
	 * 		  As such, the user has to define a variable and pass it as the 
	 * 		  parameter of the function. 
	 * 
	 * 		  The variable will be updated with the number of values that are 
	 * 		  available in the buffer.
	 *
	 * @note  This function can NOT be called before the sensor is enabled
	 *        and the DataAPI module is started, either explicitly
	 *        or by starting the Uninterruptible task.
	 *
	 * @note  When calling this function, it invalidates the buffer
	 *        returned by a previous call to the same function.
	 * 
	 *        However, different sensors buffers are independent
	 *        from each other.
	 *
	 * @note  When using this functions, the user is responsible for data
	 *        conversion. 
	 * 
	 * 		  Use matching spin.data.convert*() function for this purpose.
	 *
	 * @note  When using this function, DO NOT use the function to get the
	 *        latest converted value for the same sensor as this function
	 *        will clear the buffer and disregard all values but the latest.
	 *
	 * @param[in]  sensor_name Name of the shield sensor from which
	 * 						   to obtain values.
	 * @param[out] number_of_values_acquired Pass an `uint32_t` variable.
	 *             
	 * 			   This variable will be updated with the number of values that
	 *             are present in the returned buffer.
	 *
	 * @return Pointer to a buffer in which the acquired values are stored.
	 *         If number_of_values_acquired is 0, do not try to access the
	 *         buffer as it may be nullptr.
	 */
	uint16_t* getRawValues(sensor_t sensor_name,
						   uint32_t& number_of_values_acquired);

	/**
	 * @brief Function to access the acquired data for specified pin.
	 * 
	 *        This function converts all values that have been acquired
	 *        since last call are stored and provide an array containing
	 *        all of them. 
	 * 		  
	 * 		  The count of these values is returned as an output parameter, 
	 * 	 	  as such the user has to define a variable and pass
	 *        it as the parameter of the function. 
	 * 
	 * 		  The variable will be updated with the number of values that 
	 * 		  are available in the buffer.
	 *
	 * @warning This is an expensive function. 
	 * 
	 * 		    Calling this function trigger the conversion of all values 
	 * 			acquired since the last call.
	 *          
	 * 			If only the latest value is required, it is advised to call
	 *          getLatestValue() instead. 
	 * 			
	 * 			If multiple values are required, but not all, it is advised to 
	 * 			call getRawValues() instead, then explicitly convert required 
	 * 			values using convertValue().
	 *
	 * @note  This function can NOT be called before the pin is enabled.
	 * 
	 *        The DataAPI module must have been started, either
	 *        explicitly or by starting the Uninterruptible task.
	 *
	 * @note  When calling this function, it invalidates the array
	 *        returned by a previous call to the same function.
	 * 
	 *        However, different channels buffers are independent
	 *        from each other.
	 *
	 * @param[in]  sensor_name Name of the shield sensor from which
	 * 						   to obtain values.
	 * @param[out] number_of_values_acquired Pass an `uint32_t` variable.
	 *             This variable will be updated with the number of values that
	 *             are present in the returned buffer.
	 *
	 * @return Pointer to an array in which the acquired values are stored.
	 *         
	 * 		   If number_of_values_acquired is `0`, do not try to access the
	 *         buffer as it may be nullptr.
	 */
	float32_t* getValues(sensor_t sensor_name,
						 uint32_t& number_of_values_acquired);

	/**
	 * @brief Function to access the latest value available from the sensor.
	 * 			
	 * 		  Values are expressed in the relevant unit for the data: Volts, 
	 * 		  Amperes, or Degree Celsius. 
	 * 
	 * 		  This function will not touch anything in the buffer, and thus can 
	 * 		  be called safely at any time after the module has been started.
	 *
	 * @note  This function can NOT be called before the sensor is enabled
	 *        and the DataAPI module is started, either explicitly
	 *        or by starting the Uninterruptible task.
	 *
	 * @param[in] sensor_name Name of the shield sensor from which to obtain value.
	 *
	 * @return Latest available value available from the given sensor.
	 *         If there was no value acquired by this sensor yet,
	 *         return value is `NO_VALUE`.
	 */
	float32_t peekLatestValue(sensor_t sensor_name);

	/**
	 * @brief This function returns the latest acquired measure expressed
	 *        in the relevant unit for the sensor: Volts, Amperes, or
	 *        Degree Celsius.
	 *
	 * @note  This function can NOT be called before the sensor is enabled
	 *        and the DataAPI module is started, either explicitly
	 *        or by starting the Uninterruptible task.
	 *
	 * @note  When using this functions, you loose the ability to access raw
	 *        values using spin.data.get*RawValues() function for the
	 *        matching sensor, as spin.data.get*() function clears the
	 *        buffer on each call.
	 *
	 * @param sensor_name Name of the shield sensor from which to obtain value.
	 * @param dataValid Pointer to an `uint8_t` variable. 
	 * 		
	 * This parameter is optional. 
	 * 
	 * If this parameter is provided, it will be updated
	 * to indicate information about spin.data. 
	 * 
	 * Possible values for this parameter will be:
	 * 
	 * - `DATA_IS_OK` if returned data is a newly acquired data,
	 * 
	 * - `DATA_IS_OLD` if returned data has already been provided before
	 * (no new data available since latest time this function was called),
	 * 
	 * - `DATA_IS_MISSING` if returned data is `NO_VALUE`.
	 *
	 * @return Latest measure acquired by the sensor.
	 * 
	 * If no value was acquired by this sensor yet, return value is `NO_VALUE`.
	 *
	 */
	float32_t getLatestValue(sensor_t sensor_name, uint8_t* dataValid = nullptr);

	/**
	 * @brief Use this function to convert values obtained using matching
	 *        spin.data.get*RawValues() function.
	 * 	
	 * 		  Conversion will be done to relevant unit for the data: 
	 * 		  Volts, Amperes, or Degree Celsius.
	 *
	 * @note  This function can NOT be called before the sensor is enabled.
	 *
	 * @param[in] sensor_name Name of the shield sensor from which the value originates
	 * @param[in] raw_value Raw value obtained from which the value originates
	 *
	 * @return Converted value in the relevant unit. 
	 * 
	 * Returns `ERROR_CHANNEL_NOT_FOUND` if the sensor is not active.
	 */
	float32_t convertRawValue(sensor_t sensor_name, uint16_t raw_value);

	/**
	 * @brief Use this function to tweak the conversion values for any linear
	 *        sensor if default values are not accurate enough.
	 *
	 * @note  This function can NOT be called before the sensor is enabled.
	 * 
	 *        The DataAPI must not have been started, neither explicitly
	 *        nor by starting the Uninterruptible task.
	 *
	 * @param[in] sensor_name Name of the shield sensor to set conversion values.
	 * @param[in] gain Gain to be applied (multiplied) to the sensor raw value.
	 * @param[in] offset Offset to be applied (added) to the sensor value
	 *            after gain has been applied.
	 */
	void setConversionParametersLinear(sensor_t sensor_name,
									   float32_t gain,
									   float32_t offset);

	/**
	 * @brief Use this function to set the conversion values for any NTC
	 * 		  thermistor sensor if default values are not accurate enough.
	 *
	 * @note  This function can NOT be called before the sensor is enabled.
	 * 
	 *        The DataAPI must not have been started, neither explicitly
	 *        nor by starting the Uninterruptible task.
	 *
	 * @param[in] sensor_name Name of the shield sensor to set conversion values.
	 * @param[in] r0 The NTC resistance at a reference temperature.
	 * @param[in] b The sensibility coefficient of the resistance to temperature.
	 * @param[in] rdiv The bridge divider resistance used to condition the NTC.
	 * @param[in] t0 The reference temperature of the thermistor.
	 */
	void setConversionParametersNtcThermistor(sensor_t sensor_name,
											  float32_t r0,
											  float32_t b,
											  float32_t rdiv,
											  float32_t t0);

	/**
	 * @brief Use this function to get the current conversion parameters for the chosen sensor.
	 *
	 * @note  This function can NOT be called before the sensor is enabled.
	 *
	 * @param[in] sensor_name Name of the shield sensor to get a conversion parameter.
	 * @param[in] parameter_name Paramater to be retrieved: `gain` or `offset`.
	 */
	float32_t retrieveStoredParameterValue(sensor_t sensor_name,
										   parameter_t parameter_name);

	/**
	 * @brief Use this function to get the current conversion type for the chosen sensor.
	 *
	 * @note  This function can NOT be called before the sensor is enabled.
	 *
	 * @param[in] sensor_name Name of the shield sensor to get a conversion parameter.
	 */
	conversion_type_t retrieveStoredConversionType(sensor_t sensor_name);

	/**
	 * @brief Use this function to write the gain and offset parameters
	 * 		  of the board to is non-volatile memory.
	 *
	 * @note  This function should be called after updating the parameters
	 * 		  using setParameters.
	 *
	 * @param[in] sensor_name Name of the shield sensor to save the values.
	 */
	int8_t storeParametersInMemory(sensor_t sensor_name);

	/**
	 * @brief Use this function to read the gain and offset parameters
	 * 		  of the board to is non-volatile memory.
	 *
	 * @param[in] sensor_name Name of the shield sensor to save the values.
     * @return `0` if parameters were correctly retrieved,negative value if 
	 *         there was an error:
	 * 
     * - `-1`: NVS is empty
	 * 
     * - `-2`: NVS contains data, but their version doesn't match current 
	 * 			   version
	 * 
     * - `-3`: NVS data is corrupted
	 * 
     * - `-4`: NVS contains data, but not for the requested channel
	 */
	int8_t retrieveParametersFromMemory(sensor_t sensor_name);

#ifdef CONFIG_SHIELD_OWNVERTER

	/**
	 * @brief This function is used to enable acquisition of all voltage/current
	 *        sensors on the OwnVerter shield.
	 * 
	 * @note  ADCs are triggered simultaneously.
	 * 
	 * @note  Sensors are attributed to ADC1 and ADC2 as follows: 
	 * 
	 * - `ADC1_LIST[5]`: [`V1_LOW`,`V2_LOW`, `I3_LOW`, `V_HIGH`, `V_NEUTR`    ]  
	 *      
	 * - `ADC2_LIST[5]`: [`I1_LOW`,`I2_LOW`, `V3_LOW`, `I_HIGH`, `TEMP_SENSOR`]
	 *        
	 * This function will configure ADC 1 and 2 to be automatically triggered 
	 * by the HRTIM, so the board must be configured as a power converted to 
	 * enable HRTIM events.
	 * 
	 * All other ADCs remain software triggered, thus will only be acquired 
	 * when triggerAcquisition() is called.
	 * 
	 * It also configures the gpios that control the MUX that chooses which
	 * temperature will be measured.
	 *
	 * @note  This function must be called *before* ADC is started.
	 */
	void enableDefaultOwnverterSensors();

	/**
	 * @brief This function sets the GPIOs attached to the MUX to control which
	 * 		  temperature sensor will be measured.
	 * 
	 * @param[in] temperature_sensor Name of the temperature sensor to trigger:
	 * `TEMP_1`, `TEMP_2`, `TEMP_3`
	 *
	 * @note  This function will decide which value will be read automatically
	 * 	      by the ADC2 to which the temperature of the Ownverter is linked.
	 * 
	 * The logic is: 
	 * 					    
	 * - `TEMP_1: IN1 = T  IN2 =  F`
	 * 
	 * - `TEMP_2: IN1 = F  IN2 =  T`
	 * 
	 * - `TEMP_3: IN1 = T  IN2 =  T`
	 *
	 * Please refer to the OwnVerter documentation and repository for more 
	 * details
	 * 
	 */
	void setOwnverterTempMeas(ownverter_temp_sensor_t temperature_sensor);
#endif

#ifdef CONFIG_SHIELD_TWIST

	/**
	 * @brief This function is used to enable acquisition of all voltage/current
	 *        sensors on the Twist shield.
	 * 
	 * @note  ADCs are triggered simultaneously.
	 * 
	 * @note  Sensors are attributed to ADC1 and ADC2 as follows: 
	 * 
	 * - `ADC1_LIST[3]`: [`V1_LOW`,`V2_LOW`,`V_HIGH`]  
	 *      
	 * - `ADC2_LIST[3]`: [`I1_LOW`,`I2_LOW`,`I_HIGH`]
	 *
	 * 	This function will configure ADC 1 and 2 to be automatically
	 *  triggered by the HRTIM, so the board must be configured as
	 *  a power converted to enable HRTIM events.
	 * 
	 *  All other ADCs remain software triggered, thus will only be
	 *  acquired when triggerAcquisition() is called.
	 * 
	 *
	 * @warning  This function must be called `before` ADC is started.
	 */
	void enableDefaultTwistSensors();

	/**
	 * @brief Manually set parameters values using console. You will be directed
	 * 		  via console to input the parameters of each sensor of the Twist
	 * 		  board.
	 * 
	 * 		  After the parameters have been set, they will be stored in
	 *        Spin Non-Volatile memory so that they are automatically applied on
	 *        subsequent boots.
	 *
	 * @note  This function requires a console to interact with the user.
	 * 
	 *        The board must be connected to a computer using USB to display
	 *        the console.
	 *
	 * @note  This function can NOT be called before *all* Twist sensors have
	 *        been enabled (you can use enableDefaultTwistSensors() for that
	 *        purpose). 
	 * 
	 * 		  The DataAPI must not have been started, neither explicitly nor 
	 *        by starting the Uninterruptible task.
	 */
	void setTwistSensorsUserCalibrationFactors();

	/**
	 * @brief Manually triggers the temperature measurement of the Twist board.
	 *
	 * @param[in] temperature_sensor Name of the temperature sensor to trigger:
	 *                               `TEMP_SENSOR_1`, `TEMP_SENSOR_2`
	 *
	 * @note  This function must be called to trigger a conversion of the ADC
	 * 		  to which the sensor is linked. 
	 * 
	 * 		  It must be called `BEFORE` reading a new measurement. 
	 * 
	 * 		  Account for delays in the measurement.
	 */
	void triggerTwistTempMeas(sensor_t temperature_sensor);

#endif

private:


	/**
	 * @brief  This function returns a structure containing information about
	 *         an enabled sensor from a sensor name.
	 *
	 * @param[in] sensor_name Name of the sensor as defined in the device tree.
	 *
	 * @return Structure for the given sensor name containing : 
	 * 			
	 * 		   - the ADC number 
	 * 
	 *         - channel number 
	 * 
	 *         - pin number 
	 * 
	 * 		   or: 
	 * 
	 * 			- `(0, 0, 0)` if sensor name does not exist or has not been 
	 * 		   configured.
	 *
	 */
	sensor_info_t getEnabledSensorInfo(sensor_t sensor_name);

	/**
	 * @brief    Builds the list of device-tree defined sensors for each ADC.
	 */
	void buildSensorListFromDeviceTree();

	/**
	 * @brief Function to retrieve a line from console.
	 */
	void getLineFromConsole(char* buffer, uint8_t buffer_size);

	/**
	 * @brief Function to ask user about a coefficient.
	 */
	float32_t getCalibrationCoefficients(const char* physicalParameter,
										 const char* gainOrOffset);

private:
	static sensor_dt_data_t dt_sensors_props[];
	static uint8_t available_sensors_count[ADC_COUNT];
	static sensor_dt_data_t** available_sensors_props[ADC_COUNT];
	static sensor_dt_data_t* enabled_sensors[];
	static bool initialized;

	#ifdef CONFIG_SHIELD_OWNVERTER
	static uint8_t   temp_mux_in_1;
	static uint8_t   temp_mux_in_2;

	#endif

};

#endif /* SENSORS_H */
