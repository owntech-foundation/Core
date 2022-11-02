/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @date   2022
 * @author Martin Jäger <martin@libre.solar>
 * @author Jean Alinei <jean.alinei@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#define CONFIG_THINGSET_CAN 1

#if CONFIG_THINGSET_CAN

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/can.h>

#include "data_objects.h"
#include "thingset.h"
#include "can.h"


extern ThingSet ts;
extern uint16_t can_node_addr;
uint16_t broadcast_time = 10; //the time of the measurement variables broadcast (multiples of 100ms)
uint16_t control_time = 10;  //the time of the control variables broadcast/receive (multiples of 100ms)
static const struct device* can_dev = DEVICE_DT_GET(DT_NODELABEL(can1));

// below defines should go into the ThingSet library
#define TS_CAN_SOURCE_GET(id)           (((uint32_t)id & TS_CAN_SOURCE_MASK) >> TS_CAN_SOURCE_POS)
#define TS_CAN_DATA_ID_GET(id)          (((uint32_t)id & TS_CAN_DATA_ID_MASK) >> TS_CAN_DATA_ID_POS)

CAN_DEFINE_MSGQ(sub_msgq, 10);

const struct zcan_filter ctrl_filter = {
    .id = TS_CAN_BASE_CONTROL,
    .rtr = CAN_DATAFRAME,
    .id_type = CAN_EXTENDED_IDENTIFIER,
    .id_mask = TS_CAN_TYPE_MASK,
    .rtr_mask = 1
};

void can_pub_isr(uint32_t err_flags, void *arg)
{
	// Do nothing. Publication messages are fire and forget.
}

void can_pub_send(uint32_t can_id, uint8_t can_data[8], uint8_t data_len)
{
    if (!device_is_ready(can_dev))
	{
        return;
    }

    struct zcan_frame frame = {0};
    frame.id_type = CAN_EXTENDED_IDENTIFIER;
    frame.rtr     = CAN_DATAFRAME;
    frame.id      = can_id;
    memcpy(frame.data, can_data, 8);


    if (data_len >= 0)
	{
        frame.dlc = data_len;

        can_send(can_dev, &frame, K_MSEC(10), can_pub_isr, NULL);
    }
}

void send_ts_can_pub_message(uint16_t tag)
{
	int data_len = 0;
	int start_pos = 0;
	uint32_t can_id;
	uint8_t can_data[8];

	do
	{
		data_len = ts.bin_pub_can(start_pos, tag, can_node_addr, can_id, can_data);
		if (data_len >= 0)
		{
			can_pub_send(can_id, can_data, data_len);
		}
	} while (data_len >= 0);
}

void update_ts_data_nodes(struct zcan_frame rx_frame)
{
	// process message
	uint16_t data_id = TS_CAN_DATA_ID_GET(rx_frame.id);
	uint8_t sender_addr = TS_CAN_SOURCE_GET(rx_frame.id);

	// control message received?
	if (data_id > 0x8000 && sender_addr < can_node_addr) {
		uint8_t buf[5 + 8];     // ThingSet bin headers + CAN frame payload
		buf[0] = 0x1F;          // ThingSet pub message
		buf[1] = 0xA1;          // CBOR: map with 1 element
		buf[2] = 0x19;          // CBOR: uint16 follows
		buf[3] = data_id >> 8;
		buf[4] = data_id;
		memcpy(&buf[5], rx_frame.data, 8);

		// int status = ts.bin_sub(buf, 5 + rx_frame.dlc, TS_WRITE_MASK, SUBSET_CTRL);
        int status = ts.bin_import(buf + 1, 4 + rx_frame.dlc, TS_WRITE_MASK, SUBSET_CTRL);
		if (status == TS_STATUS_CHANGED) {

		}
	}
}


void can_pubsub_thread()
{
	enable_can();

    if (!device_is_ready(can_dev))
	{
        return;
    }

    int filter_id = can_attach_msgq(can_dev, &sub_msgq, &ctrl_filter);
    if (filter_id < 0) {
        return;
    }

    int64_t next_pub = k_uptime_get();
    uint32_t count = 0;

    while (1) {

        // task_wdt_feed(wdt_channel);

        if (count % broadcast_time == 0) {
            // normal objects: only every second
            dataObjectsUpdateMeasures();
            send_ts_can_pub_message(SUBSET_CAN);
        }

        if (count % control_time == 0) {
            // control objects: every 100 ms
            send_ts_can_pub_message(SUBSET_CTRL);
        }

		struct zcan_frame rx_frame;

        // wait for incoming messages until the next pub message has to be sent out
        while (k_msgq_get(&sub_msgq, &rx_frame, K_TIMEOUT_ABS_MS(next_pub)) != -EAGAIN)
		{
            update_ts_data_nodes(rx_frame);
        }

        next_pub += 100;       // 100 ms period (currently fixed)
        count++;
    }
}

K_THREAD_DEFINE(can_pubsub, 1024, can_pubsub_thread, NULL, NULL, NULL, 6, 0, 1000);

#endif /* CONFIG_THINGSET_CAN */
