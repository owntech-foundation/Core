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

#include "SafetyAPI.h"
#include "../src/safety_shield.h"
#include "../src/safety_setting.h"

safety Safety;

void safety::init_shield()
{
    safety_init_shield(true);
}

void safety::init_shield(sensor_t* sensors_watch, uint8_t sensors_watch_number)
{
    safety_init_shield(false);
    safety_set_sensor_watch(sensors_watch, sensors_watch_number);
}

int8_t safety::setChannelWatch(sensor_t* sensors_watch, uint8_t sensors_watch_number)
{
    int8_t status =  safety_set_sensor_watch(sensors_watch, sensors_watch_number);
    return status;
}

int8_t safety::unsetChannelWatch(sensor_t* sensors_watch, uint8_t sensors_watch_number)
{
    int8_t status =  safety_unset_sensor_watch(sensors_watch, sensors_watch_number);
    return status;
}

bool safety::getChannelWatch(sensor_t  sensors_watch)
{
    bool Is_watched = safety_get_sensor_watch(sensors_watch);
    return Is_watched;
}

void safety::setChannelReaction(safety_reaction_t sensors_reaction)
{
    safety_set_sensor_reaction(sensors_reaction);

}

safety_reaction_t safety::getChannelReaction()
{
    safety_reaction_t sensors_reaction = safety_get_sensor_reaction();
    return sensors_reaction;
}

int8_t safety::setChannelThresholdMax(sensor_t *sensors_threshold, float32_t *threshold_max, uint8_t sensors_threshold_number)
{
    uint8_t ret =  safety_set_sensor_threshold_max(sensors_threshold, threshold_max, sensors_threshold_number);
    return ret;
}

int8_t safety::setChannelThresholdMin(sensor_t *sensors_threshold, float32_t *threshold_min, uint8_t sensors_threshold_number)
{
    uint8_t ret =  safety_set_sensor_threshold_min(sensors_threshold, threshold_min, sensors_threshold_number);
    return ret;
}

float32_t safety::getChannelThresholdMax(sensor_t sensors_threshold)
{
    float32_t threshold = safety_get_sensor_threshold_max(sensors_threshold);
    return threshold;
}

float32_t safety::getChannelThresholdMin(sensor_t sensors_threshold)
{
    float32_t threshold = safety_get_sensor_threshold_min(sensors_threshold);
    return threshold;
}

bool safety::getChannelError(sensor_t sensors_error)
{
    bool error_status = safety_get_sensor_error(sensors_error);
    return error_status;
}

void safety::enableSafetyApi()
{
    safety_enable_task();
}

void safety::disableSafetyApi()
{
    safety_disable_task();
}

int8_t safety::storeThreshold(sensor_t sensor_threshold_store)
{
    uint8_t ret = safety_store_threshold_in_nvs(sensor_threshold_store);
    return ret;
}

int8_t safety::retrieveThreshold(sensor_t sensor_threshold_retrieve)
{
    uint8_t ret = safety_retrieve_threshold_in_nvs(sensor_threshold_retrieve);
    return ret;
}