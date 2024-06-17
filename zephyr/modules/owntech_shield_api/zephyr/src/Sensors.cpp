/*
 * Copyright (c) 2021-2024 LAAS-CNRS
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
 * @author Hugues Larrive <hugues.larrive@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Thomas Walter <thomas.walter@laas.fr>
 */



// Stdlib
#include <stdlib.h>

// Zephyr headers
#include <zephyr/console/console.h>

// Current class header
#include "Sensors.h"

// Other modules public API
#include "SpinAPI.h"



/////
// Device-tree related macros

#define SENSOR_NAME(node_id)     DT_STRING_TOKEN(node_id, sensor_name)
#define CHANNEL_IS_DIFF(node_id) DT_PROP(node_id, differential)
#define CHANNEL_NUMBER(node_id)  DT_PHA_BY_IDX(node_id, io_channels, 0, input)
#define PIN_NUMBER(node_id)      DT_PROP(node_id, spin_pin)
#define ADC_REG_ADDR(node_id)    DT_REG_ADDR(DT_PHANDLE(node_id, io_channels))
#define SENSOR_GAIN(node_id)     DT_PROP(node_id, default_gain)
#define SENSOR_OFFSET(node_id)   DT_PROP(node_id, default_offset)

// Sensor properties
#define SENSOR_WRITE_PROP(node_id)                                       \
	{                                                                    \
		.name=SENSOR_NAME(DT_PARENT(node_id)),                           \
		.channel_number=CHANNEL_NUMBER(node_id),                         \
		.pin_number=PIN_NUMBER(node_id),                                 \
		.is_differential=CHANNEL_IS_DIFF(node_id),                       \
		.adc_reg_addr=ADC_REG_ADDR(node_id),                             \
		.default_gain={.raw_value = SENSOR_GAIN(DT_PARENT(node_id))},    \
		.default_offset={.raw_value = SENSOR_OFFSET(DT_PARENT(node_id))} \
	},

#define SUBSENSOR_WRITE_PROP(node_id) DT_FOREACH_CHILD(node_id, SENSOR_WRITE_PROP)


// Sensors count. This is very dirty!
#define SENSORS_COUNTER(node_id) +1
#define SUBSENSORS_COUNTER(node_id) DT_FOREACH_CHILD(node_id, SENSORS_COUNTER)
#define DT_SENSORS_COUNT DT_FOREACH_STATUS_OKAY(shield_sensors, SUBSENSORS_COUNTER)




/////
// Variables

// Auto-populated array containing available sensors
// extracted from the device tree.
SensorsAPI::sensor_prop_t SensorsAPI::dt_sensors_props[] =
{
	DT_FOREACH_STATUS_OKAY(shield_sensors, SUBSENSOR_WRITE_PROP)
};

// Number of available sensors defined in device tree for each ADC.
uint8_t SensorsAPI::available_sensors_count[ADC_COUNT] = {0};

// List of available sensors containing 1 array for each ADC.
// Each array contains pointers to sensors definitions in
// available_sensors_props array.
// For each ADC, the array size will match the value of
// available_sensors_count for the ADC.
SensorsAPI::sensor_prop_t** SensorsAPI::available_sensors_props[ADC_COUNT] = {0};

// List of sensors enabled by user configuration.
// For each sensor, a nullptr indicates it has not been
// enabled, and a valid pointer will point to the structure
// containing relevant information for this sensor.
SensorsAPI::sensor_prop_t* SensorsAPI::enabled_sensors[DT_SENSORS_COUNT] = {0};

bool SensorsAPI::initialized = false;



/////
// Public functions accessible only when using Twist


int8_t SensorsAPI::enableSensor(sensor_t sensor_name, uint8_t adc_num)
{
	if (initialized == false)
	{
		buildSensorListFromDeviceTree();
	}

	// Check parameters
	if (adc_num > ADC_COUNT) return ERROR_CHANNEL_NOT_FOUND;
	if (sensor_name == UNDEFINED_SENSOR) return ERROR_CHANNEL_NOT_FOUND;

	// Find sensor property
	uint8_t adc_index = adc_num-1;
	sensor_prop_t* sensor_prop = nullptr;
	for (uint8_t sensor = 0 ; sensor < available_sensors_count[adc_index] ; sensor++)
	{
		sensor_prop_t* current_sensor = available_sensors_props[adc_index][sensor];
		if (current_sensor->name == sensor_name)
		{
			sensor_prop = current_sensor;
		}
	}

	// Check if we did find a sensor
	if (sensor_prop == nullptr) return ERROR_CHANNEL_NOT_FOUND;

	// Register sensor enabling
	int sensor_index = ((int)sensor_name) - 1;
	enabled_sensors[sensor_index] = sensor_prop;


	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return DataAPI::enableChannel(sensor_info.adc_num, sensor_info.channel_num);
}

