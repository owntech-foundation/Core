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
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef STM32_DAC_DRIVER_H_
#define STM32_DAC_DRIVER_H_

// Zephyr
#include <device.h>

// Public API
#include "dac.h"


#define DAC1_NODE DT_NODELABEL(dac1)
#define DAC2_NODE DT_NODELABEL(dac2)
#define DAC3_NODE DT_NODELABEL(dac3)


#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	dac_mode_unset,
	dac_mode_constant,
	dac_mode_function
} dac_mode_t;

struct stm32_dac_driver_data
{
	DAC_TypeDef* dac_struct;
	dac_mode_t   dac_mode[2];
	uint8_t      started[2];
	union
	{
		uint32_t              constant_value;
		dac_function_config_t function_config;
	} dac_config[2];
};



static int  dac_stm32_init(const struct device* dev);
static void dac_stm32_set_const_value(const struct device* dev, uint8_t channel, uint32_t value);
static void dac_stm32_set_function(const struct device* dev, uint8_t channel, const dac_function_config_t* function_config);
static void dac_stm32_function_update_reset(const struct device* dev, uint8_t channel, uint32_t reset_data);
static void dac_stm32_function_update_step(const struct device* dev, uint8_t channel, uint32_t step_data);
static void dac_stm32_pin_configure(const struct device* dev, uint8_t channel, dac_pin_config_t pin_config);
static void dac_stm32_start(const struct device* dev, uint8_t channel);
static void dac_stm32_stop(const struct device* dev, uint8_t channel);


#ifdef __cplusplus
}
#endif

#endif // STM32_DAC_DRIVER_H_
