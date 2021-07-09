/*
 * Copyright (c) 2021 LAAS-CNRS
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
 * @author  Clément Foucher <clement.foucher@laas.fr>
 */


// Zephyr
#include <zephyr.h>
#include <device.h>

// STM32 LL
#include "stm32_ll_bus.h"
#include "stm32_ll_system.h"


static int vrefbuf_init(const struct device* dev)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_VREFBUF_SetVoltageScaling(LL_VREFBUF_VOLTAGE_SCALE0);
	LL_VREFBUF_DisableHIZ();
	LL_VREFBUF_Enable();

	return 0;
}

DEVICE_DEFINE(vrefbuf_driver,
              "vrefbuf_driver",
              vrefbuf_init,
              device_pm_control_nop,
              NULL,
              NULL,
              PRE_KERNEL_1,
              CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
              NULL
             );
