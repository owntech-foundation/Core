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

#ifndef TIMER_H_
#define TIMER_H_


// Zephyr
#include <zephyr.h>
#include <device.h>


#ifdef __cplusplus
extern "C" {
#endif


// Configuration structure

typedef void (*timer_callback_t)();

struct timer_config_t
{
	uint32_t         timer_enable_irq : 1;
	timer_callback_t timer_callback;
};

// API

typedef void     (*timer_api_config)   (const struct device* dev, const struct timer_config_t* config);
typedef void     (*timer_api_start)    (const struct device* dev, uint32_t t_usec);
typedef uint32_t (*timer_api_get_count)(const struct device* dev);

__subsystem struct timer_driver_api
{
    timer_api_config    config;
	timer_api_start     start;
	timer_api_get_count get_count;
};


static inline void timer_config(const struct device* dev, const struct timer_config_t* config)
{
	const struct timer_driver_api* api = (const struct timer_driver_api*)(dev->api);

	api->config(dev, config);
}

static inline void timer_start(const struct device* dev, uint32_t t_usec)
{
	const struct timer_driver_api* api = (const struct timer_driver_api*)(dev->api);

	api->start(dev, t_usec);
}

static inline uint32_t timer_get_count(const struct device* dev)
{
	const struct timer_driver_api* api = (const struct timer_driver_api*)(dev->api);

	return api->get_count(dev);
}


#ifdef __cplusplus
}
#endif

#endif // TIMER_H_
