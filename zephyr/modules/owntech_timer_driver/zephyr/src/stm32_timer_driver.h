/*
 * Copyright (c) 2021-2022 LAAS-CNRS
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
#ifndef STM32_TIMER_DRIVER_H_
#define STM32_TIMER_DRIVER_H_


// Zephyr
#include <device.h>

// Common driver header
#include "timer.h"


#define TIMER4_NODE           DT_NODELABEL(timers4)
#define TIMER4_INTERRUPT_LINE DT_IRQN(TIMER4_NODE)
#define TIMER4_INTERRUPT_PRIO DT_IRQ_BY_IDX(TIMER4_NODE, 0, priority)

#define TIMER6_NODE           DT_NODELABEL(timers6)
#define TIMER6_INTERRUPT_LINE DT_IRQN(TIMER6_NODE)
#define TIMER6_INTERRUPT_PRIO DT_IRQ_BY_IDX(TIMER6_NODE, 0, priority)

#define TIMER7_NODE           DT_NODELABEL(timers7)
#define TIMER7_INTERRUPT_LINE DT_IRQN(TIMER7_NODE)
#define TIMER7_INTERRUPT_PRIO DT_IRQ_BY_IDX(TIMER7_NODE, 0, priority)


#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	periodic_interrupt,
	incremental_coder
} timer_mode_t;


/**
 * Members of this structure marked with a "§"
 * have to be set when calling DEVICE_DEFINE.
 *
 * timer_struct§:          stores the STM32 LL timer structure
 * interrupt_line§:        interrupt line number (if interrupt has to be enabled)
 * interrupt_prio$:        interrupt priority (if interrupt has to be enabled)
 * timer_mode:             Mode in which the timer is configured.
 * timer_irq_callback:     user-defined, set by the timer_config call (if interrupt has to be enabled).
 *                         Should be set to NULL in DEVICE_DEFINE
 * timer_irq_period_usec : period of the irq in microseconds.
 */
struct stm32_timer_driver_data
{
	TIM_TypeDef*     timer_struct;
	unsigned int     interrupt_line;
	unsigned int     interrupt_prio;
	timer_mode_t     timer_mode;
	timer_callback_t timer_irq_callback;
	uint32_t         timer_irq_period_usec;
};

static int timer_stm32_init(const struct device* dev);
void timer_stm32_config(const struct device* dev, const struct timer_config_t* config);
void timer_stm32_start(const struct device* dev);
void timer_stm32_stop(const struct device* dev);
uint32_t timer_stm32_get_count(const struct device* dev);
void timer_stm32_clear(const struct device* dev);

void init_timer_4();
void init_timer_6();
void init_timer_7();


#ifdef __cplusplus
}
#endif


#endif // STM32_TIMER_DRIVER_H_
