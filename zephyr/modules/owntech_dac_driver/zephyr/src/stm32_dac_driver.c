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
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


// Zephyr
#include <zephyr.h>

// STM32 LL
#include <stm32_ll_dac.h>
#include <stm32_ll_bus.h>

// Current file header
#include "stm32_dac_driver.h"


/////
// Init function

static int dac_stm32_init(const struct device* dev)
{
	DAC_TypeDef* dac_dev = ((struct stm32_dac_driver_data*)dev->data)->dac_struct;

	if (dac_dev == DAC1)
	{
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_DAC1);
	}
	else if (dac_dev == DAC2)
	{
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_DAC2);
	}
	else if (dac_dev == DAC3)
	{
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_DAC3);
	}

	return 0;
}


/////
// API

static const struct dac_driver_api dac_funcs =
{
	.setconstvalue = dac_stm32_set_const_value,
	.setfunction   = dac_stm32_set_function,
	.fn_upd_reset  = dac_stm32_function_update_reset,
	.fn_upd_step   = dac_stm32_function_update_step,
	.pinconfigure  = dac_stm32_pin_configure,
	.start         = dac_stm32_start,
	.stop          = dac_stm32_stop
};

static void dac_stm32_set_const_value(const struct device* dev, uint8_t channel, uint32_t value)
{
	struct stm32_dac_driver_data* data = (struct stm32_dac_driver_data*)dev->data;
	DAC_TypeDef* dac_dev = data->dac_struct;

	uint8_t dac_channel = __LL_DAC_DECIMAL_NB_TO_CHANNEL(channel);

	if (data->dac_mode[channel-1] != dac_mode_constant)
	{
		data->dac_mode[channel-1] = dac_mode_constant;
		data->dac_config->constant_value = value;

		LL_DAC_SetSignedFormat(dac_dev, dac_channel, LL_DAC_SIGNED_FORMAT_DISABLE);

		LL_DAC_SetWaveAutoGeneration(dac_dev, dac_channel, LL_DAC_WAVE_AUTO_GENERATION_NONE);

		LL_DAC_DisableTrigger(dac_dev, dac_channel);
		LL_DAC_DisableDMADoubleDataMode(dac_dev, dac_channel);
	}

	LL_DAC_ConvertData12RightAligned(dac_dev, dac_channel, value);
}

