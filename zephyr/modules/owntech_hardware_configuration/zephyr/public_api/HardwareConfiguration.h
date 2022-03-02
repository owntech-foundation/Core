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


#ifndef HARDWARECONFIGURATION_H_
#define HARDWARECONFIGURATION_H_


#include <stdint.h>


/////
// Static class definition

class HardwareConfiguration
{

public:
	// DAC
	void initDac1Dac3CurrentMode();

	// NGND
	void setNgndOn();
	void setNgndOff();

	// LED
	void setLedOn();
	void setLedOff();
	void setLedToggle();

	// Incremental encoder
	void startLoggingIncrementalEncoder();
	uint32_t getIncrementalEncoderValue();

};


/////
// Public object to interact with the class

extern HardwareConfiguration hwConfig;



#endif // HARDWARECONFIGURATION_H_
