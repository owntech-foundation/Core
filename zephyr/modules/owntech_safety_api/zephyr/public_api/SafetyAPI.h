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
     *        All the sensors will be monitored to detect potential faults in voltage/current.
     *
     * @return none
    */
    void init_shield();

    /**
     * @brief Initializes thresholds min/max with the default value from the device tree,
     *        if values were stored and found in the flash they will be used instead.
     *        All the sensors will be monitored to detect potential faults in voltage/current.
     *
     * @param sensors_watch A list of the sensors to watch. The variables in the list can be :
     *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
     * @param sensors_watch_number The number of sensors present in the list sensors_watch.
     *
     * @return none
    */
    void init_shield(sensor_t* sensors_watch, uint8_t sensors_watch_number);

    /**
     * @brief Enables the monitoring of the selected sensors for safety.
     *
     * @param sensors_watch A list of the sensors to watch. The variables in the list can be :
     *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
     * @param sensors_watch_number The number of sensors present in the list sensors_watch.
     *
     * @return 0 if sucessfull, or -1 if there was an error
    */
    int8_t setChannelWatch(sensor_t* sensors_watch, uint8_t sensors_watch_number);

    /**
     * @brief Disables the monitoring of the selected sensors for safety.
     *
     * @param sensors_watch A list of the sensors to unwatch. The variables in the list can be :
     *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
     * @param sensors_watch_number The number of sensors present in the list sensors_watch.
     *
     * @return 0 if sucessfull, or -1 if there was an error
    */
    int8_t unsetChannelWatch(sensor_t* sensors_watch, uint8_t sensors_watch_number);

    /**
     * @brief check if a sensor is being monitored or not.
     *
     * @param sensors_watch the sensor to check
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
     * @return true if the sensor is being monitored, false if not
    */
    bool getChannelWatch(sensor_t  sensors_watch);

    /**
     * @brief Set the reaction to do if an error has been detected. Choose either open-circuit (both switches are opened
     *        and no power flows in the output) or short-circuit (high-side switch is opened, and low-side is closed maintaining
     *        the output in short-circuit)
     *
     * @param sensors_reaction the reaction to do
     *        @arg Open_Circuit
     *        @arg Short_Circuit
     *
     * @return none
    */
    void setChannelReaction(safety_reaction_t sensors_reaction);

    /**
     * @brief returns the reaction to do when encoutering an error.
     *
     * @return Open_Circuit or Short_Circuit
    */
    safety_reaction_t getChannelReaction();

    /**
     * @brief set the maximum threshold for the sensors present in the list safety_sensors.
     *
     * @param sensors_threshold A list of the sensors to set the threshold. The variables in the list can be :
     *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
     * @param threshold_max A list of the maximum threshold to apply to the sensors in safety_sensors.
     * @param sensors_threshold_number the number of sensors present in the list sensor_threshold
     *
     * @return 0 if sucessfull, or -1 if not.
    */
    int8_t setChannelThresholdMax(sensor_t *sensors_threshold, float32_t *threshold_max, uint8_t sensors_threshold_number);

    /**
     * @brief set the minimum threshold for the sensors present in the list safety_sensors.
     *
     * @param sensors_threshold A list of the sensors to set the threshold. The variables in the list can be :
     *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
     * @param threshold_min A list of the minimum threshold to apply to the sensors in safety_sensors.
     * @param sensors_threshold_number the number of sensors present in the list sensor_threshold
     *
     * @return 0 if sucessfull, or -1 if not.
    */
    int8_t setChannelThresholdMin(sensor_t *sensors_threshold, float32_t *threshold_min, uint8_t sensors_threshold_number);

    /**
     * @brief get the maximum threshold of the selected sensor
     *
     * @param sensors_threshold the sensor to check
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
    float32_t getChannelThresholdMax(sensor_t sensors_threshold);

    /**
     * @brief get the minimum threshold of the selected sensor
     *
     * @param sensors_threshold the sensor to check
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
    float32_t getChannelThresholdMin(sensor_t sensors_threshold);

    /**
     * @brief check if the sensor faced an error (went over/under threshold)
     *
     * @param sensors_error the sensor to check
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
     * @return true if the sensor faced an error, false if not
    */
    bool getChannelError(sensor_t sensors_error);


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
     * @param sensor_threshold_store the sensor for which we store the threshold in the NVS
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
    int8_t storeThreshold(sensor_t sensor_threshold_store);

    /**
     * @brief store the current minimum and maximum threshold in the flash (non volatile memory)
     *
     * @param sensor_threshold_retrieve the sensor for which we store the threshold in the NVS
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
     *         -4: NVS contains data, but not for the requested sensor
     *
    */
    int8_t retrieveThreshold(sensor_t sensor_threshold_retrieve);
};

extern safety Safety;

#endif // SAFETY_H_
