/*
 * Copyright (c) 2021-present LAAS-CNRS
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

/*
 * @date   2022
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef STM32_TIMER_DRIVER_H_
#define STM32_TIMER_DRIVER_H_


/* Zephyr */
#include <zephyr/device.h>

/* Common driver header */
#include "timer.h"


#define TIMER4_NODE           DT_NODELABEL(timers4)
#define TIMER4_INTERRUPT_LINE DT_IRQN(TIMER4_NODE)
#define TIMER4_INTERRUPT_PRIO DT_IRQ_BY_IDX(TIMER4_NODE, 0, priority)

#define TIMER3_NODE           DT_NODELABEL(timers3)
#define TIMER3_INTERRUPT_LINE DT_IRQN(TIMER3_NODE)
#define TIMER3_INTERRUPT_PRIO DT_IRQ_BY_IDX(TIMER3_NODE, 0, priority)

#define TIMER6_NODE           DT_NODELABEL(timers6)
#define TIMER6_INTERRUPT_LINE DT_IRQN(TIMER6_NODE)
#define TIMER6_INTERRUPT_PRIO DT_IRQ_BY_IDX(TIMER6_NODE, 0, priority)

#define TIMER7_NODE           DT_NODELABEL(timers7)
#define TIMER7_INTERRUPT_LINE DT_IRQN(TIMER7_NODE)
#define TIMER7_INTERRUPT_PRIO DT_IRQ_BY_IDX(TIMER7_NODE, 0, priority)


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Defines the timer modes available.
 *
 * - `periodic_interrupt`: timer to create period interruptions
 * 
 * - `incremental_coder`: timer to be used with an incremental encoder for motor
 * 					    control
 */
typedef enum
{
	periodic_interrupt,
	incremental_coder
} timer_mode_t;


/**
 * @brief Members of this structure marked with a "§" have to be set when 
 * 		  calling DEVICE_DEFINE.
 *
 * - `timer_struct (§)`: stores the STM32 LL timer structure
 * 
 * - `interrupt_line (§)`: interrupt line number (if interrupt has to be enabled)
 * 
 * - `interrupt_prio (§)`: interrupt priority (if interrupt has to be enabled)
 * 
 * - `timer_mode`: Mode in which the timer is configured.
 * 
 * - `timer_irq_callback`: user-defined, set by the timer_config call
 * 						 (if interrupt has to be enabled).
 *                       Should be set to `NULL` in structure passed to
 * 						 `DEVICE_DT_DEFINE`
 * 
 * - `timer_irq_period_usec`:  period of the irq in microseconds.
 * 
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

/**
 * @brief Initialize the STM32 timer device.
 *
 * Enables the peripheral clock and sets up basic hardware configuration
 * for the specified timer. This function is typically called during
 * device initialization.
 *
 * @param dev Pointer to the Zephyr device structure representing the timer.
 * @return 0 on success, negative value on error.
 */
static int timer_stm32_init(const struct device* dev);


/**
 * @brief Configure timer behavior and register callback.
 *
 * Applies the user-provided configuration such as period, interrupt enabling,
 * and ISR function. Typically called once before starting the timer.
 *
 * @param dev Pointer to the timer device.
 * @param config Pointer to the timer configuration structure.
 */
void timer_stm32_config(const struct device* dev,
						const struct timer_config_t* config);

/**
 * @brief Start the STM32 timer.
 *
 * Enables the timer counter and optionally starts triggering interrupts
 * or events depending on the configuration.
 *
 * @param dev Pointer to the timer device.
 */
void timer_stm32_start(const struct device* dev);

/**
 * @brief Stop the STM32 timer.
 *
 * Halts the timer counter. Timer can be restarted later using
 * `timer_stm32_start()`.
 *
 * @param dev Pointer to the timer device.
 */
void timer_stm32_stop(const struct device* dev);

/**
 * @brief Get the current counter value of the timer.
 *
 * Returns the present count value of the running or stopped timer.
 *
 * @param dev Pointer to the timer device.
 * @return Current counter value as 32-bit unsigned integer.
 */
uint32_t timer_stm32_get_count(const struct device* dev);

/**
 * @brief Clear the timer counter.
 *
 * Resets the timer counter to zero.
 *
 * @param dev Pointer to the timer device.
 */
void timer_stm32_clear(const struct device* dev);


/**
 * @brief Initialize `TIM4` in incremental encoder mode.
 *
 * This function configures TIM4 as a quadrature encoder interface using
 * STM32 LL (Low Layer) drivers. It sets up input capture channels, filters,
 * polarity, encoder mode, and index signal behavior.
 *
 * - Enables the peripheral clock for `TIM4`.
 *
 * - Initializes `TIM4` with no prescaler and 16-bit auto-reload.
 *
 * - Sets encoder mode to X4 using both TI1 and TI2 signals.
 *
 * - Configures input capture channels CH1 and CH2 for rising edge detection,
 *   filter settings, and direct input mapping.
 *
 * - Disables master/slave mode and trigger output.
 *
 * - Configures the external trigger and index signal parameters.
 *
 * - Enables the encoder index feature.
 *
 */
void init_timer_3();
void init_timer_4();


/**
 * @brief Initialize `TIM6` as a basic timer with microsecond resolution.
 *
 * This function sets up TIM6 using STM32 LL drivers as a basic up-counting
 * timer with a tick resolution of 0.1 microseconds. It does not use ARR
 * preload, master/slave mode, or advanced triggering features.
 *
 * - Enables the peripheral clock for `TIM6`.
 *
 * - Configures the prescaler based on the system clock to achieve a
 *   0.1µs time base.
 *
 * - Sets the timer to up-counting mode.
 *
 * - Disables auto-reload register preload.
 *
 * - Disables master/slave mode and sets trigger output to reset.
 *
 */
void init_timer_6();

/**
 * @brief Initialize `TIM7` as a basic timer with microsecond resolution.
 *
 * This function configures `TIM7` using STM32 LL drivers as a basic
 * up-counting timer with a tick resolution of 0.1 microseconds. The timer
 * operates without auto-reload preload or master/slave synchronization.
 *
 * - Enables the peripheral clock for `TIM7`.
 *
 * - Configures the prescaler to generate a 0.1µs time base.
 *
 * - Sets the counter mode to up-counting.
 *
 * - Disables auto-reload register preload.
 *
 * - Sets trigger output to reset and disables master/slave mode.
 *
 */
void init_timer_7();


#ifdef __cplusplus
}
#endif


#endif /* STM32_TIMER_DRIVER_H_ */
