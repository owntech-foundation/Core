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
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef HARDWARECONFIGURATION_H_
#define HARDWARECONFIGURATION_H_

// Std lib
#include <stdint.h>

// ARM lib
#include <arm_math.h>

// OwnTech API
#include "../src/adc_configuration.h"


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
	static void setBoardVersion(hardware_version_t hardware_version);

	// DAC
	static void initDac1Dac3CurrentMode();

	// NGND
	static void setNgndOn();
	static void setNgndOff();

	// LED
	static void setLedOn();
	static void setLedOff();
	static void setLedToggle();

	// Incremental encoder
	static void startLoggingIncrementalEncoder();
	static uint32_t getIncrementalEncoderValue();

	// Power converter
	static void initInterleavedBuckMode();
	static void initInterleavedBoostMode();
	static void initFullBridgeBuckMode();
	static void initFullBridgeBoostMode();
	static void initIndependentMode(bool leg1_buck_mode, bool leg2_buck_mode);

	static void setInterleavedDutyCycle(float32_t duty_cycle);
	static void setFullBridgeDutyCycle(float32_t duty_cycle);
	static void setLeg1DutyCycle(float32_t duty_cycle);
	static void setLeg2DutyCycle(float32_t duty_cycle);

	static void setInterleavedOn();
	static void setFullBridgeOn();
	static void setLeg1On();
	static void setLeg2On();

	static void setInterleavedOff();
	static void setFullBridgeOff();
	static void setLeg1Off();
	static void setLeg2Off();

	// Extra UART
	static void extraUartInit();
	static char extraUartReadChar();
	static void extraUartWriteChar(char data);

	// ADC
	static void configureAdc12DualMode(uint8_t dual_mode);
	static int8_t configureAdcChannels(uint8_t adc_number, const char* channel_list[], uint8_t channel_count);
	static void configureAdcTriggerSource(uint8_t adc_number, adc_ev_src_t trigger_source);
	static void configureAdcDefaultAllMeasurements();

};


/////
// Public object to interact with the class

extern HardwareConfiguration hwConfig;



#endif // HARDWARECONFIGURATION_H_
