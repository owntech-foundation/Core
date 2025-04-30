/*
 * Copyright (c) 2024-present LAAS-CNRS
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

/*
 * @date   2024
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


/* Header */
#include "CanCommunication.h"
#include "data_objects.h"
#include <thingset.h>
#include <thingset/can.h>
#include <thingset/sdk.h>

struct thingset_can *CanCommunication::ts_can_inst = thingset_can_get_inst();

/**
 * Extern variables coming from sdk.c
 **/

#ifdef CONFIG_THINGSET_SUBSET_LIVE_METRICS
/** Boolean to set or unset CAN broadcasting */
extern bool live_reporting_enable;
/** Integer used to set broadcasting period */
extern uint32_t live_reporting_period;
#endif
/**
 * Extern variable defined in this module
 */

#ifdef CONFIG_THINGSET_CAN_CONTROL_REPORTING

extern bool      start_stop;
extern float32_t reference_value;

bool CanCommunication::getCtrlEnable()
{
    return ts_can_inst->control_enable;
}

float32_t CanCommunication::getCtrlReference()
{
    return reference_value;
}

float32_t CanCommunication::getStartStopState()
{
    return start_stop;
}

uint16_t CanCommunication::getControlPeriod()
{
    return ts_can_inst->control_period;
}

void CanCommunication::setCtrlEnable(bool enable)
{
    ts_can_inst->control_enable = enable;
}

void CanCommunication::setCtrlReference(float32_t reference)
{
    reference_value = reference;
}

void CanCommunication::stopSlaveDevice()
{
    start_stop = 0;
}

void CanCommunication::startSlaveDevice()
{
    start_stop = 1;
}

void CanCommunication::setControlPeriod(uint16_t time_ms)
{
    ts_can_inst->control_period = time_ms;
}

#endif /* CONFIG_THINGSET_CAN_CONTROL_REPORTING */

uint16_t CanCommunication::getCanNodeAddr()
{
    return ts_can_inst->node_addr;
}

void CanCommunication::setCanNodeAddr(uint16_t addr)
{
    ts_can_inst->node_addr = addr;
}

#ifdef CONFIG_THINGSET_SUBSET_LIVE_METRICS

bool CanCommunication::getBroadcastEnable()
{
    return live_reporting_enable;
}

uint16_t CanCommunication::getBroadcastPeriod()
{
    return live_reporting_period;
}

void CanCommunication::setBroadcastEnable(bool enable)
{
    live_reporting_enable = enable;
}

void CanCommunication::setBroadcastPeriod(uint16_t time_s)
{
    live_reporting_period = time_s;
}

#endif /* CONFIG_THINGSET_SUBSET_LIVE_METRICS */