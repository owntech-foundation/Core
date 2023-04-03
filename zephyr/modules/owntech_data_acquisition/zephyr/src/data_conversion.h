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


/**
 * @brief    Converts the values of the given raw_value into a physical unit
 *
 * @param[in] raw_value
 *
 * @return  a foat32_t value for v1 low voltage
 */
float32_t data_conversion_convert_v1_low(uint16_t raw_value);

/**
 * @brief    Converts the values of the given raw_value into a physical unit
 *
 * @param[in] raw_value
 *
 * @return  a foat32_t value for v2 low voltage
 */
float32_t data_conversion_convert_v2_low(uint16_t raw_value);

/**
 * @brief    Converts the values of the given raw_value into a physical unit
 *
 * @param[in] raw_value
 *
 * @return  a foat32_t value for vhigh voltage
 */
float32_t data_conversion_convert_v_high(uint16_t raw_value);

/**
 * @brief    Converts the values of the given raw_value into a physical unit
 *
 * @param[in] raw_value
 *
 * @return  a foat32_t value for i1 low voltage
 */
float32_t data_conversion_convert_i1_low(uint16_t raw_value);

/**
 * @brief    Converts the values of the given raw_value into a physical unit
 *
 * @param[in] raw_value
 *
 * @return  a foat32_t value for i2 low voltage
 */
float32_t data_conversion_convert_i2_low(uint16_t raw_value);

/**
 * @brief    Converts the values of the given raw_value into a physical unit
 *
 * @param[in] raw_value
 *
 * @return  a foat32_t value for ihigh low voltage
 */
float32_t data_conversion_convert_i_high(uint16_t raw_value);

/**
 * @brief    Converts the values of the given raw_value into a physical unit
 *
 * @param[in] raw_value
 *
 * @return  a foat32_t value for the temperature probe
 */
float32_t data_conversion_convert_temp(uint16_t raw_value);

/**
 * @brief    Converts the values of the extra parameter into a physical unit
 *
 * @param[in] raw_value
 *
 * @return  a foat32_t value for the extra measurement
 */
float32_t data_conversion_convert_extra(uint16_t raw_value);


/**
 * @brief    Converts the values of the analog communication into a physical unit
 *
 * @param[in] raw_value
 *
 * @return  a foat32_t value for the analog communication
 */
float32_t data_conversion_convert_analog_comm(uint16_t raw_value);


/**
 * @brief    Change the parameters for the data conversion of V1_low
 *
 * @param[in] gain      gain of V1_low sensor
 * @param[in] offset    offset of the sensor
 */
void data_conversion_set_v1_low_parameters(float32_t gain, float32_t offset);

/**
 * @brief    Change the parameters for the data conversion of V2_low
 *
 * @param[in] gain      gain of V2_low sensor
 * @param[in] offset    offset of the sensor
 */
void data_conversion_set_v2_low_parameters(float32_t gain, float32_t offset);

/**
 * @brief    Change the parameters for the data conversion of V_high
 *
 * @param[in] gain      gain of V_high sensor
 * @param[in] offset    offset of the sensor
 */
void data_conversion_set_v_high_parameters(float32_t gain, float32_t offset);

/**
 * @brief    Change the parameters for the data conversion of I1_low
 *
 * @param[in] gain      gain of I1_low sensor
 * @param[in] offset    offset of the sensor
 */
void data_conversion_set_i1_low_parameters(float32_t gain, float32_t offset);

/**
 * @brief    Change the parameters for the data conversion of I2_low
 *
 * @param[in] gain      gain of I2_low sensor
 * @param[in] offset    offset of the sensor
 */
void data_conversion_set_i2_low_parameters(float32_t gain, float32_t offset);

/**
 * @brief    Change the parameters for the data conversion of I_high
 *
 * @param[in] gain      gain of I_high sensor
 * @param[in] offset    offset of the sensor
 */
void data_conversion_set_i_high_parameters(float32_t gain, float32_t offset);

/**
 * @brief    Change the parameters for the data conversion of temp
 *
 * @param[in] gain      gain of temp sensor
 * @param[in] offset    offset of the sensor
 */
void data_conversion_set_temp_parameters(float32_t gain, float32_t offset);

/**
 * @brief    Change the parameters for the extra measurement
 *
 * @param[in] gain      gain of temp sensor
 * @param[in] offset    offset of the sensor
 */
void data_conversion_set_extra_parameters(float32_t gain, float32_t offset);


/**
 * @brief    Change the parameters for the analog communication measurement
 *
 * @param[in] gain      gain of the analog communication bus
 * @param[in] offset    offset of the analog communication
 */
void data_conversion_set_analog_comm_parameters(float32_t gain, float32_t offset);

/**
 * @brief    Set calibration coefficients of all gains to 1 and all offsets to 0
 */
void set_default_acquisition_parameters();

/**
 * @brief    Retrieve calibration coefficients via console input and update them
 */
void set_user_acquisition_parameters();

#endif // DATA_CONVERSION_H_
