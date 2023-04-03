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
 * @date 2023
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 *
 * @brief This file implements the core management of the ADCs.
 *
 * THIS FILE SHOULD NOT BE INCLUDED ANYWHERE OUTSIDE FILES
 * FROM THE SAME FOLDER.
 */

#ifndef ADC_CORE_H_
#define ADC_CORE_H_


#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

/////
// Init, enable, start, stop

/**
 * @brief ADC initialization procedure for
 *        ADC 1, ADC 2, ADC 3 and ADC 4.
 */
void adc_core_init();

/**
 * @brief ADC enable.
 *        Refer to RM 21.4.9
 *
 * @param adc_num Number of the ADC to enable.
 */
void adc_core_enable(uint8_t adc_num);

/**
 * @brief ADC start.
 *        Refer to RM 21.4.15
 *
 * @param adc_num Number of the ADC to start.
 * @param sequence_length Length of the sequence configured
 *        on that ADC.
 */
void adc_core_start(uint8_t adc_num, uint8_t sequence_length);

/**
 * @brief ADC stop.
 *
 * @param adc_num Number of the ADC to stop.
 */
void adc_core_stop(uint8_t adc_num);


/////
// Configuration functions

/**
 * @brief ADC DMA mode configuration.
 *        Enables DMA in circular mode on an ADC.
 *
 * @param adc_num Number of the ADC on which to enable DMA.
 * @param use_dma Set to true to use DMA for this ADC,
 *        false to not use it (default).
 */
void adc_core_configure_dma_mode(uint8_t adc_num, bool use_dma);

/**
 * @brief Defines the trigger source for an ADC.
 *
 * @param adc_num Number of the ADC to configure.
 * @param external_trigger_edge Edge of the trigger as defined
 *        in stm32gxx_ll_adc.h (LL_ADC_REG_TRIG_***).
 * @param trigger_source Source of the trigger as defined
 *        in stm32gxx_ll_adc.h (LL_ADC_REG_TRIG_***).
 */
void adc_core_configure_trigger_source(uint8_t adc_num, uint32_t external_trigger_edge, uint32_t trigger_source);

/**
 * @brief  Configures the discontinuous mode for an ADC.
 *
 * @param adc_num Number of the ADC to configure.
 * @param discontinuous_count Number of channels to acquire on each
 *        trigger event. 0 to disable discontinuous mode (default).
 */
void adc_core_configure_discontinuous_mode(uint8_t adc_num, uint32_t discontinuous_count);

/**
 * @brief ADC differential channel set-up:
 *        Applies differential mode to specified channel.
 *        Refer to RM 21.4.7
 *
 * @param adc_num Number of the ADC to configure.
 * @param channel Number of the channel to configure.
 * @param enable_differential Set true to enable differential mode,
 *        false to disable it (default).
 *
 */
void adc_core_set_channel_differential(uint8_t adc_num, uint8_t channel, bool enable_differential);

/**
 * @brief Configures an ADC channel acquisition.
 *        Acquisition rank is provided as a parameter.
 *        Channel sampling time is set to 12.5 cycles.
 *
 * @param adc_num Number of the ADC to configure.
 * @param channel Number of the channel to configure.
 * @param rank Acquisition rank.
 *
 */
void adc_core_configure_channel(uint8_t adc_num, uint8_t channel, uint8_t rank);


#ifdef __cplusplus
}
#endif

#endif // ADC_CORE_H_
