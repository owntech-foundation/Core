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
 * @author Antoine Boche <antoine.boche@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Thomas Walter <thomas.walter@laas.fr>
 */

/////
// Include


// Zephyr
#include <zephyr.h>
#include <console/console.h>

// Stdlib
#include <stdlib.h>

// Current file header
#include "data_conversion.h"

/////
// Local variables

// ADC convertions variable

// Currents
static float32_t gain_currents[3] = {0.0125, 0.0143, 0.0125}; //i1_low, i2_low and i_high
static float32_t offset_currents[3] = {25.9, 29.28, 0};       //i1_low, i2_low offsets - ihigh needs to be callibrated still

// Voltages
static float32_t gain_voltages[3] = {0.0462, 0.0462, 0.0640}; //v1_low, v2_low and v_high
static float32_t offset_voltages[3] = {-94.04, -94.04, 4.408};  //v1_low, v2_low and v_high

static float32_t gain_extra = 1.0; //gain for the extra
static float32_t offset_extra = 1.0;  //offset for the extra


// Temperature
static float32_t gain_temperature = 1.0;
static float32_t offset_temperature = 0.0;

// Analog Communication Bus
static float32_t gain_analog_comm = 1.0;
static float32_t offset_analog_comm = 0.0;

//Maximum number of number for gain and offset value
static const uint8_t MaxCharInOneLine = 20;

/////
// Private Functions

float32_t get_calibration_coefficients(const char* physicalParameter,const char* gainOrOffset)
{

	uint8_t carcount;
	char received_char;
	char line[MaxCharInOneLine];//number of character in one line
	float32_t parameterCoefficient;

	do
	{	
		//Initializing variables for eventual loop
		carcount = 0;

		printk("Type %s %s and press enter \n", physicalParameter, gainOrOffset);

		do
		{
			received_char = console_getchar();
			line[carcount] = received_char; 

			if(received_char == 0x08)//backspace character
			{
				if(carcount>0)//To avoid carcount being negative
					carcount--;
			}
			else
			{	
				carcount++;
			}

			printk("%c", received_char);//echoing value

			if(carcount>=(MaxCharInOneLine-1))
			{
				printk("Maximum caracter allowed reached \n");
				break;
			}

		} while ((received_char!='\n')); // EOL char : CRLF
		line[carcount-2] = '\0'; // adding end of tab character to prepare for atof function

		//Converting string to float
		parameterCoefficient = atof(line);

		//Getting confirmation
		printk("%s %s applied will be : %f\n", physicalParameter, gainOrOffset, parameterCoefficient);

		//Getting validation
		printk("Press y to validate, any other character to retype the %s \n", gainOrOffset);
		received_char = console_getchar();

	}while(received_char != 'y');

	return parameterCoefficient;
}

/////
// Public Functions


/**
 * These functions convert raw values into their appropriate physical unit
 */
float32_t data_conversion_convert_v1_low(uint16_t raw_value)
{
	return (raw_value*gain_voltages[0])+offset_voltages[0];
}

float32_t data_conversion_convert_i1_low(uint16_t raw_value)
{
	return (raw_value*gain_currents[0])+offset_currents[0];
}

float32_t data_conversion_convert_v2_low(uint16_t raw_value)
{
	return (raw_value*gain_voltages[1])+offset_voltages[1];
}

float32_t data_conversion_convert_i2_low(uint16_t raw_value)
{
	return (raw_value*gain_currents[1])+offset_currents[1];
}

float32_t data_conversion_convert_v_high(uint16_t raw_value)
{
	return (raw_value*gain_voltages[2])+offset_voltages[2];
}

float32_t data_conversion_convert_i_high(uint16_t raw_value)
{
	return (raw_value*gain_currents[2])+offset_currents[2];
}

float32_t data_conversion_convert_temp(uint16_t raw_value)
{
	return (raw_value*gain_temperature)+offset_temperature;
}

float32_t data_conversion_convert_extra(uint16_t raw_value)
{
	return (raw_value*gain_extra)+offset_extra;
}

