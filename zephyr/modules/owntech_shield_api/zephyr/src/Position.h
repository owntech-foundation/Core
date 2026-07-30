/*
 * Copyright (c) 2026-present LAAS-CNRS
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

#ifndef POSITION_H_
#define POSITION_H_

#include <stdbool.h>
#include <stdint.h>

#include <arm_math.h>
#include <zephyr/devicetree.h>

#include "Sensors.h"
#include "SpinAPI.h"

#define POSITION_SENSOR_TOKEN(node_id) \
	DT_STRING_TOKEN(node_id, position_sensor_name),

#define POSITION_SENSOR_FOREACH_OKAY(fn)        \
	DT_FOREACH_STATUS_OKAY(shield_position_hall, fn) \
	DT_FOREACH_STATUS_OKAY(shield_position_abz, fn)  \
	DT_FOREACH_STATUS_OKAY(shield_position_sincos, fn)

typedef enum
{
	UNDEFINED_POSITION_SENSOR = 0,
	POSITION_SENSOR_FOREACH_OKAY(POSITION_SENSOR_TOKEN)
} position_sensor_t;

typedef enum
{
	POSITION_SENSOR_TYPE_UNDEFINED = 0,
	HALL_TYPE,
	ABZ_TYPE,
	SINCOS_TYPE
} position_sensor_type_t;

typedef enum
{
	HALL_INTERPOLATION_NONE = 0,
	HALL_INTERPOLATION_LINEAR
} hall_interpolation_t;

typedef struct
{
	int8_t       direction_sign;
	uint8_t      pole_pairs;
	float32_t    electrical_offset;
} position_motor_config_t;

typedef struct
{
	uint8_t              hall_a_pin;
	uint8_t              hall_b_pin;
	uint8_t              hall_c_pin;
	uint8_t              hall_sector_table[6];
	hall_interpolation_t hall_interpolation;
} hall_position_sensor_config_t;

typedef struct
{
	timernumber_t timer;
	uint32_t      counts_per_revolution;
} incremental_encoder_position_sensor_config_t;

typedef struct
{
	uint8_t  sin_pin;
	uint8_t  cos_pin;
	sensor_t sin_sensor_name;
	sensor_t cos_sensor_name;
} sincos_position_sensor_config_t;

class PositionAPI
{
private:
	typedef union
	{
		uint32_t  raw_value;
		float32_t float_value;
	} int2float_t;

	typedef struct
	{
		const char*             name_string;
		position_sensor_t      name;
		position_sensor_type_t type;
		bool                   has_motor;
		bool                   has_direction_sign;
		bool                   has_pole_pairs;
		bool                   has_electrical_offset;
		bool                   has_counts_per_revolution;
		bool                   has_hall_sector_table;
		bool                   has_hall_interpolation;
		position_motor_config_t motor;
		union
		{
			hall_position_sensor_config_t hall;
			incremental_encoder_position_sensor_config_t incremental_encoder;
			sincos_position_sensor_config_t sincos;
		} configuration;
	} position_sensor_dt_data_t;

public:
	bool init(position_sensor_t sensor_name);
	bool initDefault();
	bool isInitialized();
	bool update(float32_t sampling_period);

	position_sensor_t getActiveSensor();
	position_sensor_type_t getActiveSensorType();

	const position_motor_config_t* getMotorConfig();
	const hall_position_sensor_config_t* getHallConfig();
	const incremental_encoder_position_sensor_config_t* getIncrementalEncoderConfig();
	const sincos_position_sensor_config_t* getSinCosConfig();

	uint8_t getHallState();
	uint32_t getIncrementalEncoderValue();
	float32_t getSinValue(uint8_t* dataValid = nullptr);
	float32_t getCosValue(uint8_t* dataValid = nullptr);

	void setDirectionSign(int8_t direction_sign);
	void setPolePairs(uint8_t pole_pairs);
	void setElectricalOffset(float32_t electrical_offset);
	bool setCountsPerRevolution(uint32_t counts_per_revolution);
	bool setHallSectorTable(const uint8_t hall_sector_table[6]);
	bool setHallInterpolation(hall_interpolation_t hall_interpolation);

	uint32_t getCountsPerRevolution();
	int8_t getDirectionSign();
	uint8_t getPolePairs();
	float32_t getElectricalOffset();
	const uint8_t* getHallSectorTable();
	hall_interpolation_t getHallInterpolation();
	float32_t getMechanicalAngle();
	float32_t getElectricalAngle();
	float32_t getMechanicalSpeed();
	float32_t getElectricalSpeed();
	float32_t getMechanicalPulsation();
	float32_t getElectricalPulsation();

private:
	static position_sensor_dt_data_t dt_position_sensors_props[];
	static bool initialized;
	static position_sensor_dt_data_t* active_sensor;

	position_sensor_dt_data_t* findSensor(position_sensor_t sensor_name);
	bool configureSelectedSensorHardware(position_sensor_dt_data_t* sensor_prop);
	void warnIfUsingDefaultConfiguration(position_sensor_dt_data_t* sensor_prop);
	void resetRuntimeState();
};

#endif /* POSITION_H_ */
