/*
 * Copyright (c) 2021-2022 LAAS-CNRS
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
 * @date   2022
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 *
 * @brief This file implements the channel management of the ADCs.
 *
 * THIS FILE SHOULD NOT BE INCLUDED ANYWHERE OUTSIDE FILES
 * FROM THE SAME FOLDER.
 */

#ifndef ADC_CHANNELS_H_
#define ADC_CHANNELS_H_


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Performs internal data structures initialization
 * and pre-ADC enable init.
 * Must be called before adc_core_enable()
 */
void adc_channels_init();

/**
 * ADC channel configuration.
 * For each channel enabled by the user, sets its sequencer rank and
 * sampling time, then sets the ADC sequencer length.
 *
 * If must be called only after adc_channnels_configure_adc_channels
 * has been called for this ADC, and ADC is enabled an not running.
 *
 * @param adc_num Number of the adc for which to configure channels.
 */
void adc_channels_configure(uint8_t adc_num);

/**
 * This function is used to configure the channels to be
 * enabled on a given ADC.
 *
 * @param  adc_number Number of the ADC on which channel configuration is
 *         to be done.
 * @param  channel_list List of channels to configure. This is a list of
 *         names as defined in the device tree (field `label`). The order
 *         of the names in the array sets the acquisition ranks (order in
 *         which the channels are acquired).
 * @param  channel_count Number of channels defined in `channel_list`.
 * @return 0 is everything went well,
 *         ECHANNOTFOUND if at least one of the channels
 *           is not available in the given ADC. Available channels are the
 *           ones defined in the device tree.
 */
int8_t adc_channnels_configure_adc_channels(uint8_t adc_num, const char* channel_list[], uint8_t channel_count);

/**
 * This function returns the name of an enabled channel.
 *
 * This function must onle be called after
 * adc_channnels_configure_adc_channels has been called.
 *
 * @param  adc_number Number of the ADC
 * @param  channel_rank Rank of the ADC channel to query.
 *         Rank ranges from 0 to (number of enabled channels)-1
 * @return Name of the channel as defined in the device tree, or
 *         NULL if channel configuration has not been made or
 *         channel_rank is over (number of enabled channels)-1.
 */
const char* adc_channels_get_channel_name(uint8_t adc_num, uint8_t channel_rank);

/**
 * Get the number of enabled channels for an ADC.
 * @param  adc_num Number of the ADC
 * @return Number of enabled channels in this ADC.
 */
uint8_t adc_channels_get_enabled_channels_count(uint8_t adc_num);


#ifdef __cplusplus
}
#endif

#endif // ADC_CHANNELS_H_
