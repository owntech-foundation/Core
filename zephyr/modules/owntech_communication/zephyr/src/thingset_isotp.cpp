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

#include <canbus/isotp.h>
#include "thingset.h"
#include "can.h"

extern ThingSet ts;
extern uint16_t can_node_addr;
static const struct device* can_dev = DEVICE_DT_GET(DT_NODELABEL(can1));

#define RX_THREAD_STACK_SIZE 1024
#define RX_THREAD_PRIORITY 2

const struct isotp_fc_opts fc_opts = {
    .bs = 8,                // block size
    .stmin = 1              // minimum separation time = 100 ms
};

struct isotp_msg_id rx_addr = {
    .id_type = CAN_EXTENDED_IDENTIFIER,
    .use_ext_addr = 0,      // Normal ISO-TP addressing (using only CAN ID)
    .use_fixed_addr = 1,    // enable SAE J1939 compatible addressing
};

struct isotp_msg_id tx_addr = {
    .id_type = CAN_EXTENDED_IDENTIFIER,
    .use_ext_addr = 0,      // Normal ISO-TP addressing (using only CAN ID)
    .use_fixed_addr = 1,    // enable SAE J1939 compatible addressing
};

static struct isotp_recv_ctx recv_ctx;
static struct isotp_send_ctx send_ctx;

void send_complete_cb(int error_nr, void *arg)
{
    ARG_UNUSED(arg);
}

void can_isotp_thread()
{
    int ret, rem_len, resp_len;
    unsigned int req_len;
    struct net_buf *buf;
    static uint8_t rx_buffer[600];      // large enough to receive a 512k flash page for DFU
    static uint8_t tx_buffer[1000];

	enable_can();

    if (!device_is_ready(can_dev)) {
        return;
    }

    while (1) {
        /* re-assign address in every loop as it may have been changed via ThingSet */
        rx_addr.ext_id = TS_CAN_BASE_REQRESP | TS_CAN_PRIO_REQRESP | TS_CAN_TARGET_SET(can_node_addr);
        tx_addr.ext_id = TS_CAN_BASE_REQRESP | TS_CAN_PRIO_REQRESP | TS_CAN_SOURCE_SET(can_node_addr);

        ret = isotp_bind(&recv_ctx, can_dev, &rx_addr, &tx_addr, &fc_opts, K_FOREVER);
        if (ret != ISOTP_N_OK) {
            return;
        }

        req_len = 0;
        do {
            rem_len = isotp_recv_net(&recv_ctx, &buf, K_FOREVER);
            if (rem_len < 0) {
                break;
            }
            if (req_len + buf->len <= sizeof(rx_buffer)) {
                memcpy(&rx_buffer[req_len], buf->data, buf->len);
            }
            req_len += buf->len;
            net_buf_unref(buf);
        } while (rem_len);

        // we need to unbind the receive ctx so that control frames are received in the send ctx
        isotp_unbind(&recv_ctx);

        if (req_len > sizeof(rx_buffer)) {
            tx_buffer[0] = TS_STATUS_REQUEST_TOO_LARGE;
            resp_len = 1;
        }
        else if (req_len > 0 && rem_len == 0) {
            resp_len = ts.process(rx_buffer, req_len, tx_buffer, sizeof(tx_buffer));
        }
        else {
            tx_buffer[0] = TS_STATUS_INTERNAL_SERVER_ERR;
            resp_len = 1;
        }

        if (resp_len > 0) {
            ret = isotp_send(&send_ctx, can_dev, tx_buffer, resp_len,
                        &recv_ctx.tx_addr, &recv_ctx.rx_addr, send_complete_cb, NULL);
        }
    }
}

K_THREAD_DEFINE(can_isotp, RX_THREAD_STACK_SIZE, can_isotp_thread, NULL, NULL, NULL,
    RX_THREAD_PRIORITY, 0, 1500);
