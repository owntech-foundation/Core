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

#include "Position.h"

#include <math.h>
#include <string.h>

#include <zephyr/sys/printk.h>

#include "filters.h"
#include "trigo.h"

#define POSITION_DEFAULT_DIRECTION_SIGN 1
#define POSITION_DEFAULT_POLE_PAIRS 1
#define POSITION_DEFAULT_COUNTS_PER_REVOLUTION 1024U
#define POSITION_DEFAULT_ELECTRICAL_OFFSET_RAW 0x00000000U
#define POSITION_DEFAULT_HALL_INTERPOLATION HALL_INTERPOLATION_NONE
#define POSITION_DEFAULT_HALL_SECTOR_TABLE {5U, 1U, 0U, 3U, 4U, 2U}

#define POSITION_HALL_A(node_id) \
	DT_GPIO_PIN_BY_IDX(node_id, hall_a_gpios, 0)

#define POSITION_HALL_B(node_id) \
	DT_GPIO_PIN_BY_IDX(node_id, hall_b_gpios, 0)

#define POSITION_HALL_C(node_id) \
	DT_GPIO_PIN_BY_IDX(node_id, hall_c_gpios, 0)

#define POSITION_TIMER_FROM_NODE(timer_node_id) \
	(DT_SAME_NODE(timer_node_id, DT_NODELABEL(timers4)) ? TIMER4 : TIMER3)

#define POSITION_TIMER(node_id) \
	POSITION_TIMER_FROM_NODE(DT_PHANDLE(node_id, timer))

#define POSITION_TIMER_IS_SUPPORTED(node_id) \
	(DT_SAME_NODE(DT_PHANDLE(node_id, timer), DT_NODELABEL(timers3)) || \
	 DT_SAME_NODE(DT_PHANDLE(node_id, timer), DT_NODELABEL(timers4)))

#define POSITION_ASSERT_SUPPORTED_TIMER(node_id) \
	BUILD_ASSERT(POSITION_TIMER_IS_SUPPORTED(node_id), \
		"Position API only supports timers3 or timers4 for incremental encoder mode");

#define POSITION_SIN_PIN(node_id) \
	DT_PROP(node_id, sin_spin_pin)

#define POSITION_COS_PIN(node_id) \
	DT_PROP(node_id, cos_spin_pin)

#define POSITION_SIN_SENSOR(node_id) \
	DT_STRING_TOKEN(node_id, sin_sensor_name)

#define POSITION_COS_SENSOR(node_id) \
	DT_STRING_TOKEN(node_id, cos_sensor_name)

#define MOTOR_NODE(node_id) DT_PHANDLE(node_id, motor)

#define POSITION_HAS_MOTOR(node_id) \
	DT_NODE_HAS_PROP(node_id, motor)

#define MOTOR_HAS_PROP(node_id, prop_name) \
	COND_CODE_1(POSITION_HAS_MOTOR(node_id), \
		(DT_NODE_HAS_PROP(MOTOR_NODE(node_id), prop_name)), \
		(0))

#define SENSOR_COUNTS_PER_REVOLUTION(node_id) \
	DT_PROP_OR(node_id, counts_per_revolution, \
			   POSITION_DEFAULT_COUNTS_PER_REVOLUTION)

#define SENSOR_DIRECTION_SIGN(node_id) \
	DT_PROP_OR(node_id, direction_sign, \
			   POSITION_DEFAULT_DIRECTION_SIGN)

#define SENSOR_ELECTRICAL_OFFSET(node_id) \
	DT_PROP_OR(node_id, electrical_offset, \
			   POSITION_DEFAULT_ELECTRICAL_OFFSET_RAW)

#define MOTOR_POLE_PAIRS(node_id) \
	COND_CODE_1(POSITION_HAS_MOTOR(node_id), \
		(DT_PROP_OR(MOTOR_NODE(node_id), pole_pairs, \
					POSITION_DEFAULT_POLE_PAIRS)), \
		(POSITION_DEFAULT_POLE_PAIRS))

