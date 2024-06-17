/*
 * Copyright (c) 2021-2023 LAAS-CNRS
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
 *
 * @brief This file implements the channel management of the ADCs.
 */

#ifndef SHIELD_CHANNELS_H_
#define SHIELD_CHANNELS_H_


#include <stdint.h>

#include "ShieldAPI.h"


/////
// Public definitions

typedef struct channel_info_t
{
	channel_info_t(uint8_t adc_num, uint8_t channel_num, uint8_t pin_num)
	{
		this->adc_num     = adc_num;
		this->channel_num = channel_num;
		this->pin_num     = pin_num;
	}

	uint8_t adc_num;
	uint8_t channel_num;
	uint8_t pin_num;
} channel_info_t;


/**
 *
 * @brief  This function is used to enable a channel on the shield.
 *
 * @param  adc_number Number of the ADC on which channel configuration is
 *         to be done.
 * @param  channel_name Name of the channel to enable as defined in the shield
 *         device tree.
 */
void shield_channels_enable_adc_channel(uint8_t adc_num, channel_t channel_name);

/**
 * @brief  This function returns a structure containing information abouut
 *         a channel from a channel name.
 *
 *         This function can only be called after the channel has been
 *         enabled on the ADC using adc_channels_enable_adc_channel().
 *
 * @param  channel Name of the channel as defined in the device tree.
 * @return Structure containing the ADC number, channel number and pin
 *         number for the given channel name, or (0, 0, 0) if channel
 *         name doesn't exist or hasn't been configured.
 *
 */
channel_info_t shield_channels_get_enabled_channel_info(channel_t channel_name);

/**
 * @brief    Retrieve calibration coefficients via console input and update them
 */
void shield_channels_set_user_acquisition_parameters();


#endif // SHIELD_CHANNELS_H_
