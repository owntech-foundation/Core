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

#ifndef COMPHAL_H_
#define COMPHAL_H_

/**
 * @brief  Handles comparator 1 and 3 of the SPIN board
 *
 * @note   Use this element to call the initialization function of either comparator
 */
class CompHAL
{
public:
	/**
	 * @brief  Initializes a comparator for the current mode control.
	 *
	 * @note   Current mode control deploys a comparator together with a DAC.
     *         This function must be called after setting up the DAC linked to the comparator.
     *         Note that comparator 1 is linked with DAC3 and comparator 3 with DAC1.
     *         Comparator 1 is linked with ilow1 and comparator 3 is linked with ilow2.
	 *
	 * @param  comparator_number can be either 1 or 3
	 */
	void initialize(uint8_t comparator_number);
};



#endif // COMPHAL_H_