#define SENSOR_HALL_TABLE(node_id) \
	DT_PROP_OR(node_id, hall_sector_table, \
			   POSITION_DEFAULT_HALL_SECTOR_TABLE)

#define MOTOR_HALL_INTERPOLATION_TOKEN(interpolation_token) \
	DT_CAT(HALL_INTERPOLATION_, interpolation_token)

#define SENSOR_HALL_INTERPOLATION(node_id) \
	(MOTOR_HALL_INTERPOLATION_TOKEN( \
		DT_STRING_TOKEN_OR(node_id, hall_interpolation, NONE)))

#define POSITION_COMMON_PROP(node_id, sensor_type_value)                          \
	.name_string = DT_PROP(node_id, position_sensor_name),                         \
	.name = DT_STRING_TOKEN(node_id, position_sensor_name),                       \
	.type = sensor_type_value,                                                    \
	.has_motor = POSITION_HAS_MOTOR(node_id),                                     \
	.has_direction_sign = DT_NODE_HAS_PROP(node_id, direction_sign),              \
	.has_pole_pairs = MOTOR_HAS_PROP(node_id, pole_pairs),                        \
	.has_electrical_offset = DT_NODE_HAS_PROP(node_id, electrical_offset),        \
	.has_counts_per_revolution = DT_NODE_HAS_PROP(node_id, counts_per_revolution),\
	.has_hall_sector_table = DT_NODE_HAS_PROP(node_id, hall_sector_table),        \
	.has_hall_interpolation = DT_NODE_HAS_PROP(node_id, hall_interpolation),      \
	.motor =                                                                      \
	{                                                                             \
		.direction_sign = SENSOR_DIRECTION_SIGN(node_id),                         \
		.pole_pairs = MOTOR_POLE_PAIRS(node_id),                                  \
		.electrical_offset = int2float_t{.raw_value = SENSOR_ELECTRICAL_OFFSET(node_id)}.float_value \
	}

#define POSITION_WRITE_PROP_HALL(node_id)                                         \
	{                                                                             \
		POSITION_COMMON_PROP(node_id, HALL_TYPE),                                 \
		.configuration =                                                          \
		{                                                                         \
			.hall =                                                               \
			{                                                                     \
				.hall_a_pin = POSITION_HALL_A(node_id),                           \
				.hall_b_pin = POSITION_HALL_B(node_id),                           \
				.hall_c_pin = POSITION_HALL_C(node_id),                           \
				.hall_sector_table = SENSOR_HALL_TABLE(node_id),                  \
				.hall_interpolation = SENSOR_HALL_INTERPOLATION(node_id),         \
			}                                                                     \
		}                                                                         \
	},

#define POSITION_WRITE_PROP_ABZ(node_id)                                          \
	{                                                                             \
		POSITION_COMMON_PROP(node_id, ABZ_TYPE),                                  \
		.configuration =                                                          \
		{                                                                         \
			.incremental_encoder =                                                \
			{                                                                     \
				.timer = POSITION_TIMER(node_id),                                 \
				.counts_per_revolution = SENSOR_COUNTS_PER_REVOLUTION(node_id),   \
			}                                                                     \
		}                                                                         \
	},

#define POSITION_WRITE_PROP_SINCOS(node_id)                                       \
	{                                                                             \
		POSITION_COMMON_PROP(node_id, SINCOS_TYPE),                               \
		.configuration =                                                          \
		{                                                                         \
			.sincos =                                                             \
			{                                                                     \
				.sin_pin = POSITION_SIN_PIN(node_id),                             \
				.cos_pin = POSITION_COS_PIN(node_id),                             \
				.sin_sensor_name = POSITION_SIN_SENSOR(node_id),                  \
				.cos_sensor_name = POSITION_COS_SENSOR(node_id),                  \
			}                                                                     \
		}                                                                         \
	},

#define POSITION_COUNT(node_id) +1
#define DT_POSITION_SENSOR_COUNT \
	(0 \
	 DT_FOREACH_STATUS_OKAY(shield_position_hall, POSITION_COUNT) \
	 DT_FOREACH_STATUS_OKAY(shield_position_abz, POSITION_COUNT) \
	 DT_FOREACH_STATUS_OKAY(shield_position_sincos, POSITION_COUNT))

DT_FOREACH_STATUS_OKAY(shield_position_abz, POSITION_ASSERT_SUPPORTED_TIMER)

#if DT_POSITION_SENSOR_COUNT > 0
PositionAPI::position_sensor_dt_data_t PositionAPI::dt_position_sensors_props[] =
{
	DT_FOREACH_STATUS_OKAY(shield_position_hall, POSITION_WRITE_PROP_HALL)
	DT_FOREACH_STATUS_OKAY(shield_position_abz, POSITION_WRITE_PROP_ABZ)
	DT_FOREACH_STATUS_OKAY(shield_position_sincos, POSITION_WRITE_PROP_SINCOS)
};
#else
PositionAPI::position_sensor_dt_data_t PositionAPI::dt_position_sensors_props[1] = {};
#endif

bool PositionAPI::initialized = false;
PositionAPI::position_sensor_dt_data_t* PositionAPI::active_sensor = nullptr;

typedef struct
{
	bool      initialized;
	float32_t elapsed_time;
	float32_t hall_time_since_transition;
	uint8_t   hall_state;
	uint8_t   hall_state_previous;
	int16_t   hall_sector;
	int16_t   hall_sector_previous;
	float32_t hall_transition_time;
	PllAngle  hall_pll;
	PllDatas  hall_pll_datas;
	float32_t hall_pll_sampling_period;
	uint32_t  incremental_encoder_count;
	uint32_t  incremental_encoder_count_previous;
	float32_t signed_sincos_angle;
	float32_t signed_sincos_angle_previous;
	float32_t mechanical_angle;
	float32_t electrical_angle;
	float32_t mechanical_speed;
	float32_t electrical_speed;
} position_runtime_state_t;

static position_runtime_state_t runtime_state = {};

static int8_t normalize_hall_delta(uint8_t current_sector, uint8_t previous_sector)
{
	int8_t delta = (int8_t)current_sector - (int8_t)previous_sector;

	if (delta > 3)
	{
		delta -= 6;
	}
	else if (delta < -3)
	{
		delta += 6;
	}

	return delta;
}

static bool is_valid_hall_sector_table(const uint8_t hall_sector_table[6])
{
	bool seen[6] = {false};

	for (uint8_t i = 0; i < 6U; i++)
	{
		uint8_t sector = hall_sector_table[i];
		if (sector > 5U)
		{
			return false;
		}

		if (seen[sector] == true)
		{
			return false;
		}

		seen[sector] = true;
	}

	return true;
}

static int32_t normalize_encoder_delta(uint32_t current_count,
									   uint32_t previous_count,
									   uint32_t counts_per_revolution)
{
	int32_t delta = (int32_t)current_count - (int32_t)previous_count;

	if (counts_per_revolution == 0U)
	{
		return delta;
	}

	int32_t half_counts_per_revolution = (int32_t)(counts_per_revolution / 2U);

	if (delta > half_counts_per_revolution)
	{
		delta -= (int32_t)counts_per_revolution;
	}
	else if (delta < -half_counts_per_revolution)
	{
		delta += (int32_t)counts_per_revolution;
	}

	return delta;
}

static float32_t wrap_pm_pi(float32_t angle)
{
	while (angle > PI)
	{
		angle -= 2.0F * PI;
	}

	while (angle < -PI)
	{
		angle += 2.0F * PI;
	}

	return angle;
}

bool PositionAPI::init(position_sensor_t sensor_name)
{
	position_sensor_dt_data_t* sensor_prop = findSensor(sensor_name);
	if (sensor_prop == nullptr)
	{
		return false;
	}

	if (configureSelectedSensorHardware(sensor_prop) == false)
	{
		return false;
	}

	active_sensor = sensor_prop;
	resetRuntimeState();
	warnIfUsingDefaultConfiguration(sensor_prop);
	initialized = true;

	return true;
}