uint16_t* SensorsAPI::getRawValues(sensor_t sensor_name, uint32_t& number_of_values_acquired)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return DataAPI::getChannelRawValues(sensor_info.adc_num, sensor_info.channel_num, number_of_values_acquired);
}

float32_t SensorsAPI::peekLatestValue(sensor_t sensor_name)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return DataAPI::peekChannel(sensor_info.adc_num, sensor_info.channel_num);
}

float32_t SensorsAPI::getLatestValue(sensor_t sensor_name, uint8_t* dataValid)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return DataAPI::getChannelLatest(sensor_info.adc_num, sensor_info.channel_num, dataValid);
}

float32_t SensorsAPI::convertRawValue(sensor_t sensor_name, uint16_t raw_value)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return data_conversion_convert_raw_value(sensor_info.adc_num, sensor_info.channel_num, raw_value);
}

void SensorsAPI::setConversionParameters(sensor_t sensor_name, float32_t gain, float32_t offset)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	data_conversion_set_conversion_parameters_linear(sensor_info.adc_num, sensor_info.channel_num, gain, offset);
}

float32_t SensorsAPI::retrieveStoredParameterValue(sensor_t sensor_name, parameter_t parameter_name)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return data_conversion_get_parameter(sensor_info.adc_num, sensor_info.channel_num, parameter_name);

}

conversion_type_t SensorsAPI::retrieveStoredConversionType(sensor_t sensor_name)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return data_conversion_get_conversion_type(sensor_info.adc_num, sensor_info.channel_num);
}

int8_t SensorsAPI::retrieveParametersFromMemory(sensor_t sensor_name)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return data_conversion_retrieve_channel_parameters_from_nvs(sensor_info.adc_num, sensor_info.channel_num);
}

int8_t SensorsAPI::storeParametersInMemory(sensor_t sensor_name)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return data_conversion_store_channel_parameters_in_nvs(sensor_info.adc_num, sensor_info.channel_num);
}


#ifdef CONFIG_SHIELD_TWIST

void SensorsAPI::enableDefaultTwistSensors()
{
	spin.data.configureTriggerSource(ADC_1, hrtim_ev1);
	spin.data.configureTriggerSource(ADC_2, hrtim_ev3);
	spin.data.configureTriggerSource(ADC_3, software);
	spin.data.configureTriggerSource(ADC_4, software);
	spin.data.configureTriggerSource(ADC_5, software);

	spin.data.configureDiscontinuousMode(ADC_1, 1);
	spin.data.configureDiscontinuousMode(ADC_2, 1);

	this->enableSensor(I1_LOW, 1);
	this->enableSensor(V1_LOW, 1);
	this->enableSensor(V_HIGH, 1);

	this->enableSensor(I2_LOW, 2);
	this->enableSensor(V2_LOW, 2);
	this->enableSensor(I_HIGH, 2);
}

