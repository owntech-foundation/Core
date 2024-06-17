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
 * @brief Public class to use safety function
 *
 */

#ifndef SAFETY_H_
#define SAFETY_H_

#include "arm_math.h"
#include "ShieldAPI.h"
#include "../src/safety_enum.h"


class safety{

    public:

    /**
     * @brief Initializes thresholds min/max with the default value from the device tree,
     *        if values were stored and found in the flash they will be used instead.
     *        All the channels will be monitored to detect potential faults in voltage/current.
     *
     * @return none
    */
    void init_shield();

    /**
     * @brief Initializes thresholds min/max with the default value from the device tree,
     *        if values were stored and found in the flash they will be used instead.
     *        All the channels will be monitored to detect potential faults in voltage/current.
     *
     * @param channels_watch A list of the channels to watch. The variables in the list can be :
     *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
     * @param channels_watch_number The number of channels present in the list channels_watch.
     *
     * @return none
    */
    void init_shield(channel_t* channels_watch, uint8_t channels_watch_number);

    /**
     * @brief Enables the monitoring of the selected channels for safety.
     *
     * @param channels_watch A list of the channels to watch. The variables in the list can be :
     *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
     * @param channels_watch_number The number of channels present in the list channels_watch.
     *
     * @return 0 if sucessfull, or -1 if there was an error
    */
    int8_t setChannelWatch(channel_t* channels_watch, uint8_t channels_watch_number);

    /**
     * @brief Disables the monitoring of the selected channels for safety.
     *
     * @param channels_watch A list of the channels to unwatch. The variables in the list can be :
     *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
     * @param channels_watch_number The number of channels present in the list channels_watch.
     *
     * @return 0 if sucessfull, or -1 if there was an error
    */
    int8_t unsetChannelWatch(channel_t* channels_watch, uint8_t channels_watch_number);

    /**
     * @brief check if a channel is being monitored or not.
     *
     * @param channels_watch the channel to check
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
    bool getChannelWatch(channel_t  channels_watch);

    /**
     * @brief Set the reaction to do if an error has been detected. Choose either open-circuit (both switches are opened
     *        and no power flows in the output) or short-circuit (high-side switch is opened, and low-side is closed maintaining
     *        the output in short-circuit)
     *
     * @param channels_reaction the reaction to do
     *        @arg Open_Circuit
     *        @arg Short_Circuit
     *
     * @return none
    */
    void setChannelReaction(safety_reaction_t channels_reaction);

    /**
     * @brief returns the reaction to do when encoutering an error.
     *
     * @return Open_Circuit or Short_Circuit
    */
    safety_reaction_t getChannelReaction();

    /**
     * @brief set the maximum threshold for the channels present in the list safety_channels.
     *
     * @param channels_threshold A list of the channels to set the threshold. The variables in the list can be :
     *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
     * @param threshold_max A list of the maximum threshold to apply to the channels in safety_channels.
     * @param channels_threshold_number the number of channels present in the list channel_threshold
     *
     * @return 0 if sucessfull, or -1 if not.
    */
    int8_t setChannelThresholdMax(channel_t *channels_threshold, float32_t *threshold_max, uint8_t channels_threshold_number);

    /**
     * @brief set the minimum threshold for the channels present in the list safety_channels.
     *
     * @param channels_threshold A list of the channels to set the threshold. The variables in the list can be :
     *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
     * @param threshold_min A list of the minimum threshold to apply to the channels in safety_channels.
     * @param channels_threshold_number the number of channels present in the list channel_threshold
     *
     * @return 0 if sucessfull, or -1 if not.
    */
    int8_t setChannelThresholdMin(channel_t *channels_threshold, float32_t *threshold_min, uint8_t channels_threshold_number);

    /**
     * @brief get the maximum threshold of the selected channel
     *
     * @param channels_threshold the channel to check
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
     * @return the maximum threshold
    */
    float32_t getChannelThresholdMax(channel_t channels_threshold);

    /**
     * @brief get the minimum threshold of the selected channel
     *
     * @param channels_threshold the channel to check
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
    float32_t getChannelThresholdMin(channel_t channels_threshold);

    /**
     * @brief check if the channel faced an error (went over/under threshold)
     *
     * @param channels_error the channel to check
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
    bool getChannelError(channel_t channels_error);


    /**
     * @brief enable the safet API fault detection task
     *
     * @return none
    */
    void enableSafetyApi();

    /**
     * @brief disable the safet API fault detection task
     *
     * @return none
    */
    void disableSafetyApi();

    /**
     * @brief store the current minimum and maximum threshold in the flash (non volatile memory)
     *
     * @param channel_threshold_store the channel for which we store the threshold in the NVS
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
    int8_t storeThreshold(channel_t channel_threshold_store);

    /**
     * @brief store the current minimum and maximum threshold in the flash (non volatile memory)
     *
     * @param channel_threshold_retrieve the channel for which we store the threshold in the NVS
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
     * @return 0 if parameters were correcly retreived, negative value if there was an error:
     *         -1: NVS is empty
     *         -2: NVS contains data, but their version doesn't match current version
     *         -3: NVS data is corrupted
     *         -4: NVS contains data, but not for the requested channel
     *
    */
    int8_t retrieveThreshold(channel_t channel_threshold_retrieve);
};

extern safety Safety;

#endif // SAFETY_H_