static void dac_stm32_set_function(const struct device* dev, uint8_t channel, const dac_function_config_t* function_config)
{
	struct stm32_dac_driver_data* data = (struct stm32_dac_driver_data*)dev->data;
	DAC_TypeDef* dac_dev = data->dac_struct;

	uint8_t dac_channel = __LL_DAC_DECIMAL_NB_TO_CHANNEL(channel);

	data->dac_mode[channel-1] = dac_mode_function;

	if (function_config->dac_function == dac_function_sawtooth)
	{
		data->dac_config->function_config = *function_config;

		uint32_t reset_trigger_source = LL_DAC_TRIG_EXT_HRTIM_RST_TRG1;
		if (function_config->reset_trigger_source == hrtim_trig2)
		{
			reset_trigger_source = LL_DAC_TRIG_EXT_HRTIM_RST_TRG2;
		}
		else if (function_config->reset_trigger_source == hrtim_trig3)
		{
			reset_trigger_source = LL_DAC_TRIG_EXT_HRTIM_RST_TRG3;
		}
		else if (function_config->reset_trigger_source == hrtim_trig4)
		{
			reset_trigger_source = LL_DAC_TRIG_EXT_HRTIM_RST_TRG4;
		}
		else if (function_config->reset_trigger_source == hrtim_trig5)
		{
			reset_trigger_source = LL_DAC_TRIG_EXT_HRTIM_RST_TRG5;
		}
		else if (function_config->reset_trigger_source == hrtim_trig6)
		{
			reset_trigger_source = LL_DAC_TRIG_EXT_HRTIM_RST_TRG6;
		}

		uint32_t step_trigger_source = LL_DAC_TRIG_EXT_HRTIM_STEP_TRG1;
		if (function_config->step_trigger_source == hrtim_trig2)
		{
			step_trigger_source = LL_DAC_TRIG_EXT_HRTIM_STEP_TRG2;
		}
		else if (function_config->step_trigger_source == hrtim_trig3)
		{
			step_trigger_source = LL_DAC_TRIG_EXT_HRTIM_STEP_TRG3;
		}
		else if (function_config->step_trigger_source == hrtim_trig4)
		{
			step_trigger_source = LL_DAC_TRIG_EXT_HRTIM_STEP_TRG4;
		}
		else if (function_config->step_trigger_source == hrtim_trig5)
		{
			step_trigger_source = LL_DAC_TRIG_EXT_HRTIM_STEP_TRG5;
		}
		else if (function_config->step_trigger_source == hrtim_trig6)
		{
			step_trigger_source = LL_DAC_TRIG_EXT_HRTIM_STEP_TRG6;
		}

		uint32_t polarity = LL_DAC_SAWTOOTH_POLARITY_DECREMENT;
		if (function_config->polarity == dac_polarity_increment)
		{
			polarity = LL_DAC_SAWTOOTH_POLARITY_INCREMENT;
		}

		LL_DAC_SetSignedFormat(dac_dev, dac_channel, LL_DAC_SIGNED_FORMAT_DISABLE);

		LL_DAC_SetWaveAutoGeneration(dac_dev, dac_channel, LL_DAC_WAVE_AUTO_GENERATION_SAWTOOTH);
		LL_DAC_SetWaveSawtoothResetTriggerSource(dac_dev, dac_channel, reset_trigger_source);
		LL_DAC_SetWaveSawtoothStepTriggerSource(dac_dev, dac_channel, step_trigger_source);
		LL_DAC_SetWaveSawtoothPolarity(dac_dev, dac_channel, polarity);
		LL_DAC_SetWaveSawtoothResetData(dac_dev, dac_channel, function_config->reset_data);
		LL_DAC_SetWaveSawtoothStepData(dac_dev, dac_channel, function_config->step_data);

		LL_DAC_EnableTrigger(dac_dev, dac_channel);
		LL_DAC_DisableDMADoubleDataMode(dac_dev, dac_channel);
	}
}

static void dac_stm32_function_update_reset(const struct device* dev, uint8_t channel, uint32_t reset_data)
{
	struct stm32_dac_driver_data* data = (struct stm32_dac_driver_data*)dev->data;
	DAC_TypeDef* dac_dev = data->dac_struct;

	uint8_t dac_channel = __LL_DAC_DECIMAL_NB_TO_CHANNEL(channel);

	if (data->dac_mode[channel-1] == dac_mode_function)
	{
		data->dac_config->function_config.reset_data = reset_data;

		LL_DAC_SetWaveSawtoothResetData(dac_dev, dac_channel, reset_data);
	}
}

static void dac_stm32_function_update_step(const struct device* dev, uint8_t channel, uint32_t step_data)
{
	struct stm32_dac_driver_data* data = (struct stm32_dac_driver_data*)dev->data;
	DAC_TypeDef* dac_dev = data->dac_struct;

	uint8_t dac_channel = __LL_DAC_DECIMAL_NB_TO_CHANNEL(channel);

	if (data->dac_mode[channel-1] == dac_mode_function)
	{
		data->dac_config->function_config.step_data = step_data;

		LL_DAC_SetWaveSawtoothStepData(dac_dev, dac_channel, step_data);	
	}
}

