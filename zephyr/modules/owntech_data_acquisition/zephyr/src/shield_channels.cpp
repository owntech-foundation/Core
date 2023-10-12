/*
 * Copyright (c) 2021-2023 LAAS-CNRS
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
 * @author Hugues Larrive <hugues.larrive@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Thomas Walter <thomas.walter@laas.fr>
 *
 * @brief  ADC channel management from device tree.
 *         This file allows, for shields that define
 *         adc-channels in device tree, to automatically
 *         extract available channels and manage them by
 *         name using an enumeration.
 */


// Stdlib
#include <string.h>
#include <stdlib.h>

// Zephyr
#include <zephyr/kernel.h>
#include <zephyr/console/console.h>

// OwnTech API
#include "DataAcquisition.h"

// Current file header
#include "shield_channels.h"


/////
// Device-tree related macros

#define CHANNEL_NAME(node_id)    DT_STRING_TOKEN(node_id, channel_name)
#define CHANNEL_IS_DIFF(node_id) DT_PROP(node_id, differential)
#define CHANNEL_NUMBER(node_id)  DT_PHA_BY_IDX(node_id, io_channels, 0, input)
#define PIN_NUMBER(node_id)      DT_PROP(node_id, spin_pin)
#define ADC_REG_ADDR(node_id)    DT_REG_ADDR(DT_PHANDLE(node_id, io_channels))
#define CHANNEL_GAIN(node_id)    DT_PROP(node_id, default_gain)
#define CHANNEL_OFFSET(node_id)  DT_PROP(node_id, default_offset)

// Channel properties
#define CHANNEL_WRITE_PROP(node_id)                                       \
	{                                                                     \
		.name=CHANNEL_NAME(DT_PARENT(node_id)),                           \
		.channel_number=CHANNEL_NUMBER(node_id),                          \
		.pin_number=PIN_NUMBER(DT_PARENT(node_id)),                       \
		.is_differential=CHANNEL_IS_DIFF(node_id),                        \
		.adc_reg_addr=ADC_REG_ADDR(node_id),                              \
		.default_gain={.raw_value = CHANNEL_GAIN(DT_PARENT(node_id))},    \
		.default_offset={.raw_value = CHANNEL_OFFSET(DT_PARENT(node_id))} \
	},

#define SUBCHANNEL_WRITE_PROP(node_id) DT_FOREACH_CHILD(node_id, CHANNEL_WRITE_PROP)


// Channel count. This is very dirty!
#define CHANNEL_COUNTER(node_id) +1
#define SUBCHANNEL_COUNTER(node_id) DT_FOREACH_CHILD(node_id, CHANNEL_COUNTER)
#define DT_CHANNELS_COUNT DT_FOREACH_STATUS_OKAY(adc_channels, SUBCHANNEL_COUNTER)


/////
// Types definition

typedef union
{
	uint32_t  raw_value;
	float32_t float_value;
} int2float;

typedef struct
{
	channel_t name;
	uint8_t   adc_number;
	uint8_t   channel_number;
	uint8_t   pin_number;
	bool      is_differential;
	uint32_t  adc_reg_addr; // ADC addr is used to identify ADC
	int2float default_gain;
	int2float default_offset;
} channel_prop_t;




/////
// Variables

// Auto-populated array containing available channels
// extracted from the device tree.
static channel_prop_t dt_channels_props[] =
{
	DT_FOREACH_STATUS_OKAY(adc_channels, SUBCHANNEL_WRITE_PROP)
};

// Number of available channels defined in device tree for each ADC.
static uint8_t available_channels_count[ADC_COUNT] = {0};

// List of available channels containing 1 array for each ADC.
// Each array contains pointers to channel definition in
// available_channels_props array.
// For each ADC, the array size will match the value of
// available_channels_count for the ADC.
static channel_prop_t** available_channels_props[ADC_COUNT] = {0};

// List of channels enabled by user configuration.
// For each channel, a nullptr indicates it has not been
// enabled, and a valid pointer will point to the structure
// containing relevant information for this channel.
static channel_prop_t* enabled_channels[DT_CHANNELS_COUNT] = {0};

static bool initialized = false;


/////
// ADC channels private functions

/**
 * Builds list of device-tree defined channels for each ADC.
 */
