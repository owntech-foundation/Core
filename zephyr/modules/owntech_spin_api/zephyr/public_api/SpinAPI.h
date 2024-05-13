/*
 * Copyright (c) 2022-2023 LAAS-CNRS
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
 * @date   2023
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


#ifndef SPINAPI_H_
#define SPINAPI_H_

// Stdlib
#include <stdint.h>

// ARM lib
#include <arm_math.h>

// Private sources include
#include "../src/LedHAL.h"
#include "../src/DacHAL.h"
#include "../src/CompHAL.h"
#include "../src/PwmHAL.h"
#include "../src/AdcHAL.h"
#include "../src/UartHAL.h"
#include "../src/TimerHAL.h"
#include "../src/VersionHAL.h"
#include "../src/GpioHAL.h"

#ifdef CONFIG_OWNTECH_DATA_API
#include "../src/DataAPI.h"
#endif

#ifdef CONFIG_SHIELD_TWIST
#include "../src/NgndHAL.h"
#endif



/**
 * @brief Contains all the elements linked to peripherals of the spin board.
 */
class SpinAPI
{

public:

	/**
	 * @brief Contains all the functions for the spin gpio.
	 */
	GpioHAL gpio;

	/**
	 * @brief Contains all the function of the embedded LED.
	 */
	LedHAL led;

	/**
	 * @brief Contains all the function of the STM32 DAC used to generate signals and handle the current mode.
	 */
	DacHAL dac;

	/**
	 * @brief Contains all the function of the STM32 comparator used with the current mode.
	 */
	CompHAL comp;

	/**
	 * @brief Contains all the function of the STM32 hrtim PWM generator.
	 */
	PwmHAL pwm;

	/**
	 * @brief Contains all the function of the STM32 ADC including configuration and synchronization with the HRTIM.
	 */
	AdcHAL adc;

	/**
	 * @brief Contains all the function of the STM32 Usart1 functions.
	 */
	UartHAL uart;

	/**
	 * @brief Contains all the function of the STM32 Timer4 functions that handle the encoder.
	 */
	TimerHAL timer;

	/**
	 * @brief Contains all the function related to the versioning of the microcontroller boards.
	 */
	VersionHAL version;

#ifdef CONFIG_OWNTECH_DATA_API
	/**
	 * @brief Data acquisition from Spin ADCs.
	 */
	DataAPI data;
#endif

#ifdef CONFIG_SHIELD_TWIST
	/**
	 * @brief Contains all the function of the NGND switch compatible with TWISTs prior to 1.4.
	 */
	NgndHAL ngnd;
#endif

};


/////
// Public object to interact with the class

extern SpinAPI spin;


#endif // SPINAPI_H_
