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

#include <zephyr.h>
#include <soc.h>

#include <stdio.h>
#include <string.h>

#include "thingset.h"
#include "DataAcquisition.h"


// can be used to configure custom data objects in separate file instead
// (e.g. data_nodes_custom.cpp)

const char manufacturer[] = "OwnTech";
const char device_type[] = "Synchronous Buck";
const char hardware_version[] = "0.9";
const char firmware_version[] = "0.0.1";
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



void dataObjectsUpdateMeasures()
{
    if (dataAcquisition.started() == true)
    {
        v1_low_value = dataAcquisition.peekV1Low();
        v2_low_value = dataAcquisition.peekV2Low();
        v_high_value = dataAcquisition.peekVHigh();
        i1_low_value = dataAcquisition.peekI1Low();
        i2_low_value = dataAcquisition.peekI2Low();
        i_high_value = dataAcquisition.peekIHigh();
        // Do not update this value for now, as the measure is not enabled
        //temp_value = peekTemperature();
    }
}

/**
 * Thing Set Data Objects (see thingset.io for specification)
 */
/* clang-format off */
static ThingSetDataObject data_objects[] = {

    ///////////////////////////////////////////////////////////////////////////////////////////////
    TS_GROUP(ID_DEVICE, "Device", TS_NO_CALLBACK, ID_ROOT),

        /*{
            "title": {
                "en": "Manufacturer"
            }
        }*/
        TS_ITEM_STRING(0x20, "cManufacturer", manufacturer, 0,
            ID_DEVICE, TS_ANY_R, 0),

        /*{
            "title": {
                "en": "Device Type"
            }
        }*/
        TS_ITEM_STRING(0x21, "cType", device_type, 0,
            ID_DEVICE, TS_ANY_R, 0),

        /*{
            "title": {
                "en": "Hardware Version"
            }
        }*/
        TS_ITEM_STRING(0x22, "cHardwareVersion", hardware_version, 0,
            ID_DEVICE, TS_ANY_R, 0),

        /*{
            "title": {
                "en": "Firmware Version"
            }
        }*/
        TS_ITEM_STRING(0x23, "cFirmwareVersion", firmware_version, 0,
            ID_DEVICE, TS_ANY_R, 0),

        TS_SUBSET(0x0B, "mCAN", SUBSET_CAN, ID_ROOT, TS_ANY_RW),

    ///////////////////////////////////////////////////////////////////////////////////////////////

    TS_GROUP(ID_MEASUREMENTS, "Measurements", TS_NO_CALLBACK, ID_ROOT),

        /*{
            "title": {
                "en": "Low Side 1 Voltage Measurement"
            }
        }*/
        TS_ITEM_FLOAT(0x31, "rMeas_V1_low_V", &v1_low_value, 2,
            ID_MEASUREMENTS, TS_ANY_R, SUBSET_CAN),

        /*{
            "title": {
                "en": "Low Side 2 Voltage Measurement"
            }
        }*/
        TS_ITEM_FLOAT(0x32, "rMeas_V2_low_V", &v2_low_value, 2,
            ID_MEASUREMENTS, TS_ANY_R, SUBSET_CAN),

        /*{
            "title": {
                "en": "High Side Voltage Measurement"
            }
        }*/
        TS_ITEM_FLOAT(0x33, "rMeas_V_high_V", &v_high_value, 2,
            ID_MEASUREMENTS, TS_ANY_R, SUBSET_CAN),

        /*{
            "title": {
                "en": "Low Side 1 Current Measurement"
            }
        }*/
        TS_ITEM_FLOAT(0x34, "rMeas_I1_low_A", &i1_low_value, 2,
            ID_MEASUREMENTS, TS_ANY_R, SUBSET_CAN),

        /*{
            "title": {
                "en": "Low Side 2 Current Measurement"
            }
        }*/
        TS_ITEM_FLOAT(0x35, "rMeas_I2_low_A", &i2_low_value, 2,
            ID_MEASUREMENTS, TS_ANY_R, SUBSET_CAN),

        /*{
            "title": {
                "en": "High Side Current Measurement"
            }
        }*/
        TS_ITEM_FLOAT(0x36, "rMeas_I_high_A", &i_high_value, 2,
            ID_MEASUREMENTS, TS_ANY_R, SUBSET_CAN),

        /*{
            "title": {
                "en": "Temperature Measurement"
            }
        }*/
        TS_ITEM_FLOAT(0x37, "rMeas_temp_degC", &temp_value, 2,
            ID_MEASUREMENTS, TS_ANY_R, SUBSET_CAN),



    ///////////////////////////////////////////////////////////////////////////////////////////////

    /*
     * Control parameters (IDs >= 0x8000)
     *
     */
    TS_GROUP(ID_CTRL, "Control", TS_NO_CALLBACK, ID_ROOT),

        /*{
            "title": {
                "en": "Current control target"
            }
        }*/
        TS_ITEM_FLOAT(0x8001, "zCtrl_Reference", &reference_value, 1,
            ID_CTRL, TS_ANY_RW, SUBSET_CTRL),

        /*{
            "title": {
                "en": "Enable/Disable signal"
            }
        }*/
        TS_ITEM_BOOL(0x8002, "zCtrl_Enable", &ctrl_enable,
            ID_CTRL, TS_ANY_RW, SUBSET_CTRL),
};
/* clang-format on */

ThingSet ts(data_objects, sizeof(data_objects) / sizeof(ThingSetDataObject));


