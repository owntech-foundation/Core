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
 * @brief  This file contains the essentials functions to set safety limits
 */

/* Header */
#include "safety_setting.h"
#include "safety_internal.h"

/* Includes */

//LL libraries
#include "stm32_ll_gpio.h"
#include "stm32_ll_bus.h"

// OWNTECH APIs
#include "nvs_storage.h"
#include "SpinAPI.h"
#include "TwistAPI.h"

// Zephyr
#include "zephyr/kernel.h"

/* Defines */

/**
 * Defines the channel numbers.
 * For twist we have :
 * V1_LOW, V2_LOW, V_HIGH, I1_LOW, I2_LOW, I_HIGH, TEMP_SENSOR, EXTRA_MEAS, ANALOG_COMM
 */
#define CHANNEL_COUNTER(node_id) +1
#define DT_CHANNELS_NUMBER DT_FOREACH_STATUS_OKAY(shield_sensors, CHANNEL_COUNTER)

/**
 * Counts the number of LEGs (i.e. the converters that need to be stopped for safety)
*/
#define POWER_SHIELD_ID DT_NODELABEL(power_shield)
#define LEG_COUNTER(node_id) +1
#define DT_LEG_NUMBER DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_COUNTER)

/**
 * Retrieves PWM GPIO pin number
*/
#define LEG_PWM_PIN_HIGH(node_id)	DT_PROP_BY_IDX(node_id, pwm_pin_num, 0),
#define LEG_PWM_PIN_LOW(node_id)	DT_PROP_BY_IDX(node_id, pwm_pin_num, 1),

/* Global variables */

static bool channel_watch[DT_CHANNELS_NUMBER + 1];             // channels that need to be watched (true) / ignored (false)
static float32_t channel_threshold_max[DT_CHANNELS_NUMBER + 1]; // threshold max for each channel
static float32_t channel_threshold_min[DT_CHANNELS_NUMBER + 1]; // threshold min for each channel
static safety_reaction_t channel_reaction = Open_Circuit;      // Reaction type by default in open circuit mode
static bool channel_errors[DT_CHANNELS_NUMBER + 1];            // channel that went over/below the threshold (true)

static uint8_t dt_pin_high_side[] = { DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_PWM_PIN_HIGH) }; // Pin number of the gpio driving high side switchs
static uint8_t dt_pin_low_side[] = { DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_PWM_PIN_LOW) };   // Pin number of the gpio driving low side switchs

/**
 * The purpose of safety_alert_counter is to have a delay when we detect a problem.
 * For example here we wait that safety_alert_counter = 5 before trigering a safety alert,
 * if the control task = 100µs then we wait 0.5ms before enabling short-circuit and open-circuit mode.
 * Thus we avoid stopping everything because of transient surge of current or voltage which are not really a
 * problem
*/
static uint8_t safety_alert_counter = 0;

static bool safety_enable = true; // enable the safety API watch and action task

///// Private functions

/**
 * @brief This function enables the short-circuit mode
 *        i.e. the high-side switch is left open and
 *        the low-side switch is closed. Can be used to
 *        brake DC motor for example.
*/
void _short_circuit(void)
{
    if(DT_LEG_NUMBER == 0) return;

    for(uint8_t i = 0; i < DT_LEG_NUMBER; i++)
    {
        spin.gpio.configurePin(dt_pin_high_side[i], OUTPUT);
        spin.gpio.configurePin(dt_pin_low_side[i], OUTPUT);
        spin.gpio.resetPin(dt_pin_high_side[i]);
        spin.gpio.setPin(dt_pin_low_side[i]);

    }
}

/**
 * @brief This function enables the open-circuit mode
 *        i.e. the high-side switch is opened and
 *        the low-side switch is opened.
*/
void _open_circuit(void)
{
    if(DT_LEG_NUMBER == 0) return;

    for(uint8_t i = 0; i < DT_LEG_NUMBER; i++)
    {
        spin.gpio.configurePin(dt_pin_high_side[i], OUTPUT);
        spin.gpio.configurePin(dt_pin_low_side[i], OUTPUT);
        spin.gpio.resetPin(dt_pin_high_side[i]);
        spin.gpio.resetPin(dt_pin_low_side[i]);

    }
}

