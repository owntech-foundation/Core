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
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


#include "stm32_ll_hrtim.h"
#include "stm32_ll_gpio.h"
#include "stm32g4xx_ll_bus.h"

void sync_master_init()
{
	LL_HRTIM_TIM_CounterDisable(HRTIM1, LL_HRTIM_TIMER_A);

	// SYNCOUT[1:0] and SYNCSRC[1:0] bitfield configuration in HRTIM_MCR
	LL_HRTIM_ConfigSyncOut(HRTIM1, LL_HRTIM_SYNCOUT_POSITIVE_PULSE, LL_HRTIM_SYNCOUT_SRC_TIMA_START);

	// HRTIM_SCOUT pin configuration
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

	// the SyncIN pin is disabled here, it will be enabled in the ctrl task to ensure synchronization between master and slave.
	LL_GPIO_SetPinSpeed     (GPIOB, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_1, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull      (GPIOB, LL_GPIO_PIN_1, LL_GPIO_PULL_NO);
	LL_GPIO_SetAFPin_0_7    (GPIOB, LL_GPIO_PIN_1, LL_GPIO_AF_13);

	LL_HRTIM_TIM_CounterEnable(HRTIM1, LL_HRTIM_TIMER_A);
}

void sync_slave_init()
{
	LL_HRTIM_TIM_CounterDisable(HRTIM1, LL_HRTIM_TIMER_MASTER);

	//  HRTIM synchronization input source
	LL_HRTIM_SetSyncInSrc(HRTIM1, LL_HRTIM_SYNCIN_SRC_EXTERNAL_EVENT);

	// Enable the master timer reset when receiving a synchronization input event
	LL_HRTIM_TIM_EnableResetOnSync(HRTIM1, LL_HRTIM_TIMER_MASTER);

	// HRTIM_SCIN pin configuration
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

	LL_GPIO_SetPinMode      (GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed     (GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull      (GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_NO);
	LL_GPIO_SetAFPin_0_7    (GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_12);

	LL_HRTIM_TIM_CounterEnable(HRTIM1, LL_HRTIM_TIMER_MASTER);
}
