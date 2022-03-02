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


// Zephyr
#include <zephyr.h>

// STM32LL
#include <stm32_ll_dac.h>

// Owntech driver
#include "dac.h"



void dac_config_dac1_dac3_current_mode_init()
{
	const struct device* dac1 = device_get_binding(DAC1_DEVICE);
	const struct device* dac3 = device_get_binding(DAC3_DEVICE);

	dac_function_config_t function_config =
	{
		.dac_function = dac_function_sawtooth,
		.trigger_source = LL_DAC_TRIG_EXT_HRTIM_RST_TRG1,
		.step_trigger_source = LL_DAC_TRIG_EXT_HRTIM_STEP_TRG1,
		.polarity = LL_DAC_SAWTOOTH_POLARITY_DECREMENT,
		.reset_data = 4000,
		.step_data = 200
	};

	// DAC 1
	dac_set_function(dac1, 1, &function_config);
	dac_pin_configure(dac1, 1, dac_pin_internal);
	dac_start(dac1, 1);

	// DAC 3
	function_config.trigger_source = LL_DAC_TRIG_EXT_HRTIM_RST_TRG2;
	function_config.step_trigger_source = LL_DAC_TRIG_EXT_HRTIM_STEP_TRG2;

	dac_set_function(dac3, 1, &function_config);
	dac_pin_configure(dac3, 1, dac_pin_internal);
	dac_start(dac3, 1);
}
