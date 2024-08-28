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
 * @date   2024
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 * @brief  this file has initialization specific to TWIST
 */

/* Include */
#include "safety_setting.h"

/* Current Header */
#include "safety_shield.h"

#define TRHRESHOLD_WRITE_PROP(node_id) \
        { \
            .sensor = DT_STRING_TOKEN(node_id, sensor_name),\
            .name = DT_PROP(node_id, threshold_name), \
            .threshold_min = DT_PROP(node_id, threshold_low),  \
            .threshold_max = DT_PROP(node_id, threshold_high),  \
        },    \

/* *
 * Define the number of sensors that need to be monitored for safety purpose.
*/
#define THRESHOLD_COUNTER(node_id) +1
#define DT_THRESHOLDS_NUMBER DT_FOREACH_STATUS_OKAY(safety_thresholds, THRESHOLD_COUNTER)

struct threshold_prop_t{
    sensor_t sensor;
    char const *name;
    uint32_t threshold_min;
    uint32_t threshold_max;
};

/* Struct wich contains all the propriety from the device tree */
static threshold_prop_t dt_threshold_props[] =
{
	DT_FOREACH_STATUS_OKAY(safety_thresholds, TRHRESHOLD_WRITE_PROP)
};

/**
 * @brief Initializes the threshold min and max for all the sensors with th default value
 *        from the device tree if values not found in the static storage.
*/
void safety_init_shield(bool watch_all)
{
    for(uint8_t i = 0; i < DT_THRESHOLDS_NUMBER; i++)
    {
        uint8_t rc = safety_retrieve_threshold_in_nvs(dt_threshold_props[i].sensor);
        if(rc != 0)
        {
            printk("%s value not found in static storage. Default value will be used \n", dt_threshold_props[i].name);
            safety_set_sensor_threshold_max(&( dt_threshold_props[i].sensor ),
                                            (float32_t*)(&( dt_threshold_props[i].threshold_max )), 1);
            safety_set_sensor_threshold_min(&( dt_threshold_props[i].sensor ),
                                            (float32_t*)(&( dt_threshold_props[i].threshold_min )), 1);
        }
        else
        {
            printk("%s value found in static storage.\n", dt_threshold_props[i].name);
        }

        if(watch_all) safety_set_sensor_watch(&( dt_threshold_props[i].sensor ), 1);
    }
}
