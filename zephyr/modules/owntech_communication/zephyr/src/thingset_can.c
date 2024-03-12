/*
 * Copyright (c) 2023 LAAS-CNRS
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
 * SPDX-License-Identifier: LGLPV2.1
 */

/**
 * @date   2024
 * @author Martin Jäger <martin@libre.solar>
 */

#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>

#include <thingset.h>
#include <thingset/can.h>
#include <thingset/sdk.h>

LOG_MODULE_REGISTER(ts_can, CONFIG_THINGSET_SDK_LOG_LEVEL_DEFAULT);

extern struct thingset_context ts;

void can_control_rx_handler(uint16_t data_id, const uint8_t *value, size_t value_len,
                                 uint8_t source_addr)
{
    // control data items use IDs >= 0x8000
    if (data_id >= 0x8000) {
        uint8_t buf[4 + CAN_MAX_DLEN]; // ThingSet bin headers + CAN frame payload
        buf[0] = 0xA1;      // CBOR: map with 1 element
        buf[1] = 0x19;      // CBOR: uint16 follows
        buf[2] = data_id >> 8;
        buf[3] = data_id;
        memcpy(&buf[4], value, value_len);

        LOG_DBG("received control msg with id 0x%X from addr 0x%X", data_id, source_addr);

        thingset_import_data(&ts, buf, 4 + value_len, THINGSET_WRITE_MASK, THINGSET_BIN_IDS_VALUES);
    }
}

static int can_control_init()
{
    thingset_can_set_item_rx_callback(can_control_rx_handler);

    return 0;
}

SYS_INIT(can_control_init, APPLICATION, THINGSET_INIT_PRIORITY_DEFAULT);
