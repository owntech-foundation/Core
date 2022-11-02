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
 *
 * @brief  This file automatically performs some hardware configuration
 *         using Zephyr macros.
 *         Configuration done in this file is low-level peripheral configuration
 *         required for OwnTech board to operate, do not mess with it unless you
 *         are absolutely sure of what you're doing.
 *         This file does not contain any public function.
 */


// Zephyr
#include <zephyr.h>
#include <device.h>

// STM32 LL
#include <stm32_ll_bus.h>
#include <stm32_ll_system.h>

// Owntech driver
#include "dac.h"

static const struct device* dac2 = DEVICE_DT_GET(DAC2_DEVICE);

/////
// Functions to be run

static int _vrefbuf_init(const struct device* dev)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_VREFBUF_SetVoltageScaling(LL_VREFBUF_VOLTAGE_SCALE0);
	LL_VREFBUF_DisableHIZ();
	LL_VREFBUF_Enable();

	return 0;
}

static int _dac2_init(const struct device* dev)
{
	if (device_is_ready(dac2) == true)
	{
		dac_set_const_value(dac2, 1, 2048U);
		dac_pin_configure(dac2, 1, dac_pin_external);
		dac_start(dac2, 1);
	}

	return 0;
}


/////
// Zephyr macros to automatically run above functions

SYS_INIT(_vrefbuf_init,
         PRE_KERNEL_1, // To be run in the first init phase
         CONFIG_KERNEL_INIT_PRIORITY_DEVICE
        );

SYS_INIT(_dac2_init,
         PRE_KERNEL_2, // To be run in the second init phase (depends on DAC driver initialization)
         CONFIG_KERNEL_INIT_PRIORITY_DEVICE
        );
