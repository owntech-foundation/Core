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
 * @brief  This is an ad-hoc ADC driver for OwnTech's
 * application. It supports differential channel setup
 * unlike Zephyr's STM32 driver.
 * It configures ADC 1 and ADC 2, using a common clock
 * which is AHB clock with a prescaler division by 4.
 * ADC 3 is also enabled independently.
 *
 * To use this driver, first call adc_init(), then call
 * required configuration functions, then call adc_start().
 *
 * This file is the entry point of the ADC management.
 * Only this header file provides public functions for the
 * ADC. No other header from this folder should be included
 * in files outside this folder.
 */


#ifndef ADC_H_
#define ADC_H_


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Initializes the ADCs. It must be
 * called *before* any configuration is made.
 */
void adc_init();

/**
 * ADC dual mode: enables ADC 1/ADC 2 synchronization.
 * When ADC 1 acquisition is triggered, it simultaneously
 * triggers an acquisition on ADC 2.
 *
 * @param dual_mode true to enable dual moode, false to
 *        disable it. false by default.
 */
void adc_set_dual_mode(uint8_t dual_mode);

/**
 * Regsters the triger source for an ADC.
 * It will be applied when ADC is started.
 *
 * @param adc_number Number of the ADC to configure
 * @param triggger_source Source of the trigger as defined
 *        in stm32gxx_ll_adc.h (LL_ADC_REG_TRIG_***)
 */
void adc_configure_trigger_source(uint8_t adc_number, uint32_t trigger_source);

/**
 * This function is used to configure the channels to be
 * enabled on a given ADC.
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
int8_t adc_configure_adc_channels(uint8_t adc_number, char* channel_list[], uint8_t channel_count);

/**
 * @brief Starts all configured ADCs.
 */
void adc_start();

/**
 * This function returns the name of an enabled channel.
 *
 * This function must onle be called after
 * adc_configure_adc_channels has been called.
 *
 * @param  adc_number Number of the ADC
 * @param  channel_rank Rank of the ADC channel to query.
 *         Rank ranges from 0 to (number of enabled channels)-1
 * @return Name of the channel as defined in the device tree, or
 *         NULL if channel configuration has not been made or
 *         channel_rank is over (number of enabled channels)-1.
 */
char* adc_get_channel_name(uint8_t adc_number, uint8_t channel_rank);

#ifdef __cplusplus
}
#endif

#endif // ADC_H_
