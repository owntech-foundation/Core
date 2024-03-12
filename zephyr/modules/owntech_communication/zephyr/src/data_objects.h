/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @author Martin Jäger <martin@libre.solar>
 * @author Jean Alinei <jean.alinei@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */

#ifndef DATA_OBJECTS_H_
#define DATA_OBJECTS_H_

/** @file
 *
 * @brief Handling of ThingSet data objects
 */

#include <stdint.h>
#include <string.h>

#include <arm_math.h>

/*
 * Groups / first layer data object IDs
 */
#define ID_ROOT         0x00

#define ID_SUBSET_CAN 0x38

/* Device */
#define ID_DEVICE              0x4
#define ID_DEVICE_MANUFACTURER 0x40
#define ID_DEVICE_TYPE         0x41
#define ID_DEVICE_HW_VERSION   0x42
#define ID_DEVICE_FW_VERSION   0x43

/* Measurements */
#define ID_MEAS        0x5
#define ID_MEAS_V1_LOW 0x50
#define ID_MEAS_V2_LOW 0x51
#define ID_MEAS_V_HIGH 0x52
#define ID_MEAS_I1_LOW 0x53
#define ID_MEAS_I2_LOW 0x54
#define ID_MEAS_I_HIGH 0x55
#define ID_MEAS_TEMP   0x56

/* Control */
#define ID_CTRL           0x8000
#define ID_CTRL_REFERENCE 0x8001
#define ID_CTRL_ENABLE    0x8002

/*
 * Subset definitions for statements and publish/subscribe
 */
#define SUBSET_SER  (1U << 0)   // UART serial
#define SUBSET_CAN  (1U << 1)   // CAN bus
#define SUBSET_CTRL (1U << 3)   // control data sent and received via CAN

/*
 * Exposed variables
 */
extern bool      ctrl_enable;
extern uint16_t  can_node_addr;
extern float32_t reference_value;

#endif // DATA_OBJECTS_H_
