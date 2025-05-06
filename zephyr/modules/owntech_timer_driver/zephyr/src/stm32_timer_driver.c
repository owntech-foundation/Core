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


/* STM32 LL */
#include <stm32_ll_tim.h>
#include <stm32_ll_bus.h>
#include <stm32_ll_gpio.h>

/* Current file header */
#include "stm32_timer_driver.h"


static int timer_stm32_init(const struct device* dev)
{
	TIM_TypeDef* tim_dev =
				((struct stm32_timer_driver_data*)dev->data)->timer_struct;

	if (tim_dev == TIM4)
		init_timer_4();
	else if (tim_dev == TIM3)
		init_timer_3();
	else if (tim_dev == TIM6)
		init_timer_6();
	else if (tim_dev == TIM7)
		init_timer_7();
	else
		return -1;

	return 0;
}

/**
 * @brief Callback function triggered by a timer interrupt.
 *
 * This function is called when the hardware timer generates an interrupt.
 * It clears the interrupt flag and calls the user-defined callback if it
 * has been set.
 *
 * - Casts the generic argument to a device pointer.
 *
 * - Clears the hardware timer interrupt flag.
 *
 * - Invokes the registered callback function, if available.
 *
 * @param arg Pointer to the timer device (cast from a generic void pointer).
 *
 */
static void timer_stm32_callback(const void* arg)
{
	const struct device* timer_dev = (const struct device*)arg;
	struct stm32_timer_driver_data* data =
							(struct stm32_timer_driver_data*)timer_dev->data;

	timer_stm32_clear(timer_dev);

	if (data->timer_irq_callback != NULL)
	{
		data->timer_irq_callback();
	}
}


/** @brief Defines a structure to hold the timer functions   */
static const struct timer_driver_api timer_funcs =
{
	.config    = timer_stm32_config,
	.start     = timer_stm32_start,
	.stop      = timer_stm32_stop,
	.get_count = timer_stm32_get_count
};

