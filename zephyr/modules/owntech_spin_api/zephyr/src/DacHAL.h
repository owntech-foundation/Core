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
 * SPDX-License-Identifier: LGLPV2.1
 */

/**
 * @date   2023
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */



#ifndef DACHAL_H_
#define DACHAL_H_

// Stdlib
#include <stdint.h>

// ARM lib
#include <arm_math.h>

//OwnTech Modules
#include "hrtim_enum.h"


class DacHAL
{
public:
	void initConstValue(uint8_t dac_number);
	void setConstValue(uint8_t dac_number, uint8_t channel, uint32_t const_value);
	void slopeCompensation(uint8_t dac_number, float32_t peak_voltage, float32_t low_voltage);
	void currentModeInit(uint8_t dac_number, hrtim_tu_t tu_src);
};



#endif // DAC_HAL_H_