bool PositionAPI::initDefault()
{
#if DT_HAS_CHOSEN(owntech_position_sensor)
	return init(DT_STRING_TOKEN(DT_CHOSEN(owntech_position_sensor),
								position_sensor_name));
#else
	return false;
#endif
}

bool PositionAPI::isInitialized()
{
	return initialized;
}

bool PositionAPI::update(float32_t sampling_period)
{
	if ((active_sensor == nullptr) || (sampling_period <= 0.0F))
	{
		return false;
	}

	const position_motor_config_t* motor_config = getMotorConfig();
	float32_t electrical_offset =
		(motor_config != nullptr) ? motor_config->electrical_offset : 0.0F;
	uint8_t pole_pairs = (motor_config != nullptr) ? motor_config->pole_pairs : 0U;
	int8_t direction_sign = (motor_config != nullptr) ? motor_config->direction_sign : 1;
	runtime_state.elapsed_time += sampling_period;

	switch (active_sensor->type)
	{
		case HALL_TYPE:
		{
			const hall_position_sensor_config_t* hall_config = getHallConfig();
			if (hall_config == nullptr)
			{
				return false;
			}

			runtime_state.hall_time_since_transition += sampling_period;

			uint8_t hall_state = getHallState();
			runtime_state.hall_state = hall_state;

			if ((hall_state == 0U) || (hall_state == 7U))
			{
				return false;
			}

			uint8_t hall_sector = hall_config->hall_sector_table[hall_state - 1U];
			runtime_state.hall_sector = hall_sector;
			float32_t base_mechanical_angle =
				ot_modulo_2pi((float32_t)direction_sign *
							  ((PI / 3.0F) * (float32_t)hall_sector));

			float32_t base_electrical_angle =
				ot_modulo_2pi(base_mechanical_angle +
							  electrical_offset);

			if (hall_config->hall_interpolation == HALL_INTERPOLATION_LINEAR)
			{
				if ((runtime_state.initialized == false) ||
					(runtime_state.hall_pll_sampling_period != sampling_period))
				{
					runtime_state.hall_pll.init(sampling_period, 10.0F, 0.04F);
					runtime_state.hall_pll.reset(base_electrical_angle);
					runtime_state.hall_pll_sampling_period = sampling_period;
				}

				runtime_state.hall_pll_datas =
					runtime_state.hall_pll.calculateWithReturn(base_electrical_angle);
				runtime_state.electrical_angle = runtime_state.hall_pll_datas.angle;
				runtime_state.electrical_speed = runtime_state.hall_pll_datas.w;
			}
			else
			{
				runtime_state.electrical_angle = base_electrical_angle;

				if (runtime_state.initialized == false)
				{
					runtime_state.electrical_speed = 0.0F;
				}
				else if (hall_sector != (uint8_t)runtime_state.hall_sector_previous)
				{
					int8_t sector_delta = normalize_hall_delta(
						hall_sector,
						(uint8_t)runtime_state.hall_sector_previous);

					if (runtime_state.hall_time_since_transition > 0.0F)
					{
						runtime_state.electrical_speed =
							(float32_t)direction_sign *
							((PI / 3.0F) * (float32_t)sector_delta) /
							runtime_state.hall_time_since_transition;
					}

					runtime_state.hall_time_since_transition = 0.0F;
				}
			}

			runtime_state.hall_state_previous = hall_state;
			runtime_state.hall_sector_previous = hall_sector;

			if (pole_pairs > 0U)
			{
				if (hall_config->hall_interpolation == HALL_INTERPOLATION_LINEAR)
				{
					float32_t signed_mechanical_angle =
						(runtime_state.electrical_angle - electrical_offset) /
						(float32_t)pole_pairs;
					runtime_state.mechanical_angle =
						ot_modulo_2pi(signed_mechanical_angle);
				}
				else
				{
					runtime_state.mechanical_angle =
						ot_modulo_2pi(base_mechanical_angle /
									  (float32_t)pole_pairs);
				}
				runtime_state.mechanical_speed =
					runtime_state.electrical_speed / (float32_t)pole_pairs;
			}
			else
			{
				runtime_state.mechanical_angle = 0.0F;
				runtime_state.mechanical_speed = 0.0F;
			}

			break;
		}

		case ABZ_TYPE:
		{
			const incremental_encoder_position_sensor_config_t* encoder_config =
				getIncrementalEncoderConfig();
			if ((encoder_config == nullptr) ||
				(encoder_config->counts_per_revolution == 0U))
			{
				return false;
			}

			uint32_t current_count =
				getIncrementalEncoderValue() %
				encoder_config->counts_per_revolution;

			if (runtime_state.initialized == false)
			{
				runtime_state.incremental_encoder_count_previous = current_count;
			}

			int32_t delta_count = normalize_encoder_delta(
				current_count,
				runtime_state.incremental_encoder_count_previous,
				encoder_config->counts_per_revolution);

			runtime_state.incremental_encoder_count = current_count;
			runtime_state.incremental_encoder_count_previous = current_count;

			float32_t signed_mechanical_angle =
				(float32_t)direction_sign *
				(2.0F * PI * (float32_t)current_count /
				 (float32_t)encoder_config->counts_per_revolution);

			runtime_state.mechanical_angle = ot_modulo_2pi(signed_mechanical_angle);
			runtime_state.mechanical_speed =
				(float32_t)direction_sign *
				(2.0F * PI * (float32_t)delta_count) /
				((float32_t)encoder_config->counts_per_revolution *
				 sampling_period);

			runtime_state.electrical_angle =
				ot_modulo_2pi((float32_t)pole_pairs *
							  runtime_state.mechanical_angle +
							  electrical_offset);
			runtime_state.electrical_speed =
				(float32_t)pole_pairs * runtime_state.mechanical_speed;

			break;
		}

		case SINCOS_TYPE:
		{
			uint8_t sin_valid = 0U;
			uint8_t cos_valid = 0U;
			float32_t sin_value = getSinValue(&sin_valid);
			float32_t cos_value = getCosValue(&cos_valid);

			if ((sin_valid == 0U) || (cos_valid == 0U) ||
				(sin_value == NO_VALUE) || (cos_value == NO_VALUE))
			{
				return false;
			}

			float32_t raw_mechanical_angle = atan2f(sin_value, cos_value);
			if (raw_mechanical_angle < 0.0F)
			{
				raw_mechanical_angle += 2.0F * PI;
			}

			float32_t signed_mechanical_angle =
				(float32_t)direction_sign * raw_mechanical_angle;

			if (runtime_state.initialized == false)
			{
				runtime_state.signed_sincos_angle_previous = signed_mechanical_angle;
			}

			float32_t delta_angle =
				wrap_pm_pi(signed_mechanical_angle -
						   runtime_state.signed_sincos_angle_previous);

			runtime_state.signed_sincos_angle = signed_mechanical_angle;
			runtime_state.signed_sincos_angle_previous = signed_mechanical_angle;
			runtime_state.mechanical_angle =
				ot_modulo_2pi(signed_mechanical_angle);
			runtime_state.mechanical_speed = delta_angle / sampling_period;
			runtime_state.electrical_angle =
				ot_modulo_2pi((float32_t)pole_pairs *
							  runtime_state.mechanical_angle +
							  electrical_offset);
			runtime_state.electrical_speed =
				(float32_t)pole_pairs * runtime_state.mechanical_speed;

			break;
		}

		case POSITION_SENSOR_TYPE_UNDEFINED:
		default:
			return false;
	}

	runtime_state.initialized = true;
	return true;
}

