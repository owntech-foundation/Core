/*
 * Copyright (c) 2024 LAAS-CNRS
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
 * @date 2024
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 *
 */

#ifndef SAFETY_SETTING_H_
#define SAFETY_SETTING_H_

#include "ShieldAPI.h"
#include "safety_enum.h"

/**
 * @brief Enables the monitoring of the selected channels for safety.
 *
 * @param safety_channels A list of the channels to watch. The variables in the list can be :
 *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
 * @param channels_number The number of channels present in the list safety_channels.
 *
 * @return 0 if sucessfull, or -1 if there was an error
*/
int8_t safety_set_channel_watch(channel_t * safety_channels, uint8_t channels_number);


/**
 * @brief Disables the monitoring of the selected channels for safety.
 *
 * @param safety_channels A list of the channels to unwatch. The variables in the list can be :
 *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
 * @param channels_number The number of channels present in the list safety_channels.
 *
 * @return 0 if sucessfull, or -1 if there was an error
*/
int8_t safety_unset_channel_watch(channel_t * safety_channels, uint8_t channels_number);

/**
 * @brief Checks if a channel is being monitored or not.
 *
 * @param safety_channels the channel to check
 *        @arg V1_LOW
 *        @arg V2_LOW
 *        @arg V_HIGH
 *        @arg I1_LOW
 *        @arg I2_LOW
 *        @arg I_HIGH
 *        @arg TEMP_SENSOR
 *        @arg EXTRA_MEAS
 *        @arg ANALOG_COMM
 *
 * @return true if the channel is being monitored, false if not
*/
bool safety_get_channel_watch(channel_t  safety_channels);

/**
 * @brief Sets the reaction to do if an error has been detected. Choose either open-circuit (both switches are opened
 *        and no power flows in the output) or short-circuit (high-side switch is opened, and low-side is closed maintaining
 *        the output in short-circuit)
 *
 * @param reaction the reaction to do
 *        @arg Open_Circuit
 *        @arg Short_Circuit
 *
 * @return none
*/
void safety_set_channel_reaction(safety_reaction_t reaction);

/**
 * @brief Returns the reaction to do when encoutering an error.
 *
 * @return Open_Circuit or Short_Circuit
*/
safety_reaction_t safety_get_channel_reaction();

/**
 * @brief Sets the maximum threshold for the channels present in the list safety_channels.
 *
 * @param safety_channels A list of the channels to set the threshold. The variables in the list can be :
 *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
 * @param threshold A list of the maximum threshold to apply to the channels in safety_channels.
 * @param channels_number the number of channels present in the list safety_channels
 *
 * @return 0 if sucessfull, or -1 if not.
*/
int8_t safety_set_channel_threshold_max(channel_t *safety_channels, float32_t *threshold, uint8_t channels_number);


/**
 * @brief Sets the minimum threshold for the channels present in the list safety_channels.
 *
 * @param safety_channels A list of the channels to set the threshold. The variables in the list can be :
 *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
 * @param threshold A list of the minimum threshold to apply to the channels in safety_channels.
 * @param channels_number the number of channels present in the list safety_channels
 *
 * @return 0 if sucessfull, or -1 if not.
*/
int8_t safety_set_channel_threshold_min(channel_t *safety_channels, float32_t *threshold, uint8_t channels_number);

/**
 * @brief Gets the maximum threshold of the selected channel
 *
 * @param safety_channels the channel to check
 *        @arg V1_LOW
 *        @arg V2_LOW
 *        @arg V_HIGH
 *        @arg I1_LOW
 *        @arg I2_LOW
 *        @arg I_HIGH
 *        @arg TEMP_SENSOR
 *        @arg EXTRA_MEAS
 *        @arg ANALOG_COMM
 *
 * @return The maximum threshold
*/
float32_t safety_get_channel_threshold_max(channel_t safety_channel);

/**
 * @brief Gets the minimum threshold of the selected channel
 *
 * @param safety_channels the channel to check
 *        @arg V1_LOW
 *        @arg V2_LOW
 *        @arg V_HIGH
 *        @arg I1_LOW
 *        @arg I2_LOW
 *        @arg I_HIGH
 *        @arg TEMP_SENSOR
 *        @arg EXTRA_MEAS
 *        @arg ANALOG_COMM
 *
 * @return the minimum threshold
*/
float32_t safety_get_channel_threshold_min(channel_t safety_channel);

/**
 * @brief Checks if the channel faced an error (went over/under threshold)
 *
 * @param safety_channels the channel to check
 *        @arg V1_LOW
 *        @arg V2_LOW
 *        @arg V_HIGH
 *        @arg I1_LOW
 *        @arg I2_LOW
 *        @arg I_HIGH
 *        @arg TEMP_SENSOR
 *        @arg EXTRA_MEAS
 *        @arg ANALOG_COMM
 *
 * @return true if the channel faced an error, false if not
*/
bool safety_get_channel_error(channel_t safety_channel);

/**
 * @brief Monitors all the channel set as watchable and compare them with the chosen thresholds.
 *
 * @return 0 if all the channels are within their trehold, or -1 if one of them went under/over the threshold.
*/
int8_t safety_watch();

/**
 * @brief Enables the open-circuit or the short-circuit mode if an error has been detected.
 *
 * @return none
*/
void safety_action();

/**
 * @brief Enable the safet API fault detection task
 *
 * @return none
*/
void safety_enable_task();

/**
 * @brief Disables the safet API fault detection task
 *
 * @return none
*/
void safety_disable_task();

/**
 * @brief Stores the current minimum and maximum threshold in the flash (non volatile memory)
 *
 * @param safety_channels the channel for which we store the threshold in the NVS
 *        @arg V1_LOW
 *        @arg V2_LOW
 *        @arg V_HIGH
 *        @arg I1_LOW
 *        @arg I2_LOW
 *        @arg I_HIGH
 *        @arg TEMP_SENSOR
 *        @arg EXTRA_MEAS
 *        @arg ANALOG_COMM
 *
 * @return 0 if parameters were correcly stored, -1 if there was an error.
 *
*/
int8_t safety_store_threshold_in_nvs(channel_t channel);

/**
 * @brief Stores the current minimum and maximum threshold in the flash (non volatile memory)
 *
 * @param safety_channels the channel for which we store the threshold in the NVS
 *        @arg V1_LOW
 *        @arg V2_LOW
 *        @arg V_HIGH
 *        @arg I1_LOW
 *        @arg I2_LOW
 *        @arg I_HIGH
 *        @arg TEMP_SENSOR
 *        @arg EXTRA_MEAS
 *        @arg ANALOG_COMM
 *
 * @return  0: if parameters were correcly retreived, negative value if there was an error:
 *         -1: NVS is empty
 *         -2: NVS contains data, but their version doesn't match current version
 *         -3: NVS data is corrupted
 *         -4: NVS contains data, but not for the requested channel
 *
*/
int8_t  safety_retrieve_threshold_in_nvs(channel_t channel);


#endif // SAFETY_SETTING_H_
