/*
 * Copyright (c) 2025-present LAAS-CNRS
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

/*
 * @date   2025
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef ADC_ENUMS_H_
#define ADC_ENUMS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines types of hrtim events to which the ADC connects:
 *
 * - software - software events
 *
 * - `hrtim_ev1` to `hrtim_ev9` - hrtim driven events
 *
 */
typedef enum
{
	software  = 0,
	hrtim_ev1 = 1,
	hrtim_ev2 = 2,
	hrtim_ev3 = 3,
	hrtim_ev4 = 4,
	hrtim_ev5 = 5,
	hrtim_ev6 = 6,
	hrtim_ev7 = 7,
	hrtim_ev8 = 8,
	hrtim_ev9 = 9
} adc_ev_src_t;

/**
 * @brief Defines the sampling time for the ADC.
 * The clock source is 170 MHz, and ADC prescaler is set
 * to 4, which makes the ADC clock 42.5 MHz.
 * Cycle duration is thus ≈ 23.5 ns.
 *
 * Available sampling times (in cycles) are:
 * - 2.5   => 58.75 ns
 * - 6.5   => 152.75 ns
 * - 12.5  => 293.75 ns
 * - 24.5  => 575.75 ns
 * - 47.5  => 1116.25 ns
 * - 92.5  => 2173.75 ns
 * - 247.5 => 5816.25 ns
 * - 640.5 => 15051.75 ns
 *
 * Let's round to the ns here.
 */
typedef enum
{
	adc_st_default = 0,
	adc_st_59ns,
	adc_st_153ns,
	adc_st_294ns,
	adc_st_576ns,
	adc_st_1116ns,
	adc_st_2174ns,
	adc_st_5817ns,
	adc_st_15052ns
} adc_sampling_time_t;

#ifdef __cplusplus
}
#endif

#endif /* ADC_ENUMS_H_ */