position_sensor_t PositionAPI::getActiveSensor()
{
	if (active_sensor == nullptr)
	{
		return UNDEFINED_POSITION_SENSOR;
	}

	return active_sensor->name;
}

position_sensor_type_t PositionAPI::getActiveSensorType()
{
	if (active_sensor == nullptr)
	{
		return POSITION_SENSOR_TYPE_UNDEFINED;
	}

	return active_sensor->type;
}

const position_motor_config_t* PositionAPI::getMotorConfig()
{
	if (active_sensor == nullptr)
	{
		return nullptr;
	}

	return &active_sensor->motor;
}

const hall_position_sensor_config_t* PositionAPI::getHallConfig()
{
	if ((active_sensor == nullptr) || (active_sensor->type != HALL_TYPE))
	{
		return nullptr;
	}

	return &active_sensor->configuration.hall;
}

const incremental_encoder_position_sensor_config_t*
PositionAPI::getIncrementalEncoderConfig()
{
	if ((active_sensor == nullptr) || (active_sensor->type != ABZ_TYPE))
	{
		return nullptr;
	}

	return &active_sensor->configuration.incremental_encoder;
}

const sincos_position_sensor_config_t* PositionAPI::getSinCosConfig()
{
	if ((active_sensor == nullptr) || (active_sensor->type != SINCOS_TYPE))
	{
		return nullptr;
	}

	return &active_sensor->configuration.sincos;
}