void timer_stm32_config(const struct device* dev,
						const struct timer_config_t* config)
{
	struct stm32_timer_driver_data* data =
						(struct stm32_timer_driver_data*)dev->data;

	TIM_TypeDef* tim_dev = data->timer_struct;

	if ( (tim_dev == TIM6) || (tim_dev == TIM7) )
	{
		if (config->timer_enable_irq == 1)
		{
			data->timer_mode = periodic_interrupt;
			data->timer_irq_callback = config->timer_irq_callback;
			data->timer_irq_period_usec = config->timer_irq_t_usec;
			uint32_t flags = 0;

			if (config->timer_use_zero_latency == 1)
			{
				flags = IRQ_ZERO_LATENCY;
			}

			irq_connect_dynamic(data->interrupt_line,
								data->interrupt_prio,
								timer_stm32_callback,
								dev,
								flags);

			irq_enable(data->interrupt_line);
		}
	}
	else if (tim_dev == TIM4)
	{
		if (config->timer_enable_encoder == 1)
		{
			data->timer_mode = incremental_coder;

			uint32_t pull = 0;
			switch (config->timer_enc_pin_mode)
			{
				case no_pull:
					pull = LL_GPIO_PULL_NO;
					break;
				case pull_up:
					pull = LL_GPIO_PULL_UP;
					break;
				case pull_down:
					pull = LL_GPIO_PULL_DOWN;
					break;
			}

			/* Configure GPIO */
			LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

			LL_GPIO_SetPinMode(GPIOB,
							   LL_GPIO_PIN_3,
							   LL_GPIO_MODE_ALTERNATE);

			LL_GPIO_SetPinSpeed(GPIOB,
								LL_GPIO_PIN_3,
								LL_GPIO_SPEED_FREQ_LOW);
			LL_GPIO_SetPinOutputType(GPIOB,
									 LL_GPIO_PIN_3,
									 LL_GPIO_OUTPUT_PUSHPULL);

			LL_GPIO_SetPinPull(GPIOB,
							   LL_GPIO_PIN_3,
							   pull);

			LL_GPIO_SetAFPin_0_7(GPIOB,
								 LL_GPIO_PIN_3,
								 LL_GPIO_AF_2);

			LL_GPIO_SetPinMode(GPIOB,
							   LL_GPIO_PIN_6,
							   LL_GPIO_MODE_ALTERNATE);

			LL_GPIO_SetPinSpeed(GPIOB,
								LL_GPIO_PIN_6,
								LL_GPIO_SPEED_FREQ_LOW);

			LL_GPIO_SetPinOutputType(GPIOB,
									 LL_GPIO_PIN_6,
									 LL_GPIO_OUTPUT_PUSHPULL);

			LL_GPIO_SetPinPull(GPIOB,LL_GPIO_PIN_6,pull);
			LL_GPIO_SetAFPin_0_7(GPIOB,LL_GPIO_PIN_6,LL_GPIO_AF_2);
			LL_GPIO_SetPinMode(GPIOB,LL_GPIO_PIN_7,LL_GPIO_MODE_ALTERNATE);
			LL_GPIO_SetPinSpeed(GPIOB,LL_GPIO_PIN_7,LL_GPIO_SPEED_FREQ_LOW);

			LL_GPIO_SetPinOutputType(GPIOB,
									 LL_GPIO_PIN_7,
									 LL_GPIO_OUTPUT_PUSHPULL);

			LL_GPIO_SetPinPull(GPIOB,LL_GPIO_PIN_7,pull);
			LL_GPIO_SetAFPin_0_7(GPIOB,LL_GPIO_PIN_7,LL_GPIO_AF_2);
		}
	} 
	else if (tim_dev == TIM3)
	{
		if (config->timer_enable_encoder == 1)
		{
			data->timer_mode = incremental_coder;

			uint32_t pull = 0;
			switch (config->timer_enc_pin_mode)
			{
				case no_pull:
					pull = LL_GPIO_PULL_NO;
					break;
				case pull_up:
					pull = LL_GPIO_PULL_UP;
					break;
				case pull_down:
					pull = LL_GPIO_PULL_DOWN;
					break;
			}

			/* Configure GPIO */
			LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);

			LL_GPIO_SetPinMode(GPIOD,
								LL_GPIO_PIN_2,
								LL_GPIO_MODE_ALTERNATE);

			LL_GPIO_SetPinSpeed(GPIOD,
								LL_GPIO_PIN_2,
								LL_GPIO_SPEED_FREQ_LOW);
			LL_GPIO_SetPinOutputType(GPIOD,
										LL_GPIO_PIN_2,
										LL_GPIO_OUTPUT_PUSHPULL);

			LL_GPIO_SetPinPull(GPIOD,
								LL_GPIO_PIN_2,
								pull);

			LL_GPIO_SetAFPin_0_7(GPIOD,
									LL_GPIO_PIN_2,
									LL_GPIO_AF_2);

			LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

			LL_GPIO_SetPinMode(GPIOC,
								LL_GPIO_PIN_6,
								LL_GPIO_MODE_ALTERNATE);

			LL_GPIO_SetPinSpeed(GPIOC,
								LL_GPIO_PIN_6,
								LL_GPIO_SPEED_FREQ_LOW);

			LL_GPIO_SetPinOutputType(GPIOC,
										LL_GPIO_PIN_6,
										LL_GPIO_OUTPUT_PUSHPULL);

			LL_GPIO_SetPinPull(GPIOC,LL_GPIO_PIN_6,pull);
			LL_GPIO_SetAFPin_0_7(GPIOC,LL_GPIO_PIN_6,LL_GPIO_AF_2);
			LL_GPIO_SetPinMode(GPIOC,LL_GPIO_PIN_7,LL_GPIO_MODE_ALTERNATE);
			LL_GPIO_SetPinSpeed(GPIOC,LL_GPIO_PIN_7,LL_GPIO_SPEED_FREQ_LOW);

			LL_GPIO_SetPinOutputType(GPIOC,
										LL_GPIO_PIN_7,
										LL_GPIO_OUTPUT_PUSHPULL);

			LL_GPIO_SetPinPull(GPIOC,LL_GPIO_PIN_7,pull);
			LL_GPIO_SetAFPin_0_7(GPIOC,LL_GPIO_PIN_7,LL_GPIO_AF_2);
		}
	}
}

void timer_stm32_start(const struct device* dev)
{
	struct stm32_timer_driver_data* data =
							(struct stm32_timer_driver_data*)dev->data;

	TIM_TypeDef* tim_dev = data->timer_struct;

	if ( (tim_dev == TIM6) || (tim_dev == TIM7) )
	{
		if (data->timer_mode == periodic_interrupt)
		{
			LL_TIM_SetAutoReload(tim_dev,
								 (data->timer_irq_period_usec*10) - 1);
			LL_TIM_EnableIT_UPDATE(tim_dev);
			LL_TIM_EnableCounter(tim_dev);
		}
	}
	else if (tim_dev == TIM4 || tim_dev == TIM3)
	{
		if (data->timer_mode == incremental_coder)
		{
			LL_TIM_EnableCounter(tim_dev);
		}
	}
}

void timer_stm32_stop(const struct device* dev)
{
	struct stm32_timer_driver_data* data =
								(struct stm32_timer_driver_data*)dev->data;

	TIM_TypeDef* tim_dev = data->timer_struct;

	if ( (tim_dev == TIM6) || (tim_dev == TIM7) )
	{
		if (data->timer_mode == periodic_interrupt)
		{
			LL_TIM_DisableCounter(tim_dev);
			LL_TIM_DisableIT_UPDATE(tim_dev);
		}
	}
	else if (tim_dev == TIM4 || tim_dev == TIM3)
	{
		if (data->timer_mode == incremental_coder)
		{
			LL_TIM_DisableCounter(tim_dev);
		}
	}
}

