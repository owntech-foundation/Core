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

#include "data_objects.h"

#include <zephyr/kernel.h>
#include <soc.h>

#include <stdio.h>
#include <string.h>

#include "thingset.h"
#include "ShieldAPI.h"

#define HW_VER DT_PROP(DT_NODELABEL(pcb), shield_version)
#define HW_NAME DT_PROP(DT_NODELABEL(pcb), shield_name)

char manufacturer[] = CONFIG_USB_DEVICE_MANUFACTURER;
char device_type[] = HW_NAME;
char hardware_version[] = HW_VER;
char firmware_version[] = "1.0.0";

/* Store value of V1_low (app task) */
float32_t v1_low_value = 0;
/* Store value of V2_low (app task) */
float32_t v2_low_value = 0;
/* Store value of Vhigh (app task) */
float32_t v_high_value = 0;

/* Store value of i1_low (app task) */
float32_t i1_low_value = 0;
/* Store value of i2_low (app task) */
float32_t i2_low_value = 0;
/* Store value of ihigh (app task) */
float32_t i_high_value = 0;

/* Store value of temperature (app task) */
float32_t temp_value = 0;

/* Store value of reference (master-slave mode) */
float32_t reference_value = 0;

/* Store value of the enable (master-slave mode) */
bool ctrl_enable = false;

uint16_t can_node_addr = 0x60;

void update_measurements(enum thingset_callback_reason cb_reason)
{
    if (cb_reason == THINGSET_CALLBACK_PRE_READ)
    {
        v1_low_value = shield.sensors.peekLatestValue(V1_LOW);
        v2_low_value = shield.sensors.peekLatestValue(V2_LOW);
        v_high_value = shield.sensors.peekLatestValue(V_HIGH);
        i1_low_value = shield.sensors.peekLatestValue(I1_LOW);
        i2_low_value = shield.sensors.peekLatestValue(I2_LOW);
        i_high_value = shield.sensors.peekLatestValue(I_HIGH);
        /* Do not update this value for now, as the measure is not enabled */
        // temp_value = shield.sensors.peekLatestValue(TEMP_SENSOR_1);
    }
}

/**
 * Thing Set Data Objects (see thingset.io for specification)
 */

THINGSET_ADD_GROUP(ID_ROOT, ID_DEVICE, "Device", THINGSET_NO_CALLBACK);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, ID_DEVICE_MANUFACTURER, "cManufacturer",
                         manufacturer, 0, THINGSET_ANY_R, 0);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, ID_DEVICE_TYPE, "cType", device_type, 0,
                         THINGSET_ANY_R, 0);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, ID_DEVICE_HW_VERSION, "cHardwareVersion",
                         hardware_version, 0, THINGSET_ANY_R, 0);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, ID_DEVICE_FW_VERSION, "cFirmwareVersion",
                         firmware_version, 0, THINGSET_ANY_R, 0);

THINGSET_ADD_SUBSET(ID_ROOT, ID_SUBSET_CAN, "mCAN", SUBSET_CAN,
                    THINGSET_ANY_RW);

THINGSET_ADD_GROUP(ID_ROOT, ID_MEAS, "Measurements", update_measurements);

THINGSET_ADD_ITEM_FLOAT(ID_MEAS, ID_MEAS_V1_LOW, "rV1Low_V", &v1_low_value, 2,
                        THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEAS, ID_MEAS_V2_LOW, "rV2Low_V", &v2_low_value, 2,
                        THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEAS, ID_MEAS_V_HIGH, "rVHigh_V", &thingset, 2,
                        THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEAS, ID_MEAS_I1_LOW, "rI1Low_A", &i1_low_value, 2,
                        THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEAS, ID_MEAS_I2_LOW, "rI2Low_A", &i2_low_value, 2,
                        THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEAS, ID_MEAS_I_HIGH, "rIHigh_A", &i_high_value, 2,
                        THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEAS, ID_MEAS_TEMP, "rTemp_degC", &temp_value, 2,
                        THINGSET_ANY_R, SUBSET_CAN);

/**
 * Control parameters (IDs >= 0x8000)
 */

THINGSET_ADD_GROUP(ID_ROOT, ID_CTRL, "Control", THINGSET_NO_CALLBACK);

THINGSET_ADD_ITEM_FLOAT(ID_CTRL, ID_CTRL_REFERENCE, "zCtrlReference",
                        &reference_value, 1, THINGSET_ANY_RW, SUBSET_CTRL);

THINGSET_ADD_ITEM_BOOL(ID_CTRL, ID_CTRL_ENABLE, "zCtrlEnable", &ctrl_enable,
                       THINGSET_ANY_RW, SUBSET_CTRL);
