/*
 * Copyright (c) 2022-2023 LAAS-CNRS
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
 * @brief This function is used to change the trigger source of an ADC.
 *        By default, triggger source for ADC 1 and ADC 2 is on HRTIM1,
 *        and ADC 3 is software-triggered.
 *
 *        This function must be called BEFORE ADC is started.
 *
 * @param  adc_number Number of the ADC to configure
 * @param  trigger_source Source of the trigger
 */
void configure_adc_trigger_source(uint8_t adc_number, adc_ev_src_t trigger_source);

/**
 * @brief Register the discontinuous count for an ADC.
 *        It will be applied when ADC is started.
 *
 * @param adc_number Number of the ADC to configure.
 * @param discontinuous_count Number of channels to acquire on each
 *        trigger event. 0 to disable discontinuous mode (default).
 */
void configure_adc_discontinuous_mode(uint8_t adc_number, uint32_t discontinuous_count);

/**
 * @brief Adds a channel to the list of channels to be acquired
 *        for an ADC.
 *        The order in which channels are added determine
 *        the order in which they will be acquired.
 *
 * @param adc_number Number of the ADC to configure.
 * @param channel Number of the channel to to be acquired.
 */
void configure_adc_add_channel(uint8_t adc_num, uint8_t channel);

/**
 * @brief Removes a channel from the list of channels
 *        that are acquired by an ADC.
 *        If a channel has been added multiple times,
 *        then only the first occurence in the list
 *        will be removed.
 *
 * @param adc_number Number of the ADC to configure.
 * @param channel Number of the channel to to no longer be acquired.
 */
void configure_adc_remove_channel(uint8_t adc_num, uint8_t channel);

/**
 * @brief ADC DMA mode configuration.
 *        Enables DMA and circular mode on an ADC.
 *
 * @param adc_num Number of the ADC on which to enable DMA.
 * @param use_dma Set to true to use DMA for this ADC,
 *        false to not use it (default).
 */
void configure_adc_dma_mode(uint8_t adc_num, bool use_dma);

/**
 * @brief Starts all configured ADCs.
 */
void start_adcs();

/**
 * @brief Stops all configured ADCs.
 */
void stop_adcs();


#endif // ADC_CONFIGURATION_H_
