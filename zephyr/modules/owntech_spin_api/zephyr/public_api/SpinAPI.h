/*
 * Copyright (c) 2022-present LAAS-CNRS
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

/**
 * @date   2024
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


#ifndef SPINAPI_H_
#define SPINAPI_H_

#include "../src/CompHAL.h"
#include "../src/DacHAL.h"
#include "../src/DataAPI.h"
#include "../src/GpioHAL.h"
#include "../src/LedHAL.h"
#include "../src/PwmHAL.h"
#include "../src/TimerHAL.h"

#ifdef CONFIG_OWNTECH_UART_API
#include "../src/UartHAL.h"
#endif

#ifdef CONFIG_OWNTECH_NGND_DRIVER
#include "../src/NgndHAL.h"
#endif



/**
 * @brief Contains all the elements linked to peripherals of the spin board.
 */
class SpinAPI
{
public:

#ifdef CONFIG_OWNTECH_GPIO_API
	/**
	 * @brief Contains all the functions for the spin gpio.
	 */
	static GpioHAL gpio;
#endif
	/**
	 * @brief Contains all the function of the embedded LED.
	 */
	static LedHAL led;

	/**
	 * @brief Contains all the function of the STM32 DAC used to generate signals and handle the current mode.
	 */
	static DacHAL dac;

	/**
	 * @brief Contains all the function of the STM32 comparator used with the current mode.
	 */
	static CompHAL comp;

	/**
	 * @brief Contains all the function of the STM32 hrtim PWM generator.
	 */
	static PwmHAL pwm;

#ifdef CONFIG_OWNTECH_UART_API
	/**
	 * @brief Contains all the function of the STM32 Usart1 functions.
	 */
	static UartHAL uart;
#endif

	/**
	 * @brief Contains all the function of the STM32 Timer4 functions that handle the encoder.
	 */
	static TimerHAL timer;

	/**
	 * @brief Data acquisition from SPIN ADCs
	 */
	static DataAPI data;

};


/**
 *  Public object to interact with the class
 */

extern SpinAPI spin;


#endif /* SPINAPI_H_ */
