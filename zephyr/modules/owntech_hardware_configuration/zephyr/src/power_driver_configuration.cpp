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


// Zephyr
#include "zephyr.h"
#include "GpioApi.h"

// Current file header
#include "HardwareConfiguration.h"


bool HardwareConfiguration::powerDriverInitialized = false;


void HardwareConfiguration::powerDriverInitialize()
{
	gpio.configurePin(PC12,OUTPUT); //configures the pin of leg 1 mosfet high
	gpio.configurePin(PC13,OUTPUT); //configures the pin of leg 2 mosfet high
	powerDriverInitialized = true;
}

void HardwareConfiguration::powerDriverLeg1On()
{
	if (powerDriverInitialized == false)
	{
		powerDriverInitialize(); //calls the config automatically during the first execution
	}

	gpio.setPin(PC12);
}

void HardwareConfiguration::powerDriverLeg2On()
{
	if (powerDriverInitialized == false)
	{
		powerDriverInitialize(); //calls the config automatically during the first execution
	}

	gpio.setPin(PC13);
}

void HardwareConfiguration::powerDriverLeg1Off()
{
	if (powerDriverInitialized == false)
	{
		powerDriverInitialize(); //calls the config automatically during the first execution
	}

	gpio.resetPin(PC12);
}

void HardwareConfiguration::powerDriverLeg2Off()
{
	if (powerDriverInitialized == false)
	{
		powerDriverInitialize(); //calls the config automatically during the first execution
	}

	gpio.resetPin(PC12);
}

void HardwareConfiguration::powerDriverInterleavedOn()
{
	if (powerDriverInitialized == false)
	{
		powerDriverInitialize(); //calls the config automatically during the first execution
	}

	gpio.setPin(PC12);
	gpio.setPin(PC13);
}

void HardwareConfiguration::powerDriverInterleavedOff()
{
	if (powerDriverInitialized == false)
	{
		powerDriverInitialize(); //calls the config automatically during the first execution
	}

	gpio.resetPin(PC12);
	gpio.resetPin(PC13);
}