static void dac_stm32_pin_configure(const struct device* dev, uint8_t channel, dac_pin_config_t pin_config)
{
	struct stm32_dac_driver_data* data = (struct stm32_dac_driver_data*)dev->data;
	DAC_TypeDef* dac_dev = data->dac_struct;

	uint8_t dac_channel = __LL_DAC_DECIMAL_NB_TO_CHANNEL(channel);

	if (pin_config == dac_pin_internal)
	{
		LL_DAC_ConfigOutput(dac_dev, dac_channel, LL_DAC_OUTPUT_MODE_NORMAL, LL_DAC_OUTPUT_BUFFER_DISABLE, LL_DAC_OUTPUT_CONNECT_INTERNAL);
	}
	else if (pin_config == dac_pin_external)
	{
		LL_DAC_ConfigOutput(dac_dev, dac_channel, LL_DAC_OUTPUT_MODE_NORMAL, LL_DAC_OUTPUT_BUFFER_ENABLE, LL_DAC_OUTPUT_CONNECT_GPIO);
	}
	else if (pin_config == dac_pin_internal_and_external)
	{
		LL_DAC_ConfigOutput(dac_dev, dac_channel, LL_DAC_OUTPUT_MODE_NORMAL, LL_DAC_OUTPUT_BUFFER_ENABLE, LL_DAC_OUTPUT_CONNECT_INTERNAL);
	}

}

static void dac_stm32_start(const struct device* dev, uint8_t channel)
{
	struct stm32_dac_driver_data* data = (struct stm32_dac_driver_data*)dev->data;
	DAC_TypeDef* dac_dev = data->dac_struct;

	uint8_t dac_channel = __LL_DAC_DECIMAL_NB_TO_CHANNEL(channel);

	if ( (data->dac_mode[channel-1] != dac_mode_unset) && (data->started[channel-1] == 0) )
	{
		LL_DAC_Enable(dac_dev, dac_channel);

		while (LL_DAC_IsReady(dac_dev, dac_channel) == 0)
		{
			// Wait
		}

		data->started[channel-1] = 1;
	}
}

static void dac_stm32_stop(const struct device* dev, uint8_t channel)
{
	struct stm32_dac_driver_data* data = (struct stm32_dac_driver_data*)dev->data;
	DAC_TypeDef* dac_dev = data->dac_struct;

	uint8_t dac_channel = __LL_DAC_DECIMAL_NB_TO_CHANNEL(channel);

	if (data->started[channel-1] == 1)
	{
		LL_DAC_Disable(dac_dev, dac_channel);

		data->started[channel-1] = 0;
	}
}


/////
// Device definitions

// DAC 1
#if DT_NODE_HAS_STATUS(DAC1_NODE, okay)

struct stm32_dac_driver_data dac1_data =
{
	.dac_struct = DAC1,
	.dac_mode   = {dac_mode_unset, dac_mode_unset},
	.started    = {0, 0}
};

DEVICE_DT_DEFINE(DAC1_NODE,
                 dac_stm32_init,
                 NULL,
                 &dac1_data,
                 NULL,
                 PRE_KERNEL_1,
                 CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                 &dac_funcs
                );

#endif // DAC 1

// DAC 2
#if DT_NODE_HAS_STATUS(DAC2_NODE, okay)

struct stm32_dac_driver_data dac2_data =
{
	.dac_struct = DAC2,
	.dac_mode   = {dac_mode_unset, dac_mode_unset},
	.started    = {0, 0}
};

DEVICE_DT_DEFINE(DAC2_NODE,
                 dac_stm32_init,
                 NULL,
                 &dac2_data,
                 NULL,
                 PRE_KERNEL_1,
                 CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                 &dac_funcs
                );

#endif // DAC 2

// DAC 3
#if DT_NODE_HAS_STATUS(DAC3_NODE, okay)

struct stm32_dac_driver_data dac3_data =
{
	.dac_struct = DAC3,
	.dac_mode   = {dac_mode_unset, dac_mode_unset},
	.started    = {0, 0}
};

DEVICE_DT_DEFINE(DAC3_NODE,
                 dac_stm32_init,
                 NULL,
                 &dac3_data,
                 NULL,
                 PRE_KERNEL_1,
                 CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                 &dac_funcs
                );

#endif // DAC 3
