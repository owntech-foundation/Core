/*
 * Copyright (c) 2021-present LAAS-CNRS
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

/* Stdlib */
#include <stdlib.h>

/* Zephyr headers */
#include <zephyr/console/console.h>

/* Current class header */
#include "Sensors.h"

/* Other modules public API */
#include "SpinAPI.h"

/**
 *  Device-tree related macros
 */

#define SENSOR_NAME(node_id) \
				DT_STRING_TOKEN(DT_PARENT(node_id), sensor_name)

#define CHANNEL_IS_DIFF(node_id) \
				DT_PROP(node_id, differential)

#define CHANNEL_NUMBER(node_id) \
				DT_PHA_BY_IDX(node_id, io_channels, 0, input)

#define PIN_NUMBER(node_id) \
				DT_PROP(node_id, spin_pin)

#define ADC_REG_ADDR(node_id) \
				DT_REG_ADDR(DT_PHANDLE(node_id, io_channels))

#define CONVERSION_TYPE(node_id) \
				DT_STRING_TOKEN(DT_PARENT(node_id), sensor_conv_type)

#define SENSOR_DEFAULT_PARAM(node_id, param_name) \
				DT_PROP_OR(DT_PARENT(node_id), default_##param_name, 0)

/* Sensor properties */
#define SENSOR_WRITE_PROP(node_id)                                            \
	{                                                                         \
		.name=SENSOR_NAME(node_id),                                           \
		.channel_number=CHANNEL_NUMBER(node_id),                              \
		.pin_number=PIN_NUMBER(node_id),                                      \
		.is_differential=CHANNEL_IS_DIFF(node_id),                            \
		.adc_reg_addr=ADC_REG_ADDR(node_id),                                  \
		.conversion_type=CONVERSION_TYPE(node_id),                            \
		.default_gain={.raw_value = SENSOR_DEFAULT_PARAM(node_id, gain)},     \
		.default_offset={.raw_value = SENSOR_DEFAULT_PARAM(node_id, offset)}, \
		.default_r0={.raw_value = SENSOR_DEFAULT_PARAM(node_id, r0)},         \
		.default_b={.raw_value = SENSOR_DEFAULT_PARAM(node_id, b)},           \
		.default_rdiv={.raw_value = SENSOR_DEFAULT_PARAM(node_id, rdiv)},     \
		.default_t0={.raw_value = SENSOR_DEFAULT_PARAM(node_id, t0)}          \
	},

#define SUBSENSOR_WRITE_PROP(node_id) \
					DT_FOREACH_CHILD(node_id, SENSOR_WRITE_PROP)


/* Sensors count. This is very dirty! */
#define SENSORS_COUNTER(node_id) +1
#define SUBSENSORS_COUNTER(node_id) \
					DT_FOREACH_CHILD(node_id, SENSORS_COUNTER)

#define DT_SENSORS_COUNT \
					DT_FOREACH_STATUS_OKAY(shield_sensors, SUBSENSORS_COUNTER)



#ifdef CONFIG_SHIELD_OWNVERTER
	uint8_t SensorsAPI::temp_mux_in_1 =
							DT_PROP(DT_NODELABEL(temp), mux_spin_pin_1);

	uint8_t SensorsAPI::temp_mux_in_2 =
							DT_PROP(DT_NODELABEL(temp), mux_spin_pin_2);
#endif


/**
 *  Variables
 */

/**
 * Auto-populated array containing available sensors extracted
 * from the device tree.
 */
SensorsAPI::sensor_dt_data_t SensorsAPI::dt_sensors_props[] =
{
	DT_FOREACH_STATUS_OKAY(shield_sensors, SUBSENSOR_WRITE_PROP)
};

/* Number of available sensors defined in device tree for each ADC. */
uint8_t SensorsAPI::available_sensors_count[ADC_COUNT] = {0};

/**
 * List of available sensors containing 1 array for each ADC.
 * Each array contains pointers to sensors definitions in
 * available_sensors_props array.
 * For each ADC, the array size will match the value of
 * available_sensors_count for the ADC.
 */
SensorsAPI::sensor_dt_data_t** SensorsAPI::available_sensors_props[ADC_COUNT] = {0};

/** List of sensors enabled by user configuration.
 * For each sensor, a nullptr indicates it has not been
 * enabled, and a valid pointer will point to the structure
 * containing relevant information for this sensor.
 */
SensorsAPI::sensor_dt_data_t* SensorsAPI::enabled_sensors[DT_SENSORS_COUNT] = {0};

bool SensorsAPI::initialized = false;


/**
 *  Public functions accessible only when using a power shield
 */

int8_t SensorsAPI::enableSensor(sensor_t sensor_name, adc_t adc_num)
{
	if (initialized == false)
	{
		buildSensorListFromDeviceTree();
	}

	/* Check parameters */
	if (adc_num > ADC_COUNT) return ERROR_CHANNEL_NOT_FOUND;
	if (sensor_name == UNDEFINED_SENSOR) return ERROR_CHANNEL_NOT_FOUND;

	/* Find sensor property */
	uint8_t adc_index = adc_num-1;
	sensor_dt_data_t* sensor_prop = nullptr;
	for (uint8_t sensor = 0 ;
		 sensor < available_sensors_count[adc_index];
		 sensor++)
	{
		sensor_dt_data_t* current_sensor =
							available_sensors_props[adc_index][sensor];

		if (current_sensor->name == sensor_name)
		{
			sensor_prop = current_sensor;
		}
	}

	/* Check if we did find a sensor */
	if (sensor_prop == nullptr) return ERROR_CHANNEL_NOT_FOUND;

	/* Register sensor enabling */
	int sensor_index = ((int)sensor_name) - 1;
	enabled_sensors[sensor_index] = sensor_prop;


	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return DataAPI::enableChannel(sensor_info.adc_num, sensor_info.channel_num);
}

uint16_t* SensorsAPI::getRawValues(sensor_t sensor_name,
								   uint32_t& number_of_values_acquired)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);

	return DataAPI::getChannelRawValues(sensor_info.adc_num,
										sensor_info.channel_num,
										number_of_values_acquired);
}

