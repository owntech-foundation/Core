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
#include "DataAPI.h"

char manufacturer[] = "OwnTech";
char device_type[] = "Synchronous Buck";
char hardware_version[] = "0.9";
char firmware_version[] = "0.0.1";
char device_id[9]="Twist";

float32_t v1_low_value = 0; //store value of V1_low (app task)
float32_t v2_low_value = 0; //store value of V2_low (app task)
float32_t v_high_value = 0; //store value of Vhigh (app task)

float32_t i1_low_value = 0; //store value of i1_low (app task)
float32_t i2_low_value = 0; //store value of i2_low (app task)
float32_t i_high_value = 0; //store value of ihigh (app task)

float32_t temp_value = 0; //store value of temperature (app task)

float32_t reference_value = 0; //store value of reference (master-slave mode)

bool ctrl_enable = false; //store value of the enable (master-slave mode)

uint16_t can_node_addr = 0x60;

static void update_measurements(enum thingset_callback_reason cb_reason)
{
    if (cb_reason == THINGSET_CALLBACK_PRE_READ && data.started() == true)
    {
        v1_low_value = data.peek(V1_LOW);
        v2_low_value = data.peek(V2_LOW);
        v_high_value = data.peek(V_HIGH);
        i1_low_value = data.peek(I1_LOW);
        i2_low_value = data.peek(I2_LOW);
        i_high_value = data.peek(I_HIGH);
        // Do not update this value for now, as the measure is not enabled
        //temp_value = peekTemperature();
    }
}

/**
 * Thing Set Data Objects (see thingset.io for specification)
 */

THINGSET_ADD_GROUP(ID_ROOT, ID_DEVICE, "Device", THINGSET_NO_CALLBACK);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, 0x20, "cManufacturer", manufacturer, 0, THINGSET_ANY_R, 0);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, 0x21, "cType", device_type, 0, THINGSET_ANY_R, 0);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, 0x22, "cHardwareVersion", hardware_version, 0, THINGSET_ANY_R, 0);

THINGSET_ADD_ITEM_STRING(ID_DEVICE, 0x23, "cFirmwareVersion", firmware_version, 0, THINGSET_ANY_R, 0);

THINGSET_ADD_SUBSET(ID_ROOT, 0x0B, "mCAN", SUBSET_CAN, THINGSET_ANY_RW);

THINGSET_ADD_GROUP(ID_ROOT, ID_MEASUREMENTS, "Measurements", update_measurements);

THINGSET_ADD_ITEM_FLOAT(ID_MEASUREMENTS, 0x31, "rV1Low_V", &v1_low_value, 2, THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEASUREMENTS, 0x32, "rV2Low_V", &v2_low_value, 2, THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEASUREMENTS, 0x33, "rVHigh_V", &v_high_value, 2, THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEASUREMENTS, 0x34, "rI1Low_A", &i1_low_value, 2, THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEASUREMENTS, 0x35, "rI2Low_A", &i2_low_value, 2, THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEASUREMENTS, 0x36, "rIHigh_A", &i_high_value, 2, THINGSET_ANY_R, SUBSET_CAN);

THINGSET_ADD_ITEM_FLOAT(ID_MEASUREMENTS, 0x37, "rTemp_degC", &temp_value, 2, THINGSET_ANY_R, SUBSET_CAN);

/*
* Control parameters (IDs >= 0x8000)
*/
THINGSET_ADD_GROUP(ID_ROOT, ID_CTRL, "Control", THINGSET_NO_CALLBACK);

THINGSET_ADD_ITEM_FLOAT(ID_CTRL, 0x8001, "zCtrlReference", &reference_value, 1, THINGSET_ANY_RW,
    SUBSET_CTRL);

THINGSET_ADD_ITEM_BOOL(ID_CTRL, 0x8002, "zCtrlEnable", &ctrl_enable, THINGSET_ANY_RW, SUBSET_CTRL);