static void _adc_channels_build_available_channels_lists()
{
	bool checkNvs = true;

	// Retreive calibration coefficients for each channel listed in device tree
	for (uint8_t dt_channel_index = 0 ; dt_channel_index < DT_CHANNELS_COUNT ; dt_channel_index++)
	{
		// Determine ADC number based on its address
		switch (dt_channels_props[dt_channel_index].adc_reg_addr)
		{
			case 0x50000000:
				dt_channels_props[dt_channel_index].adc_number = 1;
				break;
			case 0x50000100:
				dt_channels_props[dt_channel_index].adc_number = 2;
				break;
			case 0x50000400:
				dt_channels_props[dt_channel_index].adc_number = 3;
				break;
			case 0x50000500:
				dt_channels_props[dt_channel_index].adc_number = 4;
				break;
			case 0x50000600:
				dt_channels_props[dt_channel_index].adc_number = 5;
				break;
			default:
				dt_channels_props[dt_channel_index].adc_number = 0;
				continue;
				break;
		}

		// Get parameters from NVS if they exist
		bool nvsRetrieved = false;
		if (checkNvs == true)
		{
			int8_t res = data_conversion_retrieve_channel_parameters_from_nvs(dt_channels_props[dt_channel_index].adc_number,
			                                                                    dt_channels_props[dt_channel_index].channel_number
			                                                                   );

			if (res == 0)
			{
				printk("Parameters for ADC %u channel %u have been retrieved from flash\n", dt_channels_props[dt_channel_index].adc_number, dt_channels_props[dt_channel_index].channel_number);
				conversion_type_t conv_type = data_conversion_get_conversion_type(dt_channels_props[dt_channel_index].adc_number, dt_channels_props[dt_channel_index].channel_number);
				switch (conv_type)
				{
					case conversion_linear:
					{
						float32_t gain   = data_conversion_get_parameter(dt_channels_props[dt_channel_index].adc_number, dt_channels_props[dt_channel_index].channel_number, 1);
						float32_t offset = data_conversion_get_parameter(dt_channels_props[dt_channel_index].adc_number, dt_channels_props[dt_channel_index].channel_number, 2);
						printk("    Conversion type is linear, with gain=%f and offset=%f\n", gain, offset);
					}
					break;
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
				       dt_channels_props[dt_channel_index].adc_number,
				       dt_channels_props[dt_channel_index].channel_number
				      );
			}
			else if (res == -4)
			{
				printk("Unable to find calibration values for ADC %u channel %u in persistent storage. Default values will be used.\n",
				       dt_channels_props[dt_channel_index].adc_number,
				       dt_channels_props[dt_channel_index].channel_number
				      );
			}
		}

		if (nvsRetrieved == false)
		{
			// In case parameters were not found in NVS, get default vaules from device tree
			data_conversion_set_conversion_parameters_linear(dt_channels_props[dt_channel_index].adc_number,
		                                                     dt_channels_props[dt_channel_index].channel_number,
		                                                     dt_channels_props[dt_channel_index].default_gain.float_value,
		                                                     dt_channels_props[dt_channel_index].default_offset.float_value
		                                                    );
		}

		// Count channel for ADC
		uint8_t adc_index = dt_channels_props[dt_channel_index].adc_number - 1;
		available_channels_count[adc_index]++;
	}

	// Create the channels list for each ADC
	for (uint8_t adc_index = 0 ; adc_index < ADC_COUNT ; adc_index++)
	{
		available_channels_props[adc_index] = (channel_prop_t**)k_malloc(sizeof(channel_prop_t*) * available_channels_count[adc_index]);
	}

	// Populate the channels list for each ADC
	uint8_t adc_channels_count[ADC_COUNT] = {0};
	for (uint8_t dt_channel_index = 0 ; dt_channel_index < DT_CHANNELS_COUNT ; dt_channel_index++)
	{
		uint8_t adc_index = dt_channels_props[dt_channel_index].adc_number - 1;
		if (adc_index < ADC_COUNT)
		{
			uint8_t& current_adc_channels_count = adc_channels_count[adc_index];
			available_channels_props[adc_index][current_adc_channels_count] = &dt_channels_props[dt_channel_index];
			current_adc_channels_count++;
		}
	}

	initialized = true;
}


/////
// Public functions

void shield_channels_enable_adc_channel(uint8_t adc_num, channel_t channel_name)
{
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

	// Check parameters
	if (adc_num > ADC_COUNT) return;
	if (channel_name == UNDEFINED_CHANNEL) return;

	// Find channel property
	uint8_t adc_index = adc_num-1;
	channel_prop_t* channel_prop = nullptr;
	for (uint8_t channel = 0 ; channel < available_channels_count[adc_index] ; channel++)
	{
		channel_prop_t* current_channel = available_channels_props[adc_index][channel];
		if (current_channel->name == channel_name)
		{
			channel_prop = current_channel;
		}
	}

	// Check if we did find a channel
	if (channel_prop == nullptr) return;

	// Register channel enabling
	int channel_index = ((int)channel_name) - 1;
	enabled_channels[channel_index] = channel_prop;
}

channel_info_t shield_channels_get_enabled_channel_info(channel_t channel_name)
{
	if (initialized == false)
	{
		_adc_channels_build_available_channels_lists();
	}

	int channel_index = ((int)channel_name) - 1;
	channel_prop_t* channel_prop = enabled_channels[channel_index];
	if (channel_prop != nullptr)
	{
		return channel_info_t(channel_prop->adc_number, channel_prop->channel_number, channel_prop->pin_number);
	}
	else
	{
		return channel_info_t(0, 0, 0);
	}
}


/////
// Calibration for Twist shield

