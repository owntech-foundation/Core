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
 * @brief Initialize data conversion.
 */
void data_conversion_init();

/**
 * @brief    Converts the values of the given raw_value into a physical unit.
 *
 * @param[in] raw_value
 *
 * @return   A float32_t value representing the value in the physical unit of the given channel.
 */
float32_t data_conversion_convert_raw_value(uint8_t adc_num, uint8_t channel_num, uint16_t raw_value);

/**
 * @brief    Change the parameters for the data conversion of a given channel.
 *
 * @param[in] gain      gain of the channel
 * @param[in] offset    offset of the channel
 */
void data_conversion_set_conversion_parameters(uint8_t adc_num, uint8_t channel_num, float32_t gain, float32_t offset);


#endif // DATA_CONVERSION_H_
