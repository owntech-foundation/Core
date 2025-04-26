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
 * @author Martin Jäger <martin@libre.solar>
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>

#include <thingset.h>
#include <thingset/can.h>
#include <thingset/sdk.h>

LOG_MODULE_REGISTER(ts_can, CONFIG_THINGSET_SDK_LOG_LEVEL);

extern struct thingset_context ts;

/* Structure to hold the data to be processed by the workqueue */
struct can_control_work_data {
    struct k_work work;
    /* ThingSet bin headers + CAN frame payload */
    uint8_t buf[4 + CAN_MAX_DLEN];
    size_t buf_len;
};

static struct can_control_work_data can_work_data;

/* Work handler function to be executed in workqueue context */
static void can_control_work_handler(struct k_work *item)
{
    struct can_control_work_data *data =
            CONTAINER_OF(item, struct can_control_work_data, work);

    LOG_HEXDUMP_DBG(data->buf, data->buf_len, "Thingset frame:");

    thingset_import_data(&ts,
                         data->buf,
                         data->buf_len,
                         THINGSET_WRITE_MASK,
                         THINGSET_BIN_IDS_VALUES);
}

void can_control_rx_handler(uint16_t data_id,
                            const uint8_t *value,
                            size_t value_len,
                            uint8_t source_addr)
{
    /* Control data items use IDs >= 0x8000 */
    if (data_id >= 0x8000) {
        /* CBOR: map with 1 element */
        can_work_data.buf[0] = 0xA1;
        /* CBOR: uint16 follows (object ID is 2 bytes) */
        can_work_data.buf[1] = 0x19;
        /* High byte of data ID */
        can_work_data.buf[2] = data_id >> 8;
        /* Low byte of data ID */
        can_work_data.buf[3] = data_id;

        memcpy(&can_work_data.buf[4], value, value_len);

        can_work_data.buf_len = 4 + value_len;

        LOG_DBG("received control msg with id 0x%X from addr 0x%X",
                data_id,
                source_addr);

        /* Submit the work item to the system workqueue */
        k_work_submit(&can_work_data.work);
    }
}

static int can_control_init()
{
    k_work_init(&can_work_data.work, can_control_work_handler);
    thingset_can_set_item_rx_callback(can_control_rx_handler);
    return 0;
}

SYS_INIT(can_control_init, APPLICATION, THINGSET_INIT_PRIORITY_DEFAULT);