void timer_stm32_clear(const struct device* dev)
{
	TIM_TypeDef* tim_dev =
				((struct stm32_timer_driver_data*)dev->data)->timer_struct;

	if (tim_dev != NULL)
	{
		LL_TIM_ClearFlag_UPDATE(tim_dev);
	}
}

uint32_t timer_stm32_get_count(const struct device* dev)
{
	TIM_TypeDef* tim_dev =
				((struct stm32_timer_driver_data*)dev->data)->timer_struct;

	return LL_TIM_GetCounter(tim_dev);
}

/* Per-timer inits */

 void init_timer_3()
 {
	 /* Configure Timer in incremental coder mode */
 
	 /* Peripheral clock enable */
	 LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
 
	 LL_TIM_InitTypeDef TIM_InitStruct = {0};
	 TIM_InitStruct.Prescaler = 0;
	 TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	 TIM_InitStruct.Autoreload = 65535;
	 TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
 
	 LL_TIM_Init(TIM3, &TIM_InitStruct);
	 LL_TIM_EnableARRPreload(TIM3);
	 LL_TIM_SetEncoderMode(TIM3, LL_TIM_ENCODERMODE_X4_TI12);
	 LL_TIM_IC_SetActiveInput(TIM3,
							  LL_TIM_CHANNEL_CH1,
							  LL_TIM_ACTIVEINPUT_DIRECTTI);
 
	 LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
	 LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV16_N5);
	 LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
	 LL_TIM_IC_SetActiveInput(TIM3,
							  LL_TIM_CHANNEL_CH2,
							  LL_TIM_ACTIVEINPUT_DIRECTTI);
 
	 LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
	 LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
	 LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
	 LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
	 LL_TIM_DisableMasterSlaveMode(TIM3);
	 LL_TIM_ConfigETR(TIM3,
					  LL_TIM_ETR_POLARITY_NONINVERTED,
					  LL_TIM_ETR_PRESCALER_DIV1,
					  LL_TIM_ETR_FILTER_FDIV1);
 
	 LL_TIM_ConfigIDX(
		 TIM3,
		 LL_TIM_INDEX_ALL|LL_TIM_INDEX_POSITION_DOWN_DOWN|LL_TIM_INDEX_UP_DOWN
	 );
 
	 LL_TIM_EnableEncoderIndex(TIM3);
 }
 

void init_timer_4()
{
	/* Configure Timer in incremental coder mode */

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);

	LL_TIM_InitTypeDef TIM_InitStruct = {0};
	TIM_InitStruct.Prescaler = 0;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 65535;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;

	LL_TIM_Init(TIM4, &TIM_InitStruct);
	LL_TIM_EnableARRPreload(TIM4);
	LL_TIM_SetEncoderMode(TIM4, LL_TIM_ENCODERMODE_X4_TI12);
	LL_TIM_IC_SetActiveInput(TIM4,
							 LL_TIM_CHANNEL_CH1,
							 LL_TIM_ACTIVEINPUT_DIRECTTI);

	LL_TIM_IC_SetPrescaler(TIM4, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
	LL_TIM_IC_SetFilter(TIM4, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV16_N5);
	LL_TIM_IC_SetPolarity(TIM4, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
	LL_TIM_IC_SetActiveInput(TIM4,
							 LL_TIM_CHANNEL_CH2,
							 LL_TIM_ACTIVEINPUT_DIRECTTI);

	LL_TIM_IC_SetPrescaler(TIM4, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
	LL_TIM_IC_SetFilter(TIM4, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
	LL_TIM_IC_SetPolarity(TIM4, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
	LL_TIM_SetTriggerOutput(TIM4, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM4);
	LL_TIM_ConfigETR(TIM4,
					 LL_TIM_ETR_POLARITY_NONINVERTED,
					 LL_TIM_ETR_PRESCALER_DIV1,
					 LL_TIM_ETR_FILTER_FDIV1);

	LL_TIM_ConfigIDX(
		TIM4,
		LL_TIM_INDEX_ALL|LL_TIM_INDEX_POSITION_DOWN_DOWN|LL_TIM_INDEX_UP_DOWN
	);

	LL_TIM_EnableEncoderIndex(TIM4);
}

void init_timer_6()
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);

	/* TIM6 interrupt Init */

	/* Set prescaler to tick to 0.1µs */
	TIM_InitStruct.Prescaler = (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC/1e7) - 1;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	LL_TIM_Init(TIM6, &TIM_InitStruct);
	LL_TIM_DisableARRPreload(TIM6);
	LL_TIM_SetTriggerOutput(TIM6, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM6);
}

void init_timer_7()
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);

	/* TIM7 interrupt Init */

	/* Set prescaler to tick to 0.1µs */
	TIM_InitStruct.Prescaler = (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC/1e7) - 1;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	LL_TIM_Init(TIM7, &TIM_InitStruct);
	LL_TIM_DisableARRPreload(TIM7);
	LL_TIM_SetTriggerOutput(TIM7, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM7);
}