///// Public functions

/**
 * @brief Sets the channels that need to be monitored
 */
int8_t safety_set_channel_watch(channel_t * safety_channels, uint8_t channels_number)
{
    if (channels_number > DT_CHANNELS_NUMBER)
    {
        printk("ERROR: number of channels superior to number of channels defined in device tree");
        return -1;
    }

    for (uint8_t i = 0; i < channels_number; i++)
    {
        channel_watch[safety_channels[i]] = true;
    }

    return 0;
}

/**
 * @brief Gets if chosen channel is being monitored or not
 */
bool safety_get_channel_watch(channel_t  safety_channels)
{
    return channel_watch[safety_channels];
}

/**
 * @brief Unsets from the watching list the channels that do not need to be monitored
 */
int8_t safety_unset_channel_watch(channel_t * safety_channels, uint8_t channels_number)
{
    if (channels_number > DT_CHANNELS_NUMBER)
    {
        printk("ERROR: number of channels superior to number of channels defined in device tree");
        return -1;
    }

    for (uint8_t i = 0; i < channels_number; i++)
    {
        channel_watch[safety_channels[i]] = false;
    }

    return 0;
}

/**
 * @brief Sets reaction type : Short-Circuit or Open-Circuit
 */
void safety_set_channel_reaction(safety_reaction_t reaction)
{

    channel_reaction = reaction;
}

/**
 * @brief Gets reaction type : Short-Circuit or Open-Circuit
 */
safety_reaction_t safety_get_channel_reaction()
{

    return channel_reaction;
}

/**
 * @brief Sets channel threshold
 */
int8_t safety_set_channel_threshold_max(channel_t *safety_channels, float32_t *threshold, uint8_t channels_number)
{
    if (channels_number > DT_CHANNELS_NUMBER)
    {
        printk("ERROR: number of channels superior to number of channels defined in device tree");
        return -1;
    }

    for (uint8_t i = 0; i < channels_number; i++)
    {
        channel_threshold_max[safety_channels[i]] = threshold[i];
    }

    return 0;
}

/**
 * @brief Sets channel threshold
 */
int8_t safety_set_channel_threshold_min(channel_t *safety_channels, float32_t *threshold, uint8_t channels_number)
{
    if (channels_number > DT_CHANNELS_NUMBER)
    {
        printk("ERROR: number of channels superior to number of channels defined in device tree");
        return -1;
    }

    for (uint8_t i = 0; i < channels_number; i++)
    {
        channel_threshold_min[safety_channels[i]] = threshold[i];
    }

    return 0;
}

/**
 * @brief Returns the minimum threshold
*/
float32_t safety_get_channel_threshold_min(channel_t safety_channel)
{
    return channel_threshold_min[safety_channel];
}

/**
 * @brief Returns the maximum threshold
*/
float32_t safety_get_channel_threshold_max(channel_t safety_channel)
{
    return channel_threshold_max[safety_channel];
}

/**
 * @brief Returns if an error has been detected
*/
bool safety_get_channel_error(channel_t safety_channel)
{
    return channel_errors[safety_channel];
}

/**
 * @brief Monitors measures that needs to be watched for safety purpose
 */
int8_t safety_watch()
{
    uint8_t status = 0;

    for (uint8_t i = 0; i < DT_CHANNELS_NUMBER; i++)
    {
        if (channel_watch[i])
        {
            float32_t measure = shield.sensors.peek(static_cast<channel_t>(i));
            if(measure != -10000) channel_errors[i] = (measure > channel_threshold_max[i] || measure < channel_threshold_min[i]) ? true : false;
            if (channel_errors[i])
                status = -1;
        }
    }

    return status;
}

/**
 * @brief Safety actions taken when we detect an error
 */
void safety_action()
{
    twist.stopAll();
    if (channel_reaction == Open_Circuit)
    {
        _open_circuit();
    }
    else if (channel_reaction == Short_Circuit)
    {
        _short_circuit();
    }
}

