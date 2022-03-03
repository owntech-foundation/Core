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
#include <arm_math.h>


/** Hardware version. See
 *  https://gitlab.laas.fr/owntech/1leg/-/wikis/Releases
 *  for the list and specificities of versions.
 */
typedef enum
{
    v_0_0, // No power converter attached, the software is running on Nucleo G474RE
    v_0_9,
    v_1_1_2
} hardware_version_t;


/////
// Static class definition

class HardwareConfiguration
{

public:
	// Common
	void setBoardVersion(hardware_version_t hardware_version);

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

	// Power converter
	void initInterleavedBuckMode();
	void initInterleavedBoostMode();
	void initFullBridgeBuckMode();
	void initFullBridgeBoostMode();
	void initIndependentMode(bool leg1_buck_mode, bool leg2_buck_mode);

	void setInterleavedDutyCycle(float32_t duty_cycle);
	void setFullBridgeDutyCycle(float32_t duty_cycle);
	void setLeg1DutyCycle(float32_t duty_cycle);
	void setLeg2DutyCycle(float32_t duty_cycle);

	void setInterleavedOn();
	void setFullBridgeOn();
	void setLeg1On();
	void setLeg2On();

	void setInterleavedOff();
	void setFullBridgeOff();
	void setLeg1Off();
	void setLeg2Off();

	// Extra UART
	void extraUartInit();
	char extraUartReadChar();
	void extraUartWriteChar(char data);

};


/////
// Public object to interact with the class

extern HardwareConfiguration hwConfig;



#endif // HARDWARECONFIGURATION_H_
