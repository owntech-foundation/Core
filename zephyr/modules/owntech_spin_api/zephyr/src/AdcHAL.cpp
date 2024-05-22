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
 * SPDX-License-Identifier: LGPL-2.1
 */

/**
 * @date   2023
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */


// OwnTech Power API
#include "adc.h"

// Current file header
#include "AdcHAL.h"


bool AdcHAL::adcInitialized = false;


void AdcHAL::initializeAllAdcs()
{
	if (adcInitialized == false)
	{
		// Perform default configuration
		adc_configure_trigger_source(1, software);
		adc_configure_trigger_source(2, software);
		adc_configure_trigger_source(3, software);
		adc_configure_trigger_source(4, software);
		adc_configure_trigger_source(5, software);

		adcInitialized = 1;
	}
}

void AdcHAL::configureTriggerSource(uint8_t adc_number, adc_ev_src_t trigger_source)
{
	/////
	// Make sure module is initialized

	if (adcInitialized == false)
	{
		initializeAllAdcs();
	}

	/////
	// Proceed

	adc_configure_trigger_source(adc_number, trigger_source);
}

void AdcHAL::configureDiscontinuousMode(uint8_t adc_number, uint32_t discontinuous_count)
{
	/////
	// Make sure module is initialized

	if (adcInitialized == false)
	{
		initializeAllAdcs();
	}

	/////
	// Proceed

	adc_configure_discontinuous_mode(adc_number, discontinuous_count);
}

void AdcHAL::enableChannel(uint8_t adc_num, uint8_t channel)
{
	/////
	// Make sure module is initialized

	if (adcInitialized == false)
	{
		initializeAllAdcs();
	}

	/////
	// Proceed

	adc_add_channel(adc_num, channel);
}

void AdcHAL::disableChannel(uint8_t adc_num, uint8_t channel)
{
	/////
	// Make sure module is initialized

	if (adcInitialized == false)
	{
		initializeAllAdcs();
	}

	/////
	// Proceed

	adc_remove_channel(adc_num, channel);
}

uint32_t AdcHAL::getEnabledChannelsCount(uint8_t adc_number)
{
	/////
	// Make sure module is initialized

	if (adcInitialized == false)
	{
		initializeAllAdcs();
	}

	/////
	// Proceed

	return adc_get_enabled_channels_count(adc_number);
}

void AdcHAL::enableDma(uint8_t adc_num, bool use_dma)
{
	/////
	// Make sure module is initialized

	if (adcInitialized == false)
	{
		initializeAllAdcs();
	}

	/////
	// Proceed

	adc_configure_use_dma(adc_num, use_dma);
}

void AdcHAL::startAllAdcs()
{
	/////
	// Make sure module is initialized

	if (adcInitialized == false)
	{
		initializeAllAdcs();
	}

	/////
	// Proceed

	adc_start();
}

void AdcHAL::stopAllAdcs()
{
	/////
	// Make sure module is initialized

	if (adcInitialized == false)
	{
		initializeAllAdcs();
	}

	/////
	// Proceed

	adc_stop();
}

void AdcHAL::triggerSoftwareConversion(uint8_t adc_number, uint8_t number_of_acquisitions)
{
	/////
	// Make sure module is initialized

	if (adcInitialized == false)
	{
		initializeAllAdcs();
	}

	/////
	// Proceed

	adc_trigger_software_conversion(adc_number, number_of_acquisitions);
}
