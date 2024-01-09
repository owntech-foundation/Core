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
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */

#ifndef ADCHAL_H_
#define ADCHAL_H_

// Stdlib
#include <stdint.h>

// ARM lib
#include <arm_math.h>

// OwnTech API
#include "adc.h"




/**
 * @brief  Handles the ADC for the spin board
 *
 * @note   Use this element to call functions linked to the ADC for the SPIN board
 */
class AdcHAL
{
public:

/**
	 * @brief Change the trigger source of an ADC.
	 *        By default, triggger source for ADC 1/2 is on HRTIM1,
	 *        and ADC 3/4 is software-triggered.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param  adc_number Number of the ADC to configure
	 * @param  trigger_source Source of the trigger
	 */
	void configureTriggerSource(uint8_t adc_number, adc_ev_src_t trigger_source);

	/**
	 * @brief Set the discontinuous count for an ADC.
	 *        By default, ADCs are not in discontinuous mode.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param adc_number Number of the ADC to configure.
	 * @param discontinuous_count Number of channels to acquire on each
	 *        trigger event. 0 to disable discontinuous mode (default).
	 */
	void configureDiscontinuousMode(uint8_t adc_number, uint32_t dicontinuous_count);

	/**
	 * @brief ADC DMA mode configuration.
	 *        Enables DMA and circular mode on an ADC.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param adc_num Number of the ADC on which to enable DMA.
	 * @param use_dma Set to true to use DMA for this ADC,
	 *        false to not use it (default).
	 */
	void enableDma(uint8_t adc_number, bool use_dma);

	/**
	 * @brief Add a channel to the list of channels to be acquired
	 *        for an ADC.
	 *        The order in which channels are acquired is determined
	 *        by the order in which they are enabled.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param adc_number Number of the ADC to configure.
	 * @param channel Number of the channel to to be acquired.
	 */
	void enableChannel(uint8_t adc_number, uint8_t channel);

	/**
	 * @brief Removes a channel from the list of channels that are
	 *        acquired by an ADC.
	 *
	 * @note  If a channel has been enabled multiple times, then only
	 *        the first occurence in the list will be removed.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param adc_number Number of the ADC to configure.
	 * @param channel Number of the channel to to no longer be acquired.
	 */
	void disableChannel(uint8_t adc_number, uint8_t channel);

	/**
	 * @brief  Returns the number of enabled channels for an ADC.
	 *
	 * @param  adc_number Number of the ADC to fetch.
	 * @return Number of enabled channels on the given ADC.
	 */
	uint32_t getEnabledChannelsCount(uint8_t adc_number);

	/**
	 * @brief  Triggers a conversion on an ADC which is
	 *         configured as software triggered.
	 *
	 * @note   Software trigger is default for all ADCs
	 *         unless configured differently by the user or
	 *         another module.
	 *
	 * @param  adc_number Number of the ADC to fetch.
	 * @param  number_of_acquisitions Number of channels to acquire.
	 */
	void triggerSoftwareConversion(uint8_t adc_number, uint8_t number_of_acquisitions);

	/**
	 * @brief Start all configured ADCs.
	 */
	void startAllAdcs();

	/**
	 * @brief Stop all configured ADCs.
	 */
	void stopAllAdcs();

private:

	/**
	 * @brief Initialize all the adcs.
	 */
	void initializeAllAdcs();

	static bool adcInitialized;


};



#endif // ADCHAL_H_