uint8_t PositionAPI::getHallState()
{
	const hall_position_sensor_config_t* hall_config = getHallConfig();
	if (hall_config == nullptr)
	{
		return 0;
	}

	uint8_t hall_a = spin.gpio.readPin(hall_config->hall_a_pin);
	uint8_t hall_b = spin.gpio.readPin(hall_config->hall_b_pin);
	uint8_t hall_c = spin.gpio.readPin(hall_config->hall_c_pin);

	return (uint8_t)(hall_a + (2U * hall_b) + (4U * hall_c));
}

uint32_t PositionAPI::getIncrementalEncoderValue()
{
	const incremental_encoder_position_sensor_config_t* incremental_encoder_config =
		getIncrementalEncoderConfig();
	if (incremental_encoder_config == nullptr)
	{
		return 0U;
	}

	return spin.timer.getIncrementalEncoderValue(incremental_encoder_config->timer);
}

float32_t PositionAPI::getSinValue(uint8_t* dataValid)
{
	const sincos_position_sensor_config_t* sincos_config = getSinCosConfig();
	if (sincos_config == nullptr)
	{
		if (dataValid != nullptr) *dataValid = 0U;
		return NO_VALUE;
	}

	SensorsAPI sensors_api;
	return sensors_api.getLatestValue(sincos_config->sin_sensor_name, dataValid);
}

float32_t PositionAPI::getCosValue(uint8_t* dataValid)
{
	const sincos_position_sensor_config_t* sincos_config = getSinCosConfig();
	if (sincos_config == nullptr)
	{
		if (dataValid != nullptr) *dataValid = 0U;
		return NO_VALUE;
	}

	SensorsAPI sensors_api;
	return sensors_api.getLatestValue(sincos_config->cos_sensor_name, dataValid);
}

void PositionAPI::setDirectionSign(int8_t direction_sign)
{
	if (active_sensor == nullptr)
	{
		return;
	}

	if ((direction_sign != -1) && (direction_sign != 1))
	{
		printk("WARNING: invalid direction-sign %d for position sensor %s; expected -1 or 1.\n",
			   direction_sign,
			   active_sensor->name_string);
		return;
	}

	active_sensor->motor.direction_sign = direction_sign;
	resetRuntimeState();
}

void PositionAPI::setPolePairs(uint8_t pole_pairs)
{
	if (active_sensor == nullptr)
	{
		return;
	}

	if (pole_pairs == 0U)
	{
		printk("WARNING: invalid pole-pairs 0 for position sensor %s; expected a strictly positive value.\n",
			   active_sensor->name_string);
		return;
	}

	active_sensor->motor.pole_pairs = pole_pairs;
	resetRuntimeState();
}