float32_t data_conversion_convert_analog_comm(uint16_t raw_value)
{
	return (raw_value*gain_analog_comm)+offset_analog_comm;
}



/**
 * These functions update the parameters of the variables
 */
void data_conversion_set_v1_low_parameters(float32_t gain, float32_t offset)
{
	gain_voltages[0] = gain;
	offset_voltages[0] = offset;
}

void data_conversion_set_v2_low_parameters(float32_t gain, float32_t offset)
{
	gain_voltages[1] = gain;
	offset_voltages[1] = offset;
}

void data_conversion_set_v_high_parameters(float32_t gain, float32_t offset)
{
	gain_voltages[2] = gain;
	offset_voltages[2] = offset;
}

void data_conversion_set_i1_low_parameters(float32_t gain, float32_t offset)
{
	gain_currents[0] = gain;
	offset_currents[0] = offset;
}

void data_conversion_set_i2_low_parameters(float32_t gain, float32_t offset)
{
	gain_currents[1] = gain;
	offset_currents[1] = offset;
}

void data_conversion_set_i_high_parameters(float32_t gain, float32_t offset)
{
	gain_currents[2] = gain;
	offset_currents[2] = offset;
}

void data_conversion_set_temp_parameters(float32_t gain, float32_t offset)
{
	gain_temperature = gain;
	offset_temperature = offset;
}

void data_conversion_set_extra_parameters(float32_t gain, float32_t offset)
{
	gain_extra = gain;
	offset_extra = offset;
}

void data_conversion_set_analog_comm_parameters(float32_t gain, float32_t offset)
{
	gain_analog_comm = gain;
	offset_analog_comm = offset;
}

void set_default_acquisition_parameters()
{
	float32_t gains[] = {1, 1, 1, 1, 1, 1};
	float32_t offsets[] = {0, 0, 0, 0, 0, 0};

	data_conversion_set_v_high_parameters(gains[0], offsets[0]);
	data_conversion_set_v1_low_parameters(gains[1], offsets[1]);
	data_conversion_set_v2_low_parameters(gains[2], offsets[2]);
	data_conversion_set_i_high_parameters(gains[3], offsets[3]);
	data_conversion_set_i1_low_parameters(gains[4], offsets[4]);
	data_conversion_set_i2_low_parameters(gains[5], offsets[5]);

	printk("Calibration gains set to default !\n");
}

void set_user_acquisition_parameters()
{
	static float32_t gains[6];//VH, V1, V2, IH, I1, I2
	static float32_t offsets[6];//VH, V1, V2, IH, I1, I2

	gains[0] = get_calibration_coefficients("VHigh", "gain");
	gains[1] = get_calibration_coefficients("V1Low", "gain");
	gains[2] = get_calibration_coefficients("V2Low", "gain");
	gains[3] = get_calibration_coefficients("I2Low", "gain");
	gains[4] = get_calibration_coefficients("IHigh", "gain");
	gains[5] = get_calibration_coefficients("I1Low", "gain");

	offsets[0] = get_calibration_coefficients("VHigh", "offset");
	offsets[1] = get_calibration_coefficients("V1Low", "offset");
	offsets[2] = get_calibration_coefficients("V2Low", "offset");
	offsets[3] = get_calibration_coefficients("I2Low", "offset");
	offsets[4] = get_calibration_coefficients("IHigh", "offset");
	offsets[5] = get_calibration_coefficients("I1Low", "offset");

	data_conversion_set_v_high_parameters(gains[0], offsets[0]);
	data_conversion_set_v1_low_parameters(gains[1], offsets[1]);
	data_conversion_set_v2_low_parameters(gains[2], offsets[2]);
	data_conversion_set_i_high_parameters(gains[3], offsets[3]);
	data_conversion_set_i1_low_parameters(gains[4], offsets[4]);
	data_conversion_set_i2_low_parameters(gains[5], offsets[5]);

	printk("Calibration coefficients updated !\n");
}