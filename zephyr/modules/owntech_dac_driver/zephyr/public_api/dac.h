/*
 * Copyright (c) 2021-2023 LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


#ifndef DAC_H_
#define DAC_H_

// Zephyr
#include <zephyr.h>
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif


/////
// Public devices names

#define DAC1_DEVICE DT_NODELABEL(dac1)
#define DAC2_DEVICE DT_NODELABEL(dac2)
#define DAC3_DEVICE DT_NODELABEL(dac3)


/////
// Configuration types

typedef enum
{
	dac_function_noise,
	dac_function_triangle,
	dac_function_sawtooth
} dac_function_t;

typedef enum
{
	dac_polarity_decrement,
	dac_polarity_increment
} dac_polarity_t;

typedef enum
{
	hrtim_trig1,
	hrtim_trig2,
	hrtim_trig3,
	hrtim_trig4,
	hrtim_trig5,
	hrtim_trig6 
} dac_trigger_t;

typedef struct
{
	dac_function_t dac_function;
	dac_trigger_t  reset_trigger_source;
	dac_trigger_t  step_trigger_source;
	dac_polarity_t polarity;
	uint32_t       reset_data;
	uint32_t       step_data;
} dac_function_config_t;

typedef enum
{
	dac_pin_internal,
	dac_pin_external,
	dac_pin_internal_and_external
} dac_pin_config_t;

/////
// API

typedef void (*dac_api_setconstvalue) (const struct device* dev, uint8_t channel, uint32_t value);
typedef void (*dac_api_setfunction)   (const struct device* dev, uint8_t channel, const dac_function_config_t* config);
typedef void (*dac_api_fn_upd_reset)  (const struct device* dev, uint8_t channel, uint32_t reset_data);
typedef void (*dac_api_fn_upd_step)   (const struct device* dev, uint8_t channel, uint32_t step_data);
typedef void (*dac_api_pinconfigure)  (const struct device* dev, uint8_t channel, dac_pin_config_t config);
typedef void (*dac_api_start)         (const struct device* dev, uint8_t channel);
typedef void (*dac_api_stop)          (const struct device* dev, uint8_t channel);

__subsystem struct dac_driver_api
{
    dac_api_setconstvalue setconstvalue;
	dac_api_setfunction   setfunction;
	dac_api_fn_upd_reset  fn_upd_reset;
	dac_api_fn_upd_step   fn_upd_step;
	dac_api_pinconfigure  pinconfigure;
	dac_api_start         start;
	dac_api_stop          stop;
};

static inline void dac_set_const_value(const struct device* dev, uint8_t channel, uint32_t value)
{
	const struct dac_driver_api* api = (const struct dac_driver_api*)(dev->api);

	api->setconstvalue(dev, channel, value);
}

static inline void dac_set_function(const struct device* dev, uint8_t channel, const dac_function_config_t* function_config)
{
	const struct dac_driver_api* api = (const struct dac_driver_api*)(dev->api);

	api->setfunction(dev, channel, function_config);
}

static inline void dac_function_update_reset(const struct device* dev, uint8_t channel, uint32_t reset_data)
{
	const struct dac_driver_api* api = (const struct dac_driver_api*)(dev->api);

	api->fn_upd_reset(dev, channel, reset_data);
}

static inline void dac_function_update_step(const struct device* dev, uint8_t channel, uint32_t step_data)
{
	const struct dac_driver_api* api = (const struct dac_driver_api*)(dev->api);

	api->fn_upd_step(dev, channel, step_data);
}

static inline void dac_pin_configure(const struct device* dev, uint8_t channel, dac_pin_config_t pin_config)
{
	const struct dac_driver_api* api = (const struct dac_driver_api*)(dev->api);

	api->pinconfigure(dev, channel, pin_config);
}

static inline void dac_start(const struct device* dev, uint8_t channel)
{
	const struct dac_driver_api* api = (const struct dac_driver_api*)(dev->api);

	api->start(dev, channel);
}

static inline void dac_stop(const struct device* dev, uint8_t channel)
{
	const struct dac_driver_api* api = (const struct dac_driver_api*)(dev->api);

	api->stop(dev, channel);
}

#ifdef __cplusplus
}
#endif

#endif // DAC_H_
