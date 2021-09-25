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
 *
 * @brief This file implements the core management of the ADCs.
 *
 * THIS FILE SHOULD NOT BE INCLUDED ANYWHERE OUTSIDE FILES
 * FROM THE SAME FOLDER.
 */

#ifndef ADC_CORE_H_
#define ADC_CORE_H_


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/////
// Init, enable, start

/**
 * ADC initialization procedure for
 * ADC 1, ADC 2 and ADC 3.
 */
void adc_core_init();

/**
 * ADC enable.
 * Refer to RM 21.4.9
 *
 * @param adc_num Number of the ADC to enable.
 */
void adc_core_enable(uint8_t adc_num);

/**
 * ADC start.
 * Refer to RM 21.4.15
 *
 * @param adc_num Number of the ADC to start.
 */
void adc_core_start(uint8_t adc_num);

/////
// Configuration functions

/**
 * ADC dual mode: enables ADC 1/ADC 2 synchronization.
 * When ADC 1 acquisition is triggered, it simultaneously
 * triggers an acquisition on ADC 2.
 *
 * Refer to RM 21.4.30
 *
 * @param dual_mode true to enable dual moode, false to
 *        disable it. false by default.
 */
void adc_core_set_dual_mode(uint8_t dual_mode);

/**
 * ADC DMA mode configuration.
 * Enables DMA and circular mode on an ADC.
 *
 * @param adc_num Number of the ADC on which to enable DMA.
 */
void adc_core_configure_dma_mode(uint8_t adc_num);

/**
 * Defines the trigger source for an ADC.
 *
 * @param adc_num Number of the ADC to configure.
 * @param ExternalTriggerEdge Edge of the trigger as defined
 *        in stm32gxx_ll_adc.h (LL_ADC_REG_TRIG_***).
 * @param trigger_source Source of the trigger as defined
 *        in stm32gxx_ll_adc.h (LL_ADC_REG_TRIG_***).
 */
void adc_core_configure_trigger_source(uint8_t adc_num, uint32_t ExternalTriggerEdge, uint32_t TriggerSource);


#ifdef __cplusplus
}
#endif

#endif // ADC_CORE_H_