void PositionAPI::setElectricalOffset(float32_t electrical_offset)
{
	if (active_sensor == nullptr)
	{
		return;
	}

	active_sensor->motor.electrical_offset = electrical_offset;
	resetRuntimeState();
}

bool PositionAPI::setCountsPerRevolution(uint32_t counts_per_revolution)
{
	incremental_encoder_position_sensor_config_t* encoder_config =
		const_cast<incremental_encoder_position_sensor_config_t*>(
			getIncrementalEncoderConfig());
	if ((encoder_config == nullptr) || (counts_per_revolution == 0U))
	{
		return false;
	}

	encoder_config->counts_per_revolution = counts_per_revolution;
	resetRuntimeState();
	return true;
}

bool PositionAPI::setHallSectorTable(const uint8_t hall_sector_table[6])
{
	hall_position_sensor_config_t* hall_config =
		const_cast<hall_position_sensor_config_t*>(getHallConfig());
	if ((hall_config == nullptr) || (hall_sector_table == nullptr))
	{
		return false;
	}

	if (is_valid_hall_sector_table(hall_sector_table) == false)
	{
		return false;
	}

	for (uint8_t i = 0; i < 6U; i++)
	{
		hall_config->hall_sector_table[i] = hall_sector_table[i];
	}
	resetRuntimeState();
	return true;
}

bool PositionAPI::setHallInterpolation(hall_interpolation_t hall_interpolation)
{
	hall_position_sensor_config_t* hall_config =
		const_cast<hall_position_sensor_config_t*>(getHallConfig());
	if (hall_config == nullptr)
	{
		return false;
	}

	hall_config->hall_interpolation = hall_interpolation;
	resetRuntimeState();
	return true;
}

uint32_t PositionAPI::getCountsPerRevolution()
{
	const incremental_encoder_position_sensor_config_t* incremental_encoder_config =
		getIncrementalEncoderConfig();
	if (incremental_encoder_config == nullptr)
	{
		return 0U;
	}

	return incremental_encoder_config->counts_per_revolution;
}

int8_t PositionAPI::getDirectionSign()
{
	const position_motor_config_t* motor_config = getMotorConfig();
	if (motor_config == nullptr)
	{
		return 1;
	}

	return motor_config->direction_sign;
}

uint8_t PositionAPI::getPolePairs()
{
	const position_motor_config_t* motor_config = getMotorConfig();
	if (motor_config == nullptr)
	{
		return 0U;
	}

	return motor_config->pole_pairs;
}

float32_t PositionAPI::getElectricalOffset()
{
	const position_motor_config_t* motor_config = getMotorConfig();
	if (motor_config == nullptr)
	{
		return 0.0F;
	}

	return motor_config->electrical_offset;
}

const uint8_t* PositionAPI::getHallSectorTable()
{
	const hall_position_sensor_config_t* hall_config = getHallConfig();
	if (hall_config == nullptr)
	{
		return nullptr;
	}

	return hall_config->hall_sector_table;
}

hall_interpolation_t PositionAPI::getHallInterpolation()
{
	const hall_position_sensor_config_t* hall_config = getHallConfig();
	if (hall_config == nullptr)
	{
		return HALL_INTERPOLATION_NONE;
	}

	return hall_config->hall_interpolation;
}

float32_t PositionAPI::getMechanicalAngle()
{
	return runtime_state.mechanical_angle;
}

float32_t PositionAPI::getElectricalAngle()
{
	return runtime_state.electrical_angle;
}

float32_t PositionAPI::getMechanicalSpeed()
{
	return runtime_state.mechanical_speed;
}

float32_t PositionAPI::getElectricalSpeed()
{
	return runtime_state.electrical_speed;
}

