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
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


// OwnTech headers
#include "../src/dac_configuration.h"
#include "../src/ngnd_configuration.h"
#include "../src/led_configuration.h"
#include "../src/timer_configuration.h"

// Current class header
#include "HardwareConfiguration.h"


/////
// Public object to interact with the class

HardwareConfiguration hwConfig;


/////
// Public static configuration functions


/////
// DAC

void HardwareConfiguration::initDac1Dac3CurrentMode()
{
	dac_config_dac1_dac3_current_mode_init();
}


/////
// NGND

void HardwareConfiguration::setNgndOn()
{
	ngnd_config_on();
}

void HardwareConfiguration::setNgndOff()
{
	ngnd_config_off();
}

/////
// LED

void HardwareConfiguration::setLedOn()
{
	led_config_on();
}

void HardwareConfiguration::setLedOff()
{
	led_config_off();
}

void HardwareConfiguration::setLedToggle()
{
	led_config_toggle();
}

// Incremental encoder

void HardwareConfiguration::startLoggingIncrementalEncoder()
{
	timer_incremental_encoder_tim4_start();
}

uint32_t HardwareConfiguration::getIncrementalEncoderValue()
{
	return timer_incremental_encoder_tim4_get_step();
}