float32_t* SensorsAPI::getValues(sensor_t sensor_name,
								 uint32_t& number_of_values_acquired)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);

	return DataAPI::getChannelValues(sensor_info.adc_num,
									 sensor_info.channel_num,
									 number_of_values_acquired);
}

float32_t SensorsAPI::peekLatestValue(sensor_t sensor_name)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);

	return DataAPI::peekChannel(sensor_info.adc_num,
								sensor_info.channel_num);
}

float32_t SensorsAPI::getLatestValue(sensor_t sensor_name, uint8_t* dataValid)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);

	return DataAPI::getChannelLatest(sensor_info.adc_num,
									 sensor_info.channel_num,
									 dataValid);
}

float32_t SensorsAPI::convertRawValue(sensor_t sensor_name, uint16_t raw_value)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);

	return data_conversion_convert_raw_value(sensor_info.adc_num,
											 sensor_info.channel_num,
											 raw_value);
}

void SensorsAPI::setConversionParametersLinear(sensor_t sensor_name,
											   float32_t gain,
											   float32_t offset)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	conversion_type_t sensor_conv_type =
							retrieveStoredConversionType(sensor_name);

	/* Verifies the conversion is of type linear */
	if(sensor_conv_type == conversion_linear){
		data_conversion_set_conversion_parameters_linear(
			sensor_info.adc_num,
			sensor_info.channel_num,
			gain,
			offset
		);
	}
}

void SensorsAPI::setConversionParametersNtcThermistor(sensor_t sensor_name,
													  float32_t r0,
													  float32_t b,
													  float32_t rdiv,
													  float32_t t0)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	conversion_type_t sensor_conv_type =
						retrieveStoredConversionType(sensor_name);

	if(sensor_conv_type == conversion_therm){
		data_conversion_set_conversion_parameters_therm(
			sensor_info.adc_num,
			sensor_info.channel_num,\
			r0,
			b,
			rdiv,
			t0
		);
	}
}