void PositionAPI::warnIfUsingDefaultConfiguration(
	position_sensor_dt_data_t* sensor_prop)
{
	if (sensor_prop == nullptr)
	{
		return;
	}

	if (sensor_prop->has_motor == false)
	{
		printk("WARNING: position sensor %s has no motor configuration node; using default pole-pairs value 1.\n",
			   sensor_prop->name_string);
	}

	if (sensor_prop->has_direction_sign == false)
	{
		printk("WARNING: position sensor %s is missing direction-sign; using default value 1.\n",
			   sensor_prop->name_string);
	}

	if ((sensor_prop->has_motor == true) &&
		(sensor_prop->has_pole_pairs == false))
	{
		printk("WARNING: position sensor %s references a motor node without pole-pairs; using default value 1.\n",
			   sensor_prop->name_string);
	}

	if (sensor_prop->has_electrical_offset == false)
	{
		printk("WARNING: position sensor %s is missing electrical-offset; using default value 0.0.\n",
			   sensor_prop->name_string);
	}

	if ((sensor_prop->type == ABZ_TYPE) &&
		(sensor_prop->has_counts_per_revolution == false))
	{
		printk("WARNING: position sensor %s is missing counts-per-revolution; using default value 1024.\n",
			   sensor_prop->name_string);
	}

	if (sensor_prop->type == HALL_TYPE)
	{
		if (sensor_prop->has_hall_sector_table == false)
		{
			printk("WARNING: position sensor %s is missing hall-sector-table; using default sequence {5, 1, 0, 3, 4, 2}.\n",
				   sensor_prop->name_string);
		}

		if (sensor_prop->has_hall_interpolation == false)
		{
			printk("WARNING: position sensor %s is missing hall-interpolation; using NONE.\n",
				   sensor_prop->name_string);
		}
	}
}

float32_t PositionAPI::getMechanicalPulsation()
{
	return runtime_state.mechanical_speed;
}

float32_t PositionAPI::getElectricalPulsation()
{
	return runtime_state.electrical_speed;
}

PositionAPI::position_sensor_dt_data_t* PositionAPI::findSensor(
	position_sensor_t sensor_name)
{
#if DT_POSITION_SENSOR_COUNT > 0
	for (uint8_t sensor_index = 0; sensor_index < DT_POSITION_SENSOR_COUNT; sensor_index++)
	{
		if (dt_position_sensors_props[sensor_index].name == sensor_name)
		{
			return &dt_position_sensors_props[sensor_index];
		}
	}
#endif

	return nullptr;
}

bool PositionAPI::configureSelectedSensorHardware(
	position_sensor_dt_data_t* sensor_prop)
{
	if (sensor_prop == nullptr)
	{
		return false;
	}

	switch (sensor_prop->type)
	{
		case HALL_TYPE:
			spin.gpio.configurePin(sensor_prop->configuration.hall.hall_a_pin, INPUT);
			spin.gpio.configurePin(sensor_prop->configuration.hall.hall_b_pin, INPUT);
			spin.gpio.configurePin(sensor_prop->configuration.hall.hall_c_pin, INPUT);
			return true;

		case ABZ_TYPE:
			spin.timer.startLogIncrementalEncoder(
				sensor_prop->configuration.incremental_encoder.timer);
			return true;

		case SINCOS_TYPE:
		{
			SensorsAPI sensors_api;
			int8_t sin_enable_status = sensors_api.enablePositionSensor(
				sensor_prop->configuration.sincos.sin_sensor_name);
			int8_t cos_enable_status = sensors_api.enablePositionSensor(
				sensor_prop->configuration.sincos.cos_sensor_name);

			if ((sin_enable_status < 0) || (cos_enable_status < 0))
			{
				printk("WARNING: failed to enable Sin/Cos acquisition for position sensor %s.\n",
					   sensor_prop->name_string);
				return false;
			}

			return true;
		}

		case POSITION_SENSOR_TYPE_UNDEFINED:
		default:
			return false;
	}
}

void PositionAPI::resetRuntimeState()
{
	runtime_state = {};
	runtime_state.hall_pll.reset(0.0F);
}
