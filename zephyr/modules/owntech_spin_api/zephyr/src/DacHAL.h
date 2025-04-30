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



#ifndef DACHAL_H_
#define DACHAL_H_

/* Stdlib */
#include <stdint.h>

/* ARM lib */
#include <arm_math.h>

/* OwnTech Modules */
#include "hrtim_enum.h"


class DacHAL
{
public:
	/**
	 * @brief Initialize a DAC device in constant output mode.
	 *
	 * Configures the specified DAC (1, 2, or 3) to output a constant analog 
	 * value.
	 * 
	 * This includes setting the value to 0, enabling external output, 
	 * and starting the DAC.
	 *
	 * @param dac_number The DAC number (1, 2, or 3).
	 */
	void initConstValue(uint8_t dac_number);

	/**
	 * @brief Set a constant analog output value on a DAC channel.
	 *
	 * Writes a raw 12-bit value to the selected DAC channel, 
	 * provided the DAC is ready.
	 *
	 * @param dac_number   The DAC number (1, 2, or 3).
	 * @param channel      The DAC output channel index (usually 1).
	 * @param const_value  The constant value to set (0–4095).
	 */
	void setConstValue(uint8_t dac_number,
					   uint8_t channel,
					   uint32_t const_value);

	/**
	 * @brief Configure a DAC to operate in sawtooth waveform mode for current-mode control.
	 *
	 * Initializes the DAC to produce a decrementing sawtooth waveform.
	 * 
	 * The DAC is triggered by the selected HRTIM timer unit to synchronize 
	 * waveform generation with the PWM cycle. 
	 * 
	 * Reset and step triggers are selected based on the `tu_src`.
	 *
	 * @param dac_number The DAC number (1 or 3).
	 * @param tu_src     The HRTIM timer unit (TIMB to TIMF) used for synchronization.
	 */					   
	void slopeCompensation(uint8_t dac_number,
						   float32_t peak_voltage,
						   float32_t low_voltage);

	/**
	 * @brief Update slope compensation settings on a DAC using voltage values.
	 *
	 * Converts the given voltage values into digital DAC values and sets the 
	 * DAC's sawtooth waveform reset and step parameters accordingly.
	 *
	 * @param dac_number   The DAC number (1 or 3).
	 * @param peak_voltage The peak voltage value for the sawtooth ramp 
	 * 					   (reset level).
	 * @param low_voltage  The valley (starting) voltage of the ramp.
	 */
	void currentModeInit(uint8_t dac_number, hrtim_tu_t tu_src);
};


#endif /* DAC_HAL_H_ */