static void _get_line_from_console(char* buffer, uint8_t buffer_size)
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


static float32_t _shield_channels_get_calibration_coefficients(const char* physicalParameter, const char* gainOrOffset)
{
	// Maximum number of number for gain and offset value
	const uint8_t MaxCharInOneLine = 20;

	char received_char;
	char line[MaxCharInOneLine]; // Number of character in one line
	float32_t parameterCoefficient;

	do
	{
		printk("Type %s %s and press enter \n", physicalParameter, gainOrOffset);
		_get_line_from_console(line, MaxCharInOneLine);

		// Convert string to float
		parameterCoefficient = atof(line);

		// Get confirmation
		printk("%s %s applied will be : %f\n", physicalParameter, gainOrOffset, parameterCoefficient);
		printk("Press y to validate, any other character to retype the %s \n", gainOrOffset);
		received_char = console_getchar();

	} while(received_char != 'y');

	return parameterCoefficient;
}

void shield_channels_set_user_acquisition_parameters()
{
	float32_t gains[6];   // VH, V1, V2, IH, I1, I2
	float32_t offsets[6]; // VH, V1, V2, IH, I1, I2

	gains[0]   = _shield_channels_get_calibration_coefficients("VHigh", "gain");
	offsets[0] = _shield_channels_get_calibration_coefficients("VHigh", "offset");
	gains[1]   = _shield_channels_get_calibration_coefficients("V1Low", "gain");
	offsets[1] = _shield_channels_get_calibration_coefficients("V1Low", "offset");
	gains[2]   = _shield_channels_get_calibration_coefficients("V2Low", "gain");
	offsets[2] = _shield_channels_get_calibration_coefficients("V2Low", "offset");
	gains[3]   = _shield_channels_get_calibration_coefficients("IHigh", "gain");
	offsets[3] = _shield_channels_get_calibration_coefficients("IHigh", "offset");
	gains[4]   = _shield_channels_get_calibration_coefficients("I1Low", "gain");
	offsets[4] = _shield_channels_get_calibration_coefficients("I1Low", "offset");
	gains[5]   = _shield_channels_get_calibration_coefficients("I2Low", "gain");
	offsets[5] = _shield_channels_get_calibration_coefficients("I2Low", "offset");

	channel_info_t channel_info = shield_channels_get_enabled_channel_info(V_HIGH);
	data_conversion_set_conversion_parameters_linear(channel_info.adc_num, channel_info.channel_num, gains[0], offsets[0]);

	channel_info = shield_channels_get_enabled_channel_info(V1_LOW);
	data_conversion_set_conversion_parameters_linear(channel_info.adc_num, channel_info.channel_num, gains[1], offsets[1]);

	channel_info = shield_channels_get_enabled_channel_info(V2_LOW);
	data_conversion_set_conversion_parameters_linear(channel_info.adc_num, channel_info.channel_num, gains[2], offsets[2]);

	channel_info = shield_channels_get_enabled_channel_info(I_HIGH);
	data_conversion_set_conversion_parameters_linear(channel_info.adc_num, channel_info.channel_num, gains[3], offsets[3]);

	channel_info = shield_channels_get_enabled_channel_info(I1_LOW);
	data_conversion_set_conversion_parameters_linear(channel_info.adc_num, channel_info.channel_num, gains[4], offsets[4]);

	channel_info = shield_channels_get_enabled_channel_info(I2_LOW);
	data_conversion_set_conversion_parameters_linear(channel_info.adc_num, channel_info.channel_num, gains[5], offsets[5]);

	printk("Calibration coefficients successfully updated!\n");

	// Ask for save in NVS
	printk("Do you want to store these parameters in permanent storage?\n");
	printk("Parameters stored in permanent storage are automatically retreived at board boot.\n");
	printk("Not storing them in permanent storage will result in parameters being lost on board power cycle.\n");
	printk("Press y to store parameters in permanent storage, any other key to don't store them.\n");

	char received_char = console_getchar();
	if (received_char == 'y')
	{
		channel_info = shield_channels_get_enabled_channel_info(V_HIGH);
		int8_t err = data_conversion_store_channel_parameters_in_nvs(channel_info.adc_num, channel_info.channel_num);

		channel_info = shield_channels_get_enabled_channel_info(V1_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(channel_info.adc_num, channel_info.channel_num);

		channel_info = shield_channels_get_enabled_channel_info(V2_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(channel_info.adc_num, channel_info.channel_num);

		channel_info = shield_channels_get_enabled_channel_info(I_HIGH);
		err |= data_conversion_store_channel_parameters_in_nvs(channel_info.adc_num, channel_info.channel_num);

		channel_info = shield_channels_get_enabled_channel_info(I1_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(channel_info.adc_num, channel_info.channel_num);

		channel_info = shield_channels_get_enabled_channel_info(I2_LOW);
		err |= data_conversion_store_channel_parameters_in_nvs(channel_info.adc_num, channel_info.channel_num);

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
