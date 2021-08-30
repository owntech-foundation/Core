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


// STM32 LL
#include <stm32_ll_tim.h>
#include <stm32_ll_bus.h>

// Current file header
#include "stm32_timer_driver.h"


/////
// Init function

static int timer_stm32_init(const struct device* dev)
{
	TIM_TypeDef* tim_dev = ((struct stm32_timer_driver_data*)dev->data)->timer_struct;

	if (tim_dev == TIM6)
		init_timer_6();
	else if (tim_dev == TIM7)
		init_timer_7();
	else
		return -1;

	return 0;
}


/////
// Callback

static void timer_stm32_callback(const void* arg)
{
	const struct device* timer_dev = (const struct device*)arg;
	struct stm32_timer_driver_data* data = (struct stm32_timer_driver_data*)timer_dev->data;

	timer_stm32_clear(timer_dev);

	if (data->timer_callback != NULL)
	{
		data->timer_callback();
	}
}


/////
// API

static const struct timer_driver_api timer_funcs =
{
	.config    = timer_stm32_config,
	.start     = timer_stm32_start,
	.get_count = timer_stm32_get_count
};

void timer_stm32_config(const struct device* dev, const struct timer_config_t* config)
{
	struct stm32_timer_driver_data* data = (struct stm32_timer_driver_data*)dev->data;
	TIM_TypeDef* tim_dev = data->timer_struct;

	if (tim_dev != NULL)
	{
		if (config->timer_enable_irq == 1)
		{
			data->timer_callback = config->timer_callback;
			irq_connect_dynamic(data->interrupt_line, data->interrupt_prio, timer_stm32_callback, dev, 0);
			irq_enable(data->interrupt_line);
		}
	}
}

void timer_stm32_start(const struct device* dev, uint32_t t_usec)
{
	TIM_TypeDef* tim_dev = ((struct stm32_timer_driver_data*)dev->data)->timer_struct;

	if (tim_dev != NULL)
	{
		LL_TIM_SetAutoReload(tim_dev, (t_usec*10) - 1);
		LL_TIM_EnableIT_UPDATE(tim_dev);
		LL_TIM_EnableCounter(tim_dev);
	}
}

void timer_stm32_clear(const struct device* dev)
{
	TIM_TypeDef* tim_dev = ((struct stm32_timer_driver_data*)dev->data)->timer_struct;

	if (tim_dev != NULL)
	{
		LL_TIM_ClearFlag_UPDATE(tim_dev);
	}
}

uint32_t timer_stm32_get_count(const struct device* dev)
{
	TIM_TypeDef* tim_dev = ((struct stm32_timer_driver_data*)dev->data)->timer_struct;

	return LL_TIM_GetCounter(tim_dev);
}


/////
// Per-timer inits

void init_timer_6()
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

	// Peripheral clock enable
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);

	// TIM6 interrupt Init
	NVIC_SetPriority(TIM6_DAC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));

	TIM_InitStruct.Prescaler = (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC/1e7) - 1; // Set prescaler to tick to 0.1µs
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	LL_TIM_Init(TIM6, &TIM_InitStruct);
	LL_TIM_DisableARRPreload(TIM6);
	LL_TIM_SetTriggerOutput(TIM6, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM6);
}

void init_timer_7()
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

	// Peripheral clock enable
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);

	// TIM7 interrupt Init
	NVIC_SetPriority(TIM7_DAC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));

	TIM_InitStruct.Prescaler = (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC/1e7) - 1; // Set prescaler to tick to 0.1µs
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	LL_TIM_Init(TIM7, &TIM_InitStruct);
	LL_TIM_DisableARRPreload(TIM7);
	LL_TIM_SetTriggerOutput(TIM7, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM7);
}


/////
// Device definitions

// Timer 6
#if DT_NODE_HAS_STATUS(TIMER6_NODELABEL, okay)

struct stm32_timer_driver_data timer6_data =
{
	.timer_struct   = TIM6,
	.interrupt_line = TIMER6_INTERRUPT_LINE,
	.interrupt_prio = TIMER6_INTERRUPT_PRIO,
	.timer_callback = NULL
};

DEVICE_DT_DEFINE(TIMER6_NODELABEL,
                 timer_stm32_init,
                 NULL,
                 &timer6_data,
                 NULL,
                 PRE_KERNEL_1,
                 CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                 &timer_funcs
                );

#endif // Timer 6

// Timer 7
#if DT_NODE_HAS_STATUS(TIMER7_NODELABEL, okay)

struct stm32_timer_driver_data timer7_data =
{
	.timer_struct   = TIM7,
	.interrupt_line = TIMER7_INTERRUPT_LINE,
	.interrupt_prio = TIMER7_INTERRUPT_PRIO,
	.timer_callback = NULL
};

DEVICE_DT_DEFINE(TIMER7_NODELABEL,
                 timer_stm32_init,
                 NULL,
                 &timer7_data,
                 NULL,
                 PRE_KERNEL_1,
                 CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                 &timer_funcs
                );

#endif // Timer 7
