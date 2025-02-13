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
#include "thingset.h"

/*
 * Groups / first layer data object IDs
 */
#define ID_ROOT         0x00
#define ID_SUBSET_CAN   0x38

/* Device */
#define ID_DEVICE              0x4
#define ID_DEVICE_MANUFACTURER 0x40
#define ID_DEVICE_TYPE         0x41
#define ID_DEVICE_HW_VERSION   0x42
#define ID_DEVICE_FW_VERSION   0x43

/* Control */
#define ID_CTRL           0x8000
#define ID_CTRL_REFERENCE 0x8001
#define ID_CTRL_ENABLE    0x8002

#define HW_VER DT_PROP(DT_NODELABEL(pcb), shield_version)
#define HW_NAME DT_PROP(DT_NODELABEL(pcb), shield_name)

char manufacturer[] = CONFIG_USB_DEVICE_MANUFACTURER;
char device_type[] = HW_NAME;
char hardware_version[] = HW_VER;
char firmware_version[] = "1.0.0";

/* Store value of reference (master-slave mode) */
float32_t reference_value = 0;
bool      start_stop = false;

/*
 * Subset definitions for statements and publish/subscribe
 */

/* UART serial */
#define SUBSET_SER  (1U << 0)
/* CAN bus */
#define SUBSET_CAN  (1U << 1)
/* Control data sent and received via CAN */
#define SUBSET_CTRL (1U << 3)

/**
 * Thingset Data Objects (see thingset.io for specification)
 */

THINGSET_ADD_GROUP(ID_ROOT, 0x4, "Device", THINGSET_NO_CALLBACK);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, 0x40, "cManufacturer",
                         manufacturer, 0, THINGSET_ANY_R, 0);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, 0x41, "cType", device_type, 0,
                         THINGSET_ANY_R, 0);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, 0x42, "cHardwareVersion",
                         hardware_version, 0, THINGSET_ANY_R, 0);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, 0x43, "cFirmwareVersion",
                         firmware_version, 0, THINGSET_ANY_R, 0);

THINGSET_ADD_SUBSET(ID_ROOT, 0x38, "mCAN", SUBSET_CAN,
                    THINGSET_ANY_RW);

/**
 * Control parameters (IDs >= 0x8000)
 */

THINGSET_ADD_GROUP(ID_ROOT, ID_CTRL, "Control", THINGSET_NO_CALLBACK);

THINGSET_ADD_ITEM_FLOAT(ID_CTRL, ID_CTRL_REFERENCE, "zCtrlReference",
                        &reference_value, 1, THINGSET_ANY_RW, SUBSET_CTRL);

THINGSET_ADD_ITEM_BOOL(ID_CTRL, ID_CTRL_ENABLE, "zStartStop", &start_stop,
                       THINGSET_ANY_RW, SUBSET_CTRL);

#endif /* DATA_OBJECTS_H_ */
