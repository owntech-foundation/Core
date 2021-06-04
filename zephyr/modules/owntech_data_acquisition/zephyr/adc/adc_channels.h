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
 * @brief  This is the public include for adc_channels.h
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef ADC_CHANNELS_H_
#define ADC_CHANNELS_H_


// Zephyr
#include <zephyr.h>


#ifdef __cplusplus
extern "C" {
#endif

void adc_channels_init();
void adc_channels_configure(ADC_TypeDef* adc);

char* adc_channels_get_channel_name(uint8_t adc_num, uint8_t channel_rank);

uint8_t adc_channels_get_channels_count(uint8_t adc_num);


#ifdef __cplusplus
}
#endif

#endif // ADC_CHANNELS_H_
