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
 *
 * @brief  This file is the public include file for the
 *         Zephyr Timer driver. It provides basic functionnality
 *         to handle STM32 Timers. Is is for now specific
 *         to certain capabilities of G4 series Timers,
 *         and mainly restricted to the use we do of
 *         the timers in the OwnTech project, but it aims
 *         at becoming more generic over time.
 *
 *         This version suports:
 *         * Timer 6 and Timer 7: Periodic call of a callback function with period ranging from 2 to 6553 µs.
 *         * Timer 4: Incremental coder acquisition with pinout: reset=PB3; CH1=PB6; CH2=PB7.
 */

#ifndef TIMER_H_
#define TIMER_H_


// Zephyr
#include <zephyr.h>
#include <device.h>


#ifdef __cplusplus
extern "C" {
#endif


/////
// Public devices names

#define TIMER4_DEVICE DT_NODELABEL(timers4)
#define TIMER6_DEVICE DT_NODELABEL(timers6)
#define TIMER7_DEVICE DT_NODELABEL(timers7)


/////
// Configuration structure

typedef void (*timer_callback_t)();

typedef enum
{
	no_pull,
	pull_up,
	pull_down
} pin_mode_t;

/**
 * timer_enable_irq     : set to 1 to enable interrupt on timer overflow.
 * timer_enable_encoder : set to 1 for timer to act as an incremental coder counter.
 *
 * *** IRQ mode (ignored if timer_enable_irq=0) ***
 * - timer_irq_callback : pointer to a void(void) function that will be
 *                        called on timer overflow.
 * - timer_irq_t_usec : period of the interrupt in microsecond (2 to 6553 µs)
 *
 * *** Incremental code mode (ignored if timer_enable_encoder=0) ***
 * - timer_pin_mode : Pin mode for incremental coder interface.
 *
 * NOTE: At this time, only irq mode is supported on TIM6/TIM7, and
 * only incremental coder mode is suppported on TIM4, which makes this
 * configuration structure almost pointless (except for callback definition).
 * However, it is built this way with future evolutions of the driver
 * in mind.
 */
struct timer_config_t
{
	uint32_t         timer_enable_irq     : 1;
	uint32_t         timer_enable_encoder : 1;
	timer_callback_t timer_irq_callback;
	uint32_t         timer_irq_t_usec;
	pin_mode_t       timer_enc_pin_mode;
};


/////
// API

typedef void     (*timer_api_config)   (const struct device* dev, const struct timer_config_t* config);
typedef void     (*timer_api_start)    (const struct device* dev);
typedef void     (*timer_api_stop)     (const struct device* dev);
typedef uint32_t (*timer_api_get_count)(const struct device* dev);

__subsystem struct timer_driver_api
{
	timer_api_config    config;
	timer_api_start     start;
	timer_api_stop      stop;
	timer_api_get_count get_count;
};


/**
 * Configure the timer dev using given configuration structure config.
 *
 * @param dev    Zephyr device representing the timer.
 * @param config Configuration holding the timer configuration.
 */
static inline void timer_config(const struct device* dev, const struct timer_config_t* config)
{
	const struct timer_driver_api* api = (const struct timer_driver_api*)(dev->api);

	api->config(dev, config);
}

/**
 * Start the timer dev. If timer is configured to provide a perdiodic
 * interrupt, it will also enable it.
 *
 * @param dev Zephyr device representing the timer.
 */
static inline void timer_start(const struct device* dev)
{
	const struct timer_driver_api* api = (const struct timer_driver_api*)(dev->api);

	api->start(dev);
}

/**
 * Stop the timer dev. If timer is configured to provide a perdiodic
 * interrupt, it will also disable it.
 *
 * @param dev Zephyr device representing the timer.
 */
static inline void timer_stop(const struct device* dev)
{
	const struct timer_driver_api* api = (const struct timer_driver_api*)(dev->api);

	api->stop(dev);
}

/**
 * Get the current timer counter value.
 *
 * @param  dev Zephyr device representing the timer.
 * @return     Current value of the timer internal counter.
 */
static inline uint32_t timer_get_count(const struct device* dev)
{
	const struct timer_driver_api* api = (const struct timer_driver_api*)(dev->api);

	return api->get_count(dev);
}


#ifdef __cplusplus
}
#endif

#endif // TIMER_H_
