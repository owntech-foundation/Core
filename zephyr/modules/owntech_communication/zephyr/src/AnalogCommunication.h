/*
 * Copyright (c) 2024-present LAAS-CNRS
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
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef ANALOGCOMMUNICATION_H_
#define ANALOGCOMMUNICATION_H_

#ifdef CONFIG_OWNTECH_COMMUNICATION_ENABLE_ANALOG

#include <stdint.h>
#include <arm_math.h>


/**
 * Static class definition
 */

class AnalogCommunication
{

public:
	/**
	 * @brief Initializing analog communication (ADC and DAC)
	 */
	static void init();

	/**
	 * @brief Get the analog value
	 *
	 * @return Analog value (A value between 0 and 4096)
	 */
	static float32_t getAnalogCommValue();

	/**
	 * @brief Set the analog value, the DAC output
	 *
	 * @param analog_bus_value  A value between 0 and 4096
	 */
	static void setAnalogCommValue(uint32_t analog_bus_value);
};

#endif /* CONFIG_OWNTECH_COMMUNICATION_ENABLE_ANALOG */

#endif /* ANALOGCOMMUNICATION_H_ */
