/*
 * Copyright (c) 2022 LAAS-CNRS
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
 * @date   2022
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef ADC_CONFIGURATION_H_
#define ADC_CONFIGURATION_H_


// Stdlib
#include <stdint.h>

// OwnTech API
#include "adc.h"


/////
// Public API

/**
 * Use this function to set ADC 1 and ADC 2 in dual mode.
 * By default, ADC 1 and 2 are not in dual mode.
 *
 * This function must be called BEFORE ADC is started.
 *
 * @param  dual_mode Status of the dual mode:
 *         true to enable,
 *         false to disable.
 */
void configure_adc12_dual_mode(uint8_t dual_mode);

/**
 * This function is used to configure the channels to be
 * enabled on a given ADC.
 *
 * This function must be called BEFORE ADC is started.
 *
 * @param  adc_number Number of the ADC on which channel configuration is
 *         to be done.
 * @param  channel_list List of channels to configure. This is a list of
 *         names as defined in the device tree (field `label`). The order
 *         of the names in the array sets the acquisition ranks (order in
 *         which the channels are acquired).
 * @param  channel_count Number of channels defined in `channel_list`.
 * @return 0 is everything went well,
 *         ECHANNOTFOUND if at least one of the channels
 *           is not available in the given ADC. Available channels are the
 *           ones defined in the device tree.
 */
int8_t configure_adc_channels(uint8_t adc_number, const char* channel_list[], uint8_t channel_count);

/**
 * This function is used to change the trigger source of an ADC.
 * By default, triggger source for ADC 1 and ADC 2 is on HRTIM1,
 * and ADC 3 is software-triggered.
 *
 * This function must be called BEFORE ADC is started.
 *
 * @param  adc_number Number of the ADC to configure
 * @param  trigger_source Source of the trigger
 */
void configure_adc_trigger_source(uint8_t adc_number, adc_ev_src_t trigger_source);

/**
 * Registers the discontinuous count for an ADC.
 * It will be applied when ADC is started.
 *
 * @param adc_number Number of the ADC to configure.
 * @param dicontinuous_count Number of channels to acquire on each
 *        trigger event. 0 to disable discontinuous mode (default).
 */
void configure_adc_discontinuous_mode(uint8_t adc_number, uint32_t dicontinuous_count);

/**
 * This function is used to configure all ADC channels in default configuration.
 * Channels will be attributed as follows:
 * ADC1 -   V1_LOW      ADC2 -  I1_LOW
 *          V2_LOW              I2_LOW
 *          V_HIGH              I_HIGH
 *
 * This function must be called BEFORE ADC is started.
 */
void configure_adc_default_all_measurements();


/**
 * This function is used to configure all ADC channels in default configuration.
 * Channels will be attributed as follows:
 * ADC1 -   V1_LOW      ADC2 -  I1_LOW
 *          V2_LOW              I2_LOW
 *          V_HIGH              I_HIGH
 *                              EXTRA_MEAS
 *
 * This function must be called BEFORE ADC is started.
 */
void configure_adc_default_all_measurements_and_extra();


#endif // ADC_CONFIGURATION_H_