/* Device definitions */

/* Timer4 */
#if DT_NODE_HAS_STATUS(TIMER3_NODE, okay)

struct stm32_timer_driver_data timer3_data =
{
	.timer_struct       = TIM3,
	.interrupt_line     = TIMER3_INTERRUPT_LINE,
	.interrupt_prio     = TIMER3_INTERRUPT_PRIO,
	.timer_irq_callback = NULL
};

DEVICE_DT_DEFINE(TIMER3_NODE,
                 timer_stm32_init,
                 NULL,
                 &timer3_data,
                 NULL,
                 PRE_KERNEL_1,
                 CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                 &timer_funcs
                );

#endif /* Timer 3 */


/* Timer4 */
#if DT_NODE_HAS_STATUS(TIMER4_NODE, okay)

/** @brief Structure that holds the timer 4 data  */
struct stm32_timer_driver_data timer4_data =
{
	.timer_struct       = TIM4,
	.interrupt_line     = TIMER4_INTERRUPT_LINE,
	.interrupt_prio     = TIMER4_INTERRUPT_PRIO,
	.timer_irq_callback = NULL
};

/**
 * @brief Define and initialize the STM32 timer device for TIM4.
 *
 * This macro registers the TIM4 timer as a Zephyr device using the device
 * tree node specified by `TIMER4_NODE`.
 *
 * - Registers the device with the STM32 timer driver init function.
 *
 * - Associates static device data with the driver.
 *
 * - Uses `PRE_KERNEL_1` initialization level with a configurable priority.
 *
 * - Provides the function pointer table `timer_funcs` for driver API access.
 *
 */
DEVICE_DT_DEFINE(TIMER4_NODE,
                 timer_stm32_init,
                 NULL,
                 &timer4_data,
                 NULL,
                 PRE_KERNEL_1,
                 CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                 &timer_funcs
                );

#endif /* Timer 4 */

/* Timer 6 */
#if DT_NODE_HAS_STATUS(TIMER6_NODE, okay)

/** @brief Structure that holds the timer 7 data  */
struct stm32_timer_driver_data timer6_data =
{
	.timer_struct       = TIM6,
	.interrupt_line     = TIMER6_INTERRUPT_LINE,
	.interrupt_prio     = TIMER6_INTERRUPT_PRIO,
	.timer_irq_callback = NULL
};


/**
 * @brief Define and initialize the STM32 timer device for TIM6.
 *
 * This macro registers the TIM6 timer as a Zephyr device using the device
 * tree node specified by `TIMER6_NODE`.
 *
 * - Registers the device with the STM32 timer driver init function.
 *
 * - Associates static device data with the driver.
 *
 * - Uses `PRE_KERNEL_1` initialization level with a configurable priority.
 *
 * - Provides the function pointer table `timer_funcs` for driver API access.
 *
 */
DEVICE_DT_DEFINE(TIMER6_NODE,
                 timer_stm32_init,
                 NULL,
                 &timer6_data,
                 NULL,
                 PRE_KERNEL_1,
                 CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                 &timer_funcs
                );

#endif /* Timer 6 */

/* Timer 7 */
#if DT_NODE_HAS_STATUS(TIMER7_NODE, okay)

/** @brief Structure that holds the timer 7 data  */
struct stm32_timer_driver_data timer7_data =
{
	.timer_struct       = TIM7,
	.interrupt_line     = TIMER7_INTERRUPT_LINE,
	.interrupt_prio     = TIMER7_INTERRUPT_PRIO,
	.timer_irq_callback = NULL
};

/**
 * @brief Define and initialize the STM32 timer device for TIM7.
 *
 * This macro registers the TIM7 timer as a Zephyr device using the device
 * tree node specified by `TIMER7_NODE`.
 *
 * - Registers the device with the STM32 timer driver init function.
 *
 * - Associates static device data with the driver.
 *
 * - Uses `PRE_KERNEL_1` initialization level with a configurable priority.
 *
 * - Provides the function pointer table `timer_funcs` for driver API access.
 *
 */
DEVICE_DT_DEFINE(TIMER7_NODE,
                 timer_stm32_init,
                 NULL,
                 &timer7_data,
                 NULL,
                 PRE_KERNEL_1,
                 CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                 &timer_funcs
                );

#endif /* Timer 7 */