/**
 * @brief Enables the safet API fault detection task
*/
void safety_enable_task()
{
    safety_enable = true;
}

/**
 * @brief Disables the safet API fault detection task
*/
void safety_disable_task()
{
    safety_enable = false;
}

/**
 * @brief Function that need to be put in the fast uninterruptible task.
 *        It monitors the measures from the ADC, and trigger safety warning.
 *        However, to avoid false trigerring from transient phenomenon we wait for
 *        a delay with safety_alert_counter
*/
int8_t safety_task()
{
    int8_t status = 0;

    if(safety_enable){
        status = safety_watch();

        if(status != 0)
        {
            safety_alert_counter++;
            if(safety_alert_counter > 4) safety_action();
            else status = 0;
        }
        else safety_alert_counter = 0;
    }

    return status;
}

/**
 * @brief Stores threshold value in the NVS
*/
int8_t safety_store_threshold_in_nvs(channel_t channel)
{
	// The data structure is as follows:
	// - 1 byte indicating the channel descriptor string size
	// - The channel descriptor string (should be max. 23 bytes in current version)
    // - 1 byte to store the channel number (in the order in the device tree)
	// - 4 byte to store the channel threshold min
	// - 4 byte to store the channel threshold max

	uint8_t* buffer = (uint8_t*)k_malloc(1 + 23 + 4 + 4);

    switch(channel)
    {
        case I1_LOW:
            snprintk((char*)(&buffer[1]), 23, "I1_LOW_THRESHOLD");
            break;

        case I2_LOW:
            snprintk((char*)(&buffer[1]), 23, "I2_LOW_THRESHOLD");
            break;

        case V1_LOW:
            snprintk((char*)(&buffer[1]), 23, "V1_LOW_THRESHOLD");
            break;

        case V2_LOW:
            snprintk((char*)(&buffer[1]), 23, "V2_LOW_THRESHOLD");
            break;

        case V_HIGH:
            snprintk((char*)(&buffer[1]), 23, "V_HIGH_THRESHOLD");
            break;

        case I_HIGH:
            snprintk((char*)(&buffer[1]), 23, "I_HIGH_THRESHOLD");
            break;

        default:
            snprintk((char*)(&buffer[1]), 23, "OTHER_THRESHOLD");
            break;
    }

	uint8_t string_len = strlen((char*)(&buffer[1]));

	buffer[0]                                  = string_len;
    buffer[string_len + 1]                     = channel;
	*((float32_t*)&buffer[string_len + 2])     = channel_threshold_min[channel];
	*((float32_t*)&buffer[string_len + 2 + 4]) = channel_threshold_max[channel];

	uint16_t channel_ID = MEASURE_THRESHOLD | (channel&0x0F);

	int ns = nvs_storage_store_data(channel_ID, buffer, 1 + string_len + 1 + 4 + 4);

	k_free(buffer);

	if (ns < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief Retrieves threshold value from the NVS
*/
int8_t  safety_retrieve_threshold_in_nvs(channel_t channel)
{
	// Checks that parameters currently stored in NVS are from the same version
	uint16_t current_stored_version = nvs_storage_get_version_in_nvs();
	if (current_stored_version == 0)
	{
		return -1;
	}
	else if (current_stored_version != nvs_storage_get_current_version())
	{
		return -2;
	}

	uint16_t channel_ID = MEASURE_THRESHOLD | (channel&0x0F);

	int buffer_size = 1 + 23 + 4 + 4;
	uint8_t* buffer = (uint8_t*)k_malloc(buffer_size);

	int read_size = nvs_storage_retrieve_data(channel_ID, buffer, buffer_size);

	int ret = 0;
	if (read_size > 0)
	{
		uint8_t string_len = buffer[0];

		// Check that all required values match
		if (channel != buffer[string_len + 1])
		{
			ret = -3;
		}
		else
		{
            channel_threshold_min[channel] = *((float32_t*)&buffer[string_len + 2]);
            channel_threshold_max[channel] = *((float32_t*)&buffer[string_len + 2 + 4]);
		}
	}
	else
	{
		ret = -4;
	}

	k_free(buffer);
	return ret;
}
