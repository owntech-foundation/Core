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

/*
 * @date   2023
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */

#ifndef TIMERHAL_H_
#define TIMERHAL_H_

/* Stdlib */
#include <stdint.h>

/* ARM lib */
#include <arm_math.h>

/* OwnTech API */
#include "timer.h"


typedef enum
{
     TIMER3,
     TIMER4
} timernumber_t;


/**
 * @brief  Handles timer 4 for the SPIN board
 *
 * @note   Use this element to initialize timer 4 and use it with
 * 		   an incremental encoder
 */
class TimerHAL
{
public:

	/**
	 * @brief Launches the timer4 which is adapted for reading an encoder.
	 */
	void startLogIncrementalEncoder(timernumber_t timer_number);

	/**
	 * @brief Gets the encoder step value.
	 * @return An uint32 value of the counter which corresponds to the
	 * 		   step of the system.
	 */
	uint32_t getIncrementalEncoderValue(timernumber_t timer_number);



private:

	/**
	 * @brief Initializes timer 4.
	 */
	void Initialize(timernumber_t timer_number);

	/* Variables */
	static bool timer4init;
	static bool timer4started;
	static bool timer3init;
	static bool timer3started;


};



#endif /* TIMERHAL_H_ */
