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
 * @brief  This is the public include for adc_cores.h
 * It should only be included in C files from the current folder.
 */

#ifndef ADC_CORE_H_
#define ADC_CORE_H_


// Zephyr
#include <zephyr.h>


#ifdef __cplusplus
extern "C" {
#endif


// Init, enable, start
void adc_core_init();
void adc_core_enable(ADC_TypeDef* adc);
void adc_core_start(ADC_TypeDef* adc);

// Configuration functions
void adc_core_set_channel_differential(ADC_TypeDef* adc, uint8_t channel);
void adc_core_configure_internal_paths(uint8_t vts, uint8_t vbat, uint8_t vref);
void adc_core_set_dual_mode();


#ifdef __cplusplus
}
#endif

#endif // ADC_CORE_H_