float32_t SensorsAPI::retrieveStoredParameterValue(sensor_t sensor_name,
												   parameter_t parameter_name)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);

	return data_conversion_get_parameter(sensor_info.adc_num,
										 sensor_info.channel_num,
										 parameter_name);
}

conversion_type_t SensorsAPI::retrieveStoredConversionType(sensor_t sensor_name)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);

	return data_conversion_get_conversion_type(sensor_info.adc_num,
											   sensor_info.channel_num);
}

int8_t SensorsAPI::retrieveParametersFromMemory(sensor_t sensor_name)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);

	return data_conversion_retrieve_channel_parameters_from_nvs(
				sensor_info.adc_num,
				sensor_info.channel_num
			);
}

int8_t SensorsAPI::storeParametersInMemory(sensor_t sensor_name)
{
	sensor_info_t sensor_info = getEnabledSensorInfo(sensor_name);
	return data_conversion_store_channel_parameters_in_nvs(
				sensor_info.adc_num,
				sensor_info.channel_num
			);
}

#ifdef CONFIG_SHIELD_OWNVERTER

void SensorsAPI::enableDefaultOwnverterSensors()
{
	/**
	 * Defines the triggers of all ADCs.
	 * 	ADC 1 - Triggered by HRTIM C, which is linked to event 3
	 * 	ADC 2 - Triggered by HRTIM A, which is linked to event 1
	 * 	ADC 3, 4 and 5 - Triggered by software
	 * 					They are mainly used for non-real-time measurements,
	 * 					such as temperature
	 */
	spin.data.configureTriggerSource(ADC_1, hrtim_ev1);
	spin.data.configureTriggerSource(ADC_2, hrtim_ev3);
	spin.data.configureTriggerSource(ADC_3, software);
	spin.data.configureTriggerSource(ADC_4, software);
	spin.data.configureTriggerSource(ADC_5, software);

	/**
	 * Defines ADC 1 and ADC 2 measurements as discontinuous.
	 * This is specially helpful for creating synchronous measurements.
	 * Each measurement is done once per period of HRTIM at a precise moment
	 */
	spin.data.configureDiscontinuousMode(ADC_1, 1);
	spin.data.configureDiscontinuousMode(ADC_2, 1);

	/* Creates the list of measurements of the ADC 1 */
	this->enableSensor(V1_LOW, ADC_1);
	this->enableSensor(V2_LOW, ADC_1);
	this->enableSensor(I3_LOW, ADC_1);
	this->enableSensor(V_HIGH, ADC_1);
	this->enableSensor(V_NEUTR, ADC_1);

	/* Creates the list of measurements of the ADC 2 */
	this->enableSensor(I1_LOW, ADC_2);
	this->enableSensor(I2_LOW, ADC_2);
	this->enableSensor(V3_LOW, ADC_2);
	this->enableSensor(I_HIGH, ADC_2);
	this->enableSensor(TEMP_SENSOR, ADC_2);

	/* Configure the pins of the temperature MUX */
	spin.gpio.configurePin(temp_mux_in_1,OUTPUT);
	spin.gpio.configurePin(temp_mux_in_2,OUTPUT);


}

void SensorsAPI::setOwnverterTempMeas(ownverter_temp_sensor_t temperature_sensor)
{
	if(temperature_sensor == TEMP_1){
		spin.gpio.setPin(temp_mux_in_1);
		spin.gpio.resetPin(temp_mux_in_2);
	}else if(temperature_sensor == TEMP_2){
		spin.gpio.resetPin(temp_mux_in_1);
		spin.gpio.setPin(temp_mux_in_2);
	}else if(temperature_sensor == TEMP_3){
		spin.gpio.setPin(temp_mux_in_1);
		spin.gpio.setPin(temp_mux_in_2);
	}
}


#endif

#ifdef CONFIG_SHIELD_TWIST

