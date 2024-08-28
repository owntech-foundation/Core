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
 * @brief Enables the monitoring of the selected sensors for safety.
 *
 * @param safety_sensors A list of the sensors to watch. The variables in the list can be :
 *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
 * @param sensors_number The number of sensors present in the list safety_sensors.
 *
 * @return 0 if sucessfull, or -1 if there was an error
*/
int8_t safety_set_sensor_watch(sensor_t * safety_sensors, uint8_t sensors_number);


/**
 * @brief Disables the monitoring of the selected sensors for safety.
 *
 * @param safety_sensors A list of the sensors to unwatch. The variables in the list can be :
 *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
 * @param sensors_number The number of sensors present in the list safety_sensors.
 *
 * @return 0 if sucessfull, or -1 if there was an error
*/
int8_t safety_unset_sensor_watch(sensor_t * safety_sensors, uint8_t sensors_number);

/**
 * @brief Checks if a sensor is being monitored or not.
 *
 * @param safety_sensors the sensor to check
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
bool safety_get_sensor_watch(sensor_t  safety_sensors);

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
void safety_set_sensor_reaction(safety_reaction_t reaction);

/**
 * @brief Returns the reaction to do when encoutering an error.
 *
 * @return Open_Circuit or Short_Circuit
*/
safety_reaction_t safety_get_sensor_reaction();

/**
 * @brief Sets the maximum threshold for the sensors present in the list safety_sensors.
 *
 * @param safety_sensors A list of the sensors to set the threshold. The variables in the list can be :
 *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
 * @param threshold A list of the maximum threshold to apply to the sensors in safety_sensors.
 * @param sensors_number the number of sensors present in the list safety_sensors
 *
 * @return 0 if sucessfull, or -1 if not.
*/
int8_t safety_set_sensor_threshold_max(sensor_t *safety_sensors, float32_t *threshold, uint8_t sensors_number);


/**
 * @brief Sets the minimum threshold for the sensors present in the list safety_sensors.
 *
 * @param safety_sensors A list of the sensors to set the threshold. The variables in the list can be :
 *                        V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
 * @param threshold A list of the minimum threshold to apply to the sensors in safety_sensors.
 * @param sensors_number the number of sensors present in the list safety_sensors
 *
 * @return 0 if sucessfull, or -1 if not.
*/
int8_t safety_set_sensor_threshold_min(sensor_t *safety_sensors, float32_t *threshold, uint8_t sensors_number);

/**
 * @brief Gets the maximum threshold of the selected sensor
 *
 * @param safety_sensors the sensor to check
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
float32_t safety_get_sensor_threshold_max(sensor_t safety_sensor);

/**
 * @brief Gets the minimum threshold of the selected sensor
 *
 * @param safety_sensors the sensor to check
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
float32_t safety_get_sensor_threshold_min(sensor_t safety_sensor);

/**
 * @brief Checks if the sensor faced an error (went over/under threshold)
 *
 * @param safety_sensors the sensor to check
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
bool safety_get_sensor_error(sensor_t safety_sensor);

/**
 * @brief Monitors all the sensor set as watchable and compare them with the chosen thresholds.
 *
 * @return 0 if all the sensors are within their trehold, or -1 if one of them went under/over the threshold.
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
 * @param safety_sensors the sensor for which we store the threshold in the NVS
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
int8_t safety_store_threshold_in_nvs(sensor_t sensor);

/**
 * @brief Stores the current minimum and maximum threshold in the flash (non volatile memory)
 *
 * @param safety_sensors the sensor for which we store the threshold in the NVS
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
 *         -4: NVS contains data, but not for the requested sensor
 *
*/
int8_t  safety_retrieve_threshold_in_nvs(sensor_t sensor);


#endif // SAFETY_SETTING_H_
