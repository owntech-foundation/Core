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
#ifndef STM32_TIMER_DRIVER_H_
#define STM32_TIMER_DRIVER_H_


// Zephyr
#include <device.h>

// Common driver header
#include "timer.h"


#ifdef __cplusplus
extern "C" {
#endif


#define TIMER6_NODELABEL DT_NODELABEL(timers6)
#define TIMER6_LABEL     DT_PROP(TIMER6_NODELABEL, label)

#define TIMER6_INTERRUPT_LINE DT_IRQN(TIMER6_NODELABEL)
#define TIMER6_INTERRUPT_PRIO DT_IRQ_BY_IDX(TIMER6_NODELABEL, 0, priority)

/**
 * Members of this structure marked with a "§"
 * have to be set when calling DEVICE_DEFINE.
 *
 * timer_struct§:   store the STM32 LL timer structure
 * interrupt_line§: interrupt line number (if interrupt has to be enabled)
 * interrupt_prio$: interrupt priority (if interrupt has to be enabled)
 * timer_callback:  user-defined, set by the timer_config call (if interrupt has to be enabled).
 *                  Should be set to NULL in DEVICE_DEFINE
 */
struct stm32_timer_driver_data
{
	TIM_TypeDef*     timer_struct;
    unsigned int     interrupt_line;
    unsigned int     interrupt_prio;
    timer_callback_t timer_callback;
};

static int timer_stm32_init(const struct device* dev);
void timer_stm32_config(const struct device* dev, const struct timer_config_t* config);
void timer_stm32_start(const struct device* dev, uint32_t t_usec);
uint32_t timer_stm32_get_count(const struct device* dev);
void timer_stm32_clear(const struct device* dev);

void init_timer_6();


#ifdef __cplusplus
}
#endif


#endif // STM32_TIMER_DRIVER_H_
