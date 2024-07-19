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
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */



#ifndef NGNDHAL_H_
#define NGNDHAL_H_


class NgndHAL
{
public:

	/**
	 * @brief Turns the NGND switch ON.
	 *        It is used with versions of the TWIST board prior to 1.4.
	 */
	void turnOn();

	/**
	 * @brief Turns the NGND switch OFF.
	 *        It is used with versions of the TWIST board prior to 1.4.
	 */
	void turnOff();

private:


};



#endif // LED_H_
