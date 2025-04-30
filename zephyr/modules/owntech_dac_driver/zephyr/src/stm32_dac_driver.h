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
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef STM32_DAC_DRIVER_H_
#define STM32_DAC_DRIVER_H_

/* Zephyr */
#include <zephyr/device.h>

/* Public API */
#include "dac.h"


#define DAC1_NODE DT_NODELABEL(dac1)
#define DAC2_NODE DT_NODELABEL(dac2)
#define DAC3_NODE DT_NODELABEL(dac3)


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration for DAC operating modes.
 *
 * This enum defines the available operation modes for the DAC peripheral.
 *
 * - `dac_mode_unset`: indicates that no mode has been selected.
 *
 * - `dac_mode_constant`: sets the DAC output to a constant value.
 *
 * - `dac_mode_function`: generates a waveform function such as a ramp.
 *
 */
typedef enum
{
	dac_mode_unset,
	dac_mode_constant,
	dac_mode_function
} dac_mode_t;

/**
 * @brief Runtime data structure for the STM32 DAC driver.
 *
 * This structure holds the internal state and configuration for a DAC
 * peripheral instance.
 *
 * - `dac_struct`: points to the hardware DAC instance registers.
 *
 * - `dac_mode`: stores the operating mode for each DAC channel.
 *
 * - `started`: flags indicate whether each DAC channel is active.
 *
 * - `dac_config`: stores either a constant value or a function configuration
 *   for each DAC channel.
 *
 */
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


/**
 * @brief Initialize the STM32 DAC hardware and bind it to the Zephyr device.
 *
 * This function enables peripheral clocks, configures the DAC registers,
 * and prepares the device for analog output. 
 * 
 * It is typically called
 * automatically by Zephyr during system initialization.
 *
 * @param dev Pointer to the DAC device instance.
 *
 * @return 0 on success, negative errno code on failure.
 */
static int  dac_stm32_init(const struct device* dev);

/**
 * @brief Set a constant analog output value on a DAC channel.
 *
 * Writes a 12-bit value directly to the DAC register, outputting a
 * corresponding analog voltage on the specified channel.
 *
 * @param dev     Pointer to the DAC device instance.
 * @param channel DAC channel index.
 * @param value   DAC output value (0–4095 for 12-bit mode).
 */
static void dac_stm32_set_const_value(const struct device* dev,
									  uint8_t channel,
									  uint32_t value);

									  /**
 * @brief Set a predefined waveform (noise, triangle, or sawtooth) on a DAC channel.
 *
 * Configures the DAC to output a hardware-generated waveform using
 * specific trigger sources and shape parameters.
 *
 * @param dev             Pointer to the DAC device instance.
 * @param channel         DAC channel index.
 * @param function_config Pointer to the waveform function configuration structure.
 */
static void dac_stm32_set_function(
	const struct device* dev,
	uint8_t channel,
	const dac_function_config_t* function_config
);

/**
 * @brief Update the reset value for a sawtooth waveform on a DAC channel.
 *
 * Adjusts the base value to which the waveform resets after a full cycle.
 *
 * @param dev        Pointer to the DAC device instance.
 * @param channel    DAC channel index.
 * @param reset_data New reset value to apply.
 */
static void dac_stm32_function_update_reset(const struct device* dev,
											uint8_t channel,
											uint32_t reset_data);

/**
 * @brief Update the step size for a sawtooth waveform on a DAC channel.
 *
 * Defines how much the DAC value increases or decreases with each step trigger.
 *
 * @param dev       Pointer to the DAC device instance.
 * @param channel   DAC channel index.
 * @param step_data New step size to apply.
 */
static void dac_stm32_function_update_step(const struct device* dev,
										   uint8_t channel,
										   uint32_t step_data);

/**
 * @brief Configure the DAC output routing (internal, external, or both).
 *
 * Sets whether the DAC output is available on internal buses, external
 * GPIO pins, or both.
 *
 * @param dev        Pointer to the DAC device instance.
 * @param channel    DAC channel index.
 * @param pin_config Pin routing configuration value.
 */
static void dac_stm32_pin_configure(const struct device* dev,
									uint8_t channel,
									dac_pin_config_t pin_config);

/**
 * @brief Start the output on a DAC channel.
 *
 * Enables analog output generation for the selected channel.
 *
 * @param dev     Pointer to the DAC device instance.
 * @param channel DAC channel index to start.
 */
static void dac_stm32_start(const struct device* dev, uint8_t channel);

/**
 * @brief Stop the output on a DAC channel.
 *
 * Disables analog output for the selected channel.
 *
 * @param dev     Pointer to the DAC device instance.
 * @param channel DAC channel index to stop.
 */
static void dac_stm32_stop(const struct device* dev, uint8_t channel);


#ifdef __cplusplus
}
#endif

#endif /* STM32_DAC_DRIVER_H_ */