void SensorsAPI::enableDefaultTwistSensors()
{
	/**
	 * Defines the triggers of all ADCs.
	 *  ADC 1 - Triggered by HRTIM C, which is linked to event 3
	 *  ADC 2 - Triggered by HRTIM A, which is linked to event 1
	 *  ADC 3, 4 and 5 - Triggered by software
	 * 					They are mainly used for non-real-time measurements,
	 * 					such as temperature
	 */
	spin.data.configureTriggerSource(ADC_1, hrtim_ev1);
	spin.data.configureTriggerSource(ADC_2, hrtim_ev3);
	spin.data.configureTriggerSource(ADC_3, software);
	spin.data.configureTriggerSource(ADC_4, software);
	spin.data.configureTriggerSource(ADC_5, software);


	/**
	 * Defines ADC 1 and ADC 2 measurements as discontinuous.
	 * This is specially helpful for creating synchronous measurements.
	 * Each measurement is done once per period of HRTIM at a precise moment
	 */
	uint32_t num_discontinuous_meas = 1;
	spin.data.configureDiscontinuousMode(ADC_1, num_discontinuous_meas);
	spin.data.configureDiscontinuousMode(ADC_2, num_discontinuous_meas);

	/* Creates the list of measurements of the ADC 1 */
	this->enableSensor(I1_LOW, ADC_1);
	this->enableSensor(V1_LOW, ADC_1);
	this->enableSensor(V_HIGH, ADC_1);

	/* Creates the list of measurements of the ADC 2 */
	this->enableSensor(I2_LOW, ADC_2);
	this->enableSensor(V2_LOW, ADC_2);
	this->enableSensor(I_HIGH, ADC_2);

	/* Creates the list of measurements of the ADC 3 */
	int8_t test = this->enableSensor(TEMP_SENSOR_1, ADC_4);

	/* Creates the list of measurements of the ADC 4 */
	test = this->enableSensor(TEMP_SENSOR_2, ADC_3);

}

void SensorsAPI::triggerTwistTempMeas(sensor_t temperature_sensor)
{
	if(temperature_sensor == TEMP_SENSOR_1){
		 spin.data.triggerAcquisition(ADC_4);
	}else{
		 spin.data.triggerAcquisition(ADC_3);
	}
}

