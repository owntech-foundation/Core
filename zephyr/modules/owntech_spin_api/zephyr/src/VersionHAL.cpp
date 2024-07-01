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
 * SPDX-License-Identifier: LGPL-2.1
 */

/**
 * @date   2023
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


// Current file header
#include "SpinAPI.h"
#include "VersionHAL.h"

SpinAPI spin;

// Static class member
board_version_t VersionHAL::board_version = nucleo_G474RE;


void VersionHAL::setBoardVersion(board_version_t hardware_version)
{
	VersionHAL::board_version = board_version;

	if (hardware_version == O2_v_1_1_2 || hardware_version == O2_v_0_9)
	{
#ifdef CONFIG_OWNTECH_UART_API		
		spin.uart.usart1SwapRxTx();
#endif
	}else if(hardware_version == SPIN_v_0_1){
#ifdef CONFIG_OWNTECH_UART_API		
		spin.uart.usart1SwapRxTx();
#endif
	}else if(hardware_version == SPIN_v_0_9 || hardware_version == TWIST_v_1_1_2){
	}else if(hardware_version == SPIN_v_1_0){
	}else if(hardware_version == nucleo_G474RE){
	}

}

board_version_t VersionHAL::getBoardVersion()
{
	return board_version;
}
