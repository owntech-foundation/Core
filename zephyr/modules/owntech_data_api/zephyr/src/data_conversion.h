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

#ifndef DATA_CONVERSION_H_
#define DATA_CONVERSION_H_

#include <arm_math.h>   // adds all the CMSIS library


/////
// Type definitions

typedef enum : int8_t
{
	conversion_linear = 0,
	no_channel_error = -5

} conversion_type_t;


/////
// API

/**
 * @brief Initialize data conversion.
 */
void data_conversion_init();

/**
 * @brief    Converts the values of the given raw_value into a physical unit.
 *
 * @param[in] adc_num     ADC number
 * @param[in] channel_num Channel number
 * @param[in] raw_value   Value to convert
 *
 * @return   A float32_t value representing the value in the physical unit of the given channel.
 */
float32_t data_conversion_convert_raw_value(uint8_t adc_num, uint8_t channel_num, uint16_t raw_value);

/**
 * @brief    Change the parameters for the data conversion of a given channel.
 *
 * @param[in] adc_num     ADC number
 * @param[in] channel_num Channel number
 * @param[in] gain        Gain of the channel
 * @param[in] offset      Offset of the channel
 */
void data_conversion_set_conversion_parameters_linear(uint8_t adc_num, uint8_t channel_num, float32_t gain, float32_t offset);

/**
 * @brief Get the conversion type for a given channel
 *
 * @param[in] adc_num     ADC number
 * @param[in] channel_num Channel number
 *
 * @return Currently configured conversion type.
 */
conversion_type_t data_conversion_get_conversion_type(uint8_t adc_num, uint8_t channel_num);

/**
 * @brief Get a conversion parameter for a given channel
 *
 * @param[in] adc_num       ADC number
 * @param[in] channel_num   Channel number
 * @param[in] parameter_num Number of the paramter to retreive. E.g. for linear parameters,
 *                          gain is param 1 and offset is param 2.
 *
 * @return Current value of the given parameter.
 */
float32_t data_conversion_get_parameter(uint8_t adc_num, uint8_t channel_num, uint8_t parameter_num);

/**
 * @brief Store the currently configured conversion parameters of a given channel in NVS.
 *
 * @param[in] adc_num       ADC number
 * @param[in] channel_num   Channel number
 *
 * @return 0 if parameters were correcly stored, -1 if there was an error.
 */
int8_t data_conversion_store_channel_parameters_in_nvs(uint8_t adc_num, uint8_t channel_num);

/**
 * @brief Retreived previously configured conversion parameters from NVS.
 *
 * @param[in] adc_num       ADC number
 * @param[in] channel_num   Channel number
 *
 * @return 0 if parameters were correcly retreived, negative value if there was an error:
 *         -1: NVS is empty
 *         -2: NVS contains data, but their version doesn't match current version
 *         -3: NVS data is corrupted
 *         -4: NVS contains data, but not for the requested channel
 */
int8_t data_conversion_retrieve_channel_parameters_from_nvs(uint8_t adc_num, uint8_t channel_num);


#endif // DATA_CONVERSION_H_