void SensorsAPI::setTwistSensorsUserCalibrationFactors()
{
	/* VH, V1, V2, IH, I1, I2 */
	float32_t gains[6];
	/* VH, V1, V2, IH, I1, I2 */
	float32_t offsets[6];

	/* T1 and T2 R0 - sensor resistance at reference temperature */
	float32_t r0[2];
	/* T1 and T2 B - sensor temperature negative coefficient */
	float32_t b[2];
	/* T1 and T2 R_DIV - bridge divider resistance */
	float32_t rdiv[2];
	/* T1 and T2 T0 - Reference temperature */
	float32_t t0[2];

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

	r0[0]   = getCalibrationCoefficients("Temp1", "r0");
	b[0]    = getCalibrationCoefficients("Temp1", "b");
	rdiv[0] = getCalibrationCoefficients("Temp1", "rdiv");
	t0[0]   = getCalibrationCoefficients("Temp1", "t0");

	r0[1]   = getCalibrationCoefficients("Temp2", "r0");
	b[1]    = getCalibrationCoefficients("Temp2", "b");
	rdiv[1] = getCalibrationCoefficients("Temp2", "rdiv");
	t0[1]   = getCalibrationCoefficients("Temp2", "t0");


	sensor_info_t sensor_info = getEnabledSensorInfo(V_HIGH);
	data_conversion_set_conversion_parameters_linear(
		sensor_info.adc_num,
		sensor_info.channel_num,
		gains[0],
		offsets[0]
	);

	sensor_info = getEnabledSensorInfo(V1_LOW);
	data_conversion_set_conversion_parameters_linear(
		sensor_info.adc_num,
		sensor_info.channel_num,
		gains[1],
		offsets[1]
	);

	sensor_info = getEnabledSensorInfo(V2_LOW);
	data_conversion_set_conversion_parameters_linear(
		sensor_info.adc_num,
		sensor_info.channel_num,
		gains[2],
		offsets[2]
	);

	sensor_info = getEnabledSensorInfo(I_HIGH);
	data_conversion_set_conversion_parameters_linear(
		sensor_info.adc_num,
		sensor_info.channel_num,
		gains[3],
		offsets[3]
	);

	sensor_info = getEnabledSensorInfo(I1_LOW);
	data_conversion_set_conversion_parameters_linear(
		sensor_info.adc_num,
		sensor_info.channel_num,
		gains[4],
		offsets[4]
	);

	sensor_info = getEnabledSensorInfo(I2_LOW);
	data_conversion_set_conversion_parameters_linear(
		sensor_info.adc_num,
		sensor_info.channel_num,
		gains[5],
		offsets[5]
	);

	sensor_info = getEnabledSensorInfo(TEMP_SENSOR_1);
	data_conversion_set_conversion_parameters_therm(
		sensor_info.adc_num,
		sensor_info.channel_num,
		r0[0],
		b[0],
		rdiv[0],
		t0[0]
	);

	sensor_info = getEnabledSensorInfo(TEMP_SENSOR_2);
	data_conversion_set_conversion_parameters_therm(
		sensor_info.adc_num,
		sensor_info.channel_num,
		r0[1],
		b[1],
		rdiv[1],
		t0[1]
	);

	printk("Calibration coefficients successfully updated!\n");

	/* Ask for save in NVS */
	printk("Do you want to store these parameters in permanent storage?\n");
	printk("Parameters stored in permanent storage are automatically \
			retrieved at board boot.\n");

	printk("Not storing them in permanent storage will result in parameters \
			being lost on board power cycle.\n");

	printk("Press y to store parameters in permanent storage, \
			any other key to don't store them.\n");

	char received_char = console_getchar();
	if (received_char == 'y')
	{
		sensor_info = getEnabledSensorInfo(V_HIGH);
		int8_t err = data_conversion_store_channel_parameters_in_nvs(
						sensor_info.adc_num,
						sensor_info.channel_num
					 );

		sensor_info = getEnabledSensorInfo(V1_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(
					sensor_info.adc_num,
					sensor_info.channel_num
				);

		sensor_info = getEnabledSensorInfo(V2_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(
					sensor_info.adc_num,
					sensor_info.channel_num
				);

		sensor_info = getEnabledSensorInfo(I_HIGH);
		err |= data_conversion_store_channel_parameters_in_nvs(
					sensor_info.adc_num,
					sensor_info.channel_num
				);

		sensor_info = getEnabledSensorInfo(I1_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(
					sensor_info.adc_num,
					sensor_info.channel_num
				);

		sensor_info = getEnabledSensorInfo(I2_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(
					sensor_info.adc_num,
					sensor_info.channel_num
				);

		sensor_info = getEnabledSensorInfo(TEMP_SENSOR_1);
		err |= data_conversion_store_channel_parameters_in_nvs(
					sensor_info.adc_num,
					sensor_info.channel_num
				);

		sensor_info = getEnabledSensorInfo(TEMP_SENSOR_2);
		err |= data_conversion_store_channel_parameters_in_nvs(
					sensor_info.adc_num,
					sensor_info.channel_num
				);

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
		printk("Exiting without permanent storage.\
				Parameters won't be retained after power cycling.\n");
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
	sensor_dt_data_t* sensor_prop = enabled_sensors[sensor_index];
	if (sensor_prop != nullptr)
	{
		return sensor_info_t((adc_t)sensor_prop->adc_number,
							 sensor_prop->channel_number,
							 sensor_prop->pin_number);
	}
	else
	{
		return sensor_info_t(DEFAULT_ADC, 0, 0);
	}
}

void SensorsAPI::buildSensorListFromDeviceTree()
{
	bool checkNvs = true;

	/* Retrieve calibration coefficients for each sensor listed in device tree */
	for (uint8_t dt_sensor_index = 0 ;
		 dt_sensor_index < DT_SENSORS_COUNT ;
		 dt_sensor_index++)
	{
		/* Determine ADC number based on its address */
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

		/* Get parameters from NVS if they exist */
		bool nvsRetrieved = false;
		if (checkNvs == true)
		{
			int8_t res = data_conversion_retrieve_channel_parameters_from_nvs(
							dt_sensors_props[dt_sensor_index].adc_number,
							dt_sensors_props[dt_sensor_index].channel_number
						 );

			if (res == 0)
			{
				printk("Parameters for ADC %u channel %u \
						have been retrieved from flash\n",
						dt_sensors_props[dt_sensor_index].adc_number,
						dt_sensors_props[dt_sensor_index].channel_number);

				conversion_type_t conv_type =
						data_conversion_get_conversion_type(
							dt_sensors_props[dt_sensor_index].adc_number,
							dt_sensors_props[dt_sensor_index].channel_number
						);

				switch (conv_type)
				{
					case conversion_linear:
					{
						float32_t gain   =
							data_conversion_get_parameter(
								dt_sensors_props[dt_sensor_index].adc_number,
								dt_sensors_props[dt_sensor_index].channel_number,
								1
							);

						float32_t offset =
							data_conversion_get_parameter(
								dt_sensors_props[dt_sensor_index].adc_number,
								dt_sensors_props[dt_sensor_index].channel_number,
								2
							);

						printk("    Conversion type is linear, \
							   with gain=%f and offset=%f\n",
							   (double)gain,
							   (double)offset);
					}
					break;

					case conversion_therm:
					{
						float32_t r0 =
							data_conversion_get_parameter(
								dt_sensors_props[dt_sensor_index].adc_number,
								dt_sensors_props[dt_sensor_index].channel_number,
								1
							);

						float32_t b =
							data_conversion_get_parameter(
								dt_sensors_props[dt_sensor_index].adc_number,
								dt_sensors_props[dt_sensor_index].channel_number,
								2
							);

						float32_t rdiv =
							data_conversion_get_parameter(
								dt_sensors_props[dt_sensor_index].adc_number,
								dt_sensors_props[dt_sensor_index].channel_number,
								3
							);

						float32_t t0 =
							data_conversion_get_parameter(
								dt_sensors_props[dt_sensor_index].adc_number,
								dt_sensors_props[dt_sensor_index].channel_number,
								4
							);

						printk("    Conversion type is therm, \
							   with r0=%f, b=%f, rdiv=%f and t0=%f\n",
							   (double)r0,
							   (double)b,
							   (double)rdiv,
							   (double)t0);
					}
					break;

					case no_channel_error:
						continue;
				}
				nvsRetrieved = true;
			}
			else if (res == -1)
			{
				printk("No calibration value found in persistent storage. \
						Default values will be used for data conversion.\n");
				checkNvs = false;
			}
			else if (res == -2)
			{
				printk("Calibration values in persistent storage were stored \
					  with a previous version of the API and can't be recovered.
					  \ Default values will be used for data conversion.\n");
				checkNvs = false;
			}
			else if (res == -3)
			{
				printk("Calibration values for ADC %u channel %u were found \
					   in persistent storage, but their format is incorrect. \
					   Possible data corruption.\n",
				       dt_sensors_props[dt_sensor_index].adc_number,
				       dt_sensors_props[dt_sensor_index].channel_number
				      );
			}
			else if (res == -4)
			{
				printk("Unable to find calibration values for ADC \
						%u channel %u in persistent storage. \
						Default values will be used.\n",
				       dt_sensors_props[dt_sensor_index].adc_number,
				       dt_sensors_props[dt_sensor_index].channel_number
				      );
			}
		}

		if (nvsRetrieved == false)
		{
			/* In case parameters were not found in NVS,
			 * get default values from device tree */
			switch (dt_sensors_props[dt_sensor_index].conversion_type)
			{
			case LINEAR:
				data_conversion_set_conversion_parameters_linear(
					dt_sensors_props[dt_sensor_index].adc_number,
				    dt_sensors_props[dt_sensor_index].channel_number,
				    dt_sensors_props[dt_sensor_index].default_gain.float_value,
					dt_sensors_props[dt_sensor_index].default_offset.float_value
				);
				break;
			case THERMISTANCE:
				data_conversion_set_conversion_parameters_therm(
					dt_sensors_props[dt_sensor_index].adc_number,
				    dt_sensors_props[dt_sensor_index].channel_number,
				    dt_sensors_props[dt_sensor_index].default_r0.float_value,
				    dt_sensors_props[dt_sensor_index].default_b.float_value,
				    dt_sensors_props[dt_sensor_index].default_rdiv.float_value,
				    dt_sensors_props[dt_sensor_index].default_t0.float_value
				);
				break;
			default:
				break;
			}
		}

		/* Count sensor for ADC */
		uint8_t adc_index = dt_sensors_props[dt_sensor_index].adc_number - 1;
		available_sensors_count[adc_index]++;
	}

	/* Create the channels list for each ADC */
	for (uint8_t adc_index = 0 ; adc_index < ADC_COUNT ; adc_index++)
	{
		available_sensors_props[adc_index] =
			(sensor_dt_data_t**)k_malloc(sizeof(sensor_dt_data_t*) *
										available_sensors_count[adc_index]);
	}

	/* Populate the channels list for each ADC */
	uint8_t adc_channels_count[ADC_COUNT] = {0};
	for (uint8_t dt_sensor_index = 0 ;
		 dt_sensor_index < DT_SENSORS_COUNT ;
		 dt_sensor_index++)
	{
		uint8_t adc_index = dt_sensors_props[dt_sensor_index].adc_number - 1;
		if (adc_index < ADC_COUNT)
		{
			uint8_t& current_adc_channels_count = adc_channels_count[adc_index];

			available_sensors_props[adc_index][current_adc_channels_count] =
					&dt_sensors_props[dt_sensor_index];

			current_adc_channels_count++;
		}
	}

	initialized = true;
}


void SensorsAPI::getLineFromConsole(char* buffer, uint8_t buffer_size)
{
	/* Initializing variables for eventual loop */
	uint8_t carcount = 0;
	char received_char;

	do
	{
		received_char = console_getchar();
		buffer[carcount] = received_char;

		/* Backspace character */
		if (received_char == 0x08)
		{
			/* To avoid character count being negative */
			if (carcount>0)
			{
				carcount--;
			}
		}
		else
		{
			carcount++;
		}

		/* Echo received char */
		printk("%c", received_char);

		if (carcount >= (buffer_size-1))
		{
			printk("Maximum character allowed reached \n");
			break;
		}
	/* EOL char : CRLF */
	} while ((received_char!='\n'));
	 /* Adding end of tab character to prepare for atof function */
	buffer[carcount-2] = '\0';
}

float32_t SensorsAPI::getCalibrationCoefficients(const char* physicalParameter,
												 const char* gainOrOffset)
{
	/* Maximum number of number for gain and offset value */
	const uint8_t MaxCharInOneLine = 20;
	/* Number of character in one line */
	char line[MaxCharInOneLine];
	/* Confirmation */
	float32_t confirm;
	float32_t parameterCoefficient;

	do
	{
		printk("Type %s %s and press enter \n", physicalParameter, gainOrOffset);
		getLineFromConsole(line, MaxCharInOneLine);

		/* Convert string to float */
		parameterCoefficient = atof(line);

		/* Get confirmation */
		printk("%s %s applied will be : %f\n",
			   physicalParameter,
			   gainOrOffset,
			   (double)parameterCoefficient);

		printk("Press enter to validate, any other character \
				to retype the %s \n",
				gainOrOffset);

		getLineFromConsole(line, MaxCharInOneLine);

        /* Check if Enter was pressed (empty input) */
        if (strlen(line) == 0) {
			/* Confirmed */
            confirm = 0;
        } else {
			/* Re-type required */
            confirm = 1;
        }

	} while(confirm);

	return parameterCoefficient;
}
