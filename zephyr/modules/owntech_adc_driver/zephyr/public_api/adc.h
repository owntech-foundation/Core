/*
 * Copyright (c) 2021-2022 LAAS-CNRS
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
 */


#ifndef ADC_H_
#define ADC_H_


// Stdlib
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


/////
// Public enums

typedef enum
{
	software  = 0,
	hrtim_ev1 = 1,
	hrtim_ev2 = 2,
	hrtim_ev3 = 3,
	hrtim_ev4 = 4,
} adc_ev_src_t;


/////
// Public API

/**
 * @brief Registers the triger source for an ADC.
 *
 *        This will only be applied when ADC is started.
 *        If ADC is already started, it must be stopped
 *        then started again.
 *
 * @param adc_number Number of the ADC to configure.
 * @param triggger_source Source of the trigger.
 */
void adc_configure_trigger_source(uint8_t adc_number, adc_ev_src_t trigger_source);

/**
 * @brief Registers the discontinuous count for an ADC.
 *
 *        This will only be applied when ADC is started.
 *        If ADC is already started, it must be stopped
 *        then started again.
 *
 * @param adc_number Number of the ADC to configure.
 * @param discontinuous_count Number of channels to acquire on each
 *        trigger event. 0 to disable discontinuous mode (default).
 */
void adc_configure_discontinuous_mode(uint8_t adc_number, uint32_t discontinuous_count);

/**
 * @brief Adds a channel to the list of channels to be acquired
 *        for an ADC.
 *        The order in which channels are added determine
 *        the order in which they will be acquired.
 *
 *        This will only be applied when ADC is started.
 *        If ADC is already started, it must be stopped
 *        then started again.
 *
 * @param adc_number Number of the ADC to configure.
 * @param channel Number of the channel to to be acquired.
 */
void adc_add_channel(uint8_t adc_number, uint8_t channel);

/**
 * @brief Removes a channel from the list of channels
 *        that are acquired by an ADC.
 *        If a channel has been added multiple times,
 *        then only the first occurence in the list
 *        will be removed.
 *
 *        This will only be applied when ADC is started.
 *        If ADC is already started, it must be stopped
 *        then started again.
 *
 * @param adc_number Number of the ADC to configure.
 * @param channel Number of the channel to to no longer be acquired.
 */
void adc_remove_channel(uint8_t adc_number, uint8_t channel);

/**
 * @brief Configures an ADC to use DMA.
 *
 *        This will only be applied when ADC is started.
 *        If ADC is already started, it must be stopped
 *        then started again.
 *
 * @param adc_number Number of the ADC to configure.
 * @param use_dma Set to true to use DMA for this ADC,
 *        false to not use it (default).
 */
void adc_configure_use_dma(uint8_t adc_number, bool use_dma);


/**
 * @brief Starts all configured ADCs.
 */
void adc_start();

/**
 * @brief Stops all configured ADCs.
 */
void adc_stop();

/**
 * @brief This function triggers a single conversion in
 *        the case of a software triggered ADC.
 *
 *        This function must only be called after
 *        ADC has been started.
 *
 * @param  adc_number Number of the ADC.
 */
void adc_trigger_software_conversion(uint8_t adc_number);


#ifdef __cplusplus
}
#endif

#endif // ADC_H_
