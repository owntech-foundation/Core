/*
 * Copyright (c) 2023 LAAS-CNRS
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
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


// Current file header
#include "HardwareConfiguration.h"


// Public object to interact with the class
HardwareConfiguration hwConfig;

// Static class member
hardware_version_t HardwareConfiguration::hardware_version = nucleo_G474RE;


void HardwareConfiguration::setBoardVersion(hardware_version_t hardware_version)
{
	HardwareConfiguration::hardware_version = hardware_version;

	if (hardware_version == O2_v_1_1_2 || hardware_version == O2_v_0_9)
	{
		uart1SwapRxTx();
		hrtimLegTu(TIMA, TIMB);
	}else if(hardware_version == SPIN_v_0_1){
		uart1SwapRxTx();
		hrtimLegTu(TIMA, TIMC);
	}else if(hardware_version == SPIN_v_0_9 || hardware_version == TWIST_v_1_1_2){
		hrtimLegTu(TIMA, TIMC);
	}else if(hardware_version == nucleo_G474RE){
		hrtimLegTu(TIMA, TIMB);
	}

}