void SensorsAPI::setTwistSensorsUserCalibrationFactors()
{
	float32_t gains[6];   // VH, V1, V2, IH, I1, I2
	float32_t offsets[6]; // VH, V1, V2, IH, I1, I2

	gains[0]   = getCalibrationCoefficients("VHigh", "gain");
	offsets[0] = getCalibrationCoefficients("VHigh", "offset");
	gains[1]   = getCalibrationCoefficients("V1Low", "gain");
	offsets[1] = getCalibrationCoefficients("V1Low", "offset");
	gains[2]   = getCalibrationCoefficients("V2Low", "gain");
	offsets[2] = getCalibrationCoefficients("V2Low", "offset");
	gains[3]   = getCalibrationCoefficients("IHigh", "gain");
	offsets[3] = getCalibrationCoefficients("IHigh", "offset");
	gains[4]   = getCalibrationCoefficients("I1Low", "gain");
	offsets[4] = getCalibrationCoefficients("I1Low", "offset");
	gains[5]   = getCalibrationCoefficients("I2Low", "gain");
	offsets[5] = getCalibrationCoefficients("I2Low", "offset");

	sensor_info_t sensor_info = getEnabledSensorInfo(V_HIGH);
	data_conversion_set_conversion_parameters_linear(sensor_info.adc_num, sensor_info.channel_num, gains[0], offsets[0]);

	sensor_info = getEnabledSensorInfo(V1_LOW);
	data_conversion_set_conversion_parameters_linear(sensor_info.adc_num, sensor_info.channel_num, gains[1], offsets[1]);

	sensor_info = getEnabledSensorInfo(V2_LOW);
	data_conversion_set_conversion_parameters_linear(sensor_info.adc_num, sensor_info.channel_num, gains[2], offsets[2]);

	sensor_info = getEnabledSensorInfo(I_HIGH);
	data_conversion_set_conversion_parameters_linear(sensor_info.adc_num, sensor_info.channel_num, gains[3], offsets[3]);

	sensor_info = getEnabledSensorInfo(I1_LOW);
	data_conversion_set_conversion_parameters_linear(sensor_info.adc_num, sensor_info.channel_num, gains[4], offsets[4]);

	sensor_info = getEnabledSensorInfo(I2_LOW);
	data_conversion_set_conversion_parameters_linear(sensor_info.adc_num, sensor_info.channel_num, gains[5], offsets[5]);

	printk("Calibration coefficients successfully updated!\n");

	// Ask for save in NVS
	printk("Do you want to store these parameters in permanent storage?\n");
	printk("Parameters stored in permanent storage are automatically retreived at board boot.\n");
	printk("Not storing them in permanent storage will result in parameters being lost on board power cycle.\n");
	printk("Press y to store parameters in permanent storage, any other key to don't store them.\n");

	char received_char = console_getchar();
	if (received_char == 'y')
	{
		sensor_info = getEnabledSensorInfo(V_HIGH);
		int8_t err = data_conversion_store_channel_parameters_in_nvs(sensor_info.adc_num, sensor_info.channel_num);

		sensor_info = getEnabledSensorInfo(V1_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(sensor_info.adc_num, sensor_info.channel_num);

		sensor_info = getEnabledSensorInfo(V2_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(sensor_info.adc_num, sensor_info.channel_num);

		sensor_info = getEnabledSensorInfo(I_HIGH);
		err |= data_conversion_store_channel_parameters_in_nvs(sensor_info.adc_num, sensor_info.channel_num);

		sensor_info = getEnabledSensorInfo(I1_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(sensor_info.adc_num, sensor_info.channel_num);

		sensor_info = getEnabledSensorInfo(I2_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(sensor_info.adc_num, sensor_info.channel_num);

		if (err == 0)
		{
			printk("Parameters were successfully written in permanent storage.\n");
		}
		else
		{
			printk("Error writing parameters in permanent storage!\n");
		}
	}
	else
	{
		printk("Exiting without permanent storage. Parameters won't be retained after power cycling.\n");
	}
}

#endif



sensor_info_t SensorsAPI::getEnabledSensorInfo(sensor_t sensor_name)
{
	if (initialized == false)
	{
		buildSensorListFromDeviceTree();
	}

	int sensor_index = ((int)sensor_name) - 1;
	sensor_prop_t* sensor_prop = enabled_sensors[sensor_index];
	if (sensor_prop != nullptr)
	{
		return sensor_info_t((adc_t)sensor_prop->adc_number, sensor_prop->channel_number, sensor_prop->pin_number);
	}
	else
	{
		return sensor_info_t(DEFAULT_ADC, 0, 0);
	}
}

void SensorsAPI::buildSensorListFromDeviceTree()
{
	bool checkNvs = true;

	// Retreive calibration coefficients for each sensor listed in device tree
	for (uint8_t dt_sensor_index = 0 ; dt_sensor_index < DT_SENSORS_COUNT ; dt_sensor_index++)
	{
		// Determine ADC number based on its address
		switch (dt_sensors_props[dt_sensor_index].adc_reg_addr)
		{
			case 0x50000000:
				dt_sensors_props[dt_sensor_index].adc_number = 1;
				break;
			case 0x50000100:
				dt_sensors_props[dt_sensor_index].adc_number = 2;
				break;
			case 0x50000400:
				dt_sensors_props[dt_sensor_index].adc_number = 3;
				break;
			case 0x50000500:
				dt_sensors_props[dt_sensor_index].adc_number = 4;
				break;
			case 0x50000600:
				dt_sensors_props[dt_sensor_index].adc_number = 5;
				break;
			default:
				dt_sensors_props[dt_sensor_index].adc_number = 0;
				continue;
				break;
		}

		// Get parameters from NVS if they exist
		bool nvsRetrieved = false;
		if (checkNvs == true)
		{
			int8_t res = data_conversion_retrieve_channel_parameters_from_nvs(dt_sensors_props[dt_sensor_index].adc_number,
			                                                                    dt_sensors_props[dt_sensor_index].channel_number
			                                                                   );

			if (res == 0)
			{
				printk("Parameters for ADC %u channel %u have been retrieved from flash\n", dt_sensors_props[dt_sensor_index].adc_number, dt_sensors_props[dt_sensor_index].channel_number);
				conversion_type_t conv_type = data_conversion_get_conversion_type(dt_sensors_props[dt_sensor_index].adc_number, dt_sensors_props[dt_sensor_index].channel_number);
				switch (conv_type)
				{
					case conversion_linear:
					{
						float32_t gain   = data_conversion_get_parameter(dt_sensors_props[dt_sensor_index].adc_number, dt_sensors_props[dt_sensor_index].channel_number, 1);
						float32_t offset = data_conversion_get_parameter(dt_sensors_props[dt_sensor_index].adc_number, dt_sensors_props[dt_sensor_index].channel_number, 2);
						printk("    Conversion type is linear, with gain=%f and offset=%f\n", gain, offset);
					}
					break;
					case no_channel_error:
						continue;
				}
				nvsRetrieved = true;
			}
			else if (res == -1)
			{
				printk("No calibration value found in persistent storage. Default values will be used for data conversion.\n");
				checkNvs = false;
			}
			else if (res == -2)
			{
				printk("Calibration values in persistent storage were stored with a previous version of the API and can't be recovered. Default values will be used for data conversion.\n");
				checkNvs = false;
			}
			else if (res == -3)
			{
				printk("Calibration values for ADC %u channel %u were found in persistent storage, but their format is incorrect. Possible data corruption.\n",
				       dt_sensors_props[dt_sensor_index].adc_number,
				       dt_sensors_props[dt_sensor_index].channel_number
				      );
			}
			else if (res == -4)
			{
				printk("Unable to find calibration values for ADC %u channel %u in persistent storage. Default values will be used.\n",
				       dt_sensors_props[dt_sensor_index].adc_number,
				       dt_sensors_props[dt_sensor_index].channel_number
				      );
			}
		}

		if (nvsRetrieved == false)
		{
			// In case parameters were not found in NVS, get default vaules from device tree
			data_conversion_set_conversion_parameters_linear(dt_sensors_props[dt_sensor_index].adc_number,
		                                                     dt_sensors_props[dt_sensor_index].channel_number,
		                                                     dt_sensors_props[dt_sensor_index].default_gain.float_value,
		                                                     dt_sensors_props[dt_sensor_index].default_offset.float_value
		                                                    );
		}

		// Count sensor for ADC
		uint8_t adc_index = dt_sensors_props[dt_sensor_index].adc_number - 1;
		available_sensors_count[adc_index]++;
	}

	// Create the channels list for each ADC
	for (uint8_t adc_index = 0 ; adc_index < ADC_COUNT ; adc_index++)
	{
		available_sensors_props[adc_index] = (sensor_prop_t**)k_malloc(sizeof(sensor_prop_t*) * available_sensors_count[adc_index]);
	}

	// Populate the channels list for each ADC
	uint8_t adc_channels_count[ADC_COUNT] = {0};
	for (uint8_t dt_sensor_index = 0 ; dt_sensor_index < DT_SENSORS_COUNT ; dt_sensor_index++)
	{
		uint8_t adc_index = dt_sensors_props[dt_sensor_index].adc_number - 1;
		if (adc_index < ADC_COUNT)
		{
			uint8_t& current_adc_channels_count = adc_channels_count[adc_index];
			available_sensors_props[adc_index][current_adc_channels_count] = &dt_sensors_props[dt_sensor_index];
			current_adc_channels_count++;
		}
	}

	initialized = true;
}


void SensorsAPI::getLineFromConsole(char* buffer, uint8_t buffer_size)
{
	//Initializing variables for eventual loop
	uint8_t carcount = 0;
	char received_char;

	do
	{
		received_char = console_getchar();
		buffer[carcount] = received_char;

		if (received_char == 0x08) // Backspace character
		{
			if (carcount>0) // To avoid carcount being negative
			{
				carcount--;
			}
		}
		else
		{
			carcount++;
		}

		printk("%c", received_char); // Echo received char

		if (carcount >= (buffer_size-1))
		{
			printk("Maximum caracter allowed reached \n");
			break;
		}

	} while ((received_char!='\n')); // EOL char : CRLF
	buffer[carcount-2] = '\0'; // adding end of tab character to prepare for atof function
}

float32_t SensorsAPI::getCalibrationCoefficients(const char* physicalParameter, const char* gainOrOffset)
{
	// Maximum number of number for gain and offset value
	const uint8_t MaxCharInOneLine = 20;

	char received_char;
	char line[MaxCharInOneLine]; // Number of character in one line
	float32_t parameterCoefficient;

	do
	{
		printk("Type %s %s and press enter \n", physicalParameter, gainOrOffset);
		getLineFromConsole(line, MaxCharInOneLine);

		// Convert string to float
		parameterCoefficient = atof(line);

		// Get confirmation
		printk("%s %s applied will be : %f\n", physicalParameter, gainOrOffset, parameterCoefficient);
		printk("Press y to validate, any other character to retype the %s \n", gainOrOffset);
		received_char = console_getchar();

	} while(received_char != 'y');

	return parameterCoefficient;
}
