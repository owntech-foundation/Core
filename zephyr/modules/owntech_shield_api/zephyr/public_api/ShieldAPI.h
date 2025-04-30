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

/*
 * @date   2024
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Jean Alinei <jean.alinei@owntech.org>
 */


#ifndef SHIELDAPI_H_
#define SHIELDAPI_H_


#include "../src/Sensors.h"
#include "../src/Power.h"


/* Static class definition */

class ShieldAPI
{
public:

	/**
	 * @brief Contains all the functions to interact with shield sensors
	 */
	static SensorsAPI sensors;

	/**
	 * @brief Contains all the functions to drive shield power capabilities
	 */
	static PowerAPI power;

#ifdef CONFIG_OWNTECH_NGND_DRIVER
	/**
	 * @brief Contains all the function of the NGND switch compatible
	 * 	      with TWISTs prior to 1.4.
	 */
	static NgndHAL ngnd;
#endif

};

/**
 *  Public object to interact with the class
 */

extern ShieldAPI shield;


#endif /* SHIELDAPI_H_ */
