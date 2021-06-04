/*
 * Copyright (c) 2021 LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @brief  This is the public include for adc_helper.h
 * It should only be included in C files from the current folder.
 */

#ifndef ADC_HELPER_H_
#define ADC_HELPER_H_


// Zephyr
#include <zephyr.h>


#ifdef __cplusplus
extern "C" {
#endif


ADC_TypeDef* _get_adc_by_name(char* name);
uint32_t adc_decimal_nb_to_rank(uint8_t decimal_rank);


#ifdef __cplusplus
}
#endif

#endif // ADC_HELPER_H_
