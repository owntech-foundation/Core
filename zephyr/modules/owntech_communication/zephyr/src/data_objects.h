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
#define ID_DEVICE       0x01
#define ID_MEASUREMENTS 0x08
#define ID_PUB          0x100
#define ID_CTRL         0x8000

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

/*
 * Modifiers
 */

void dataObjectsUpdateMeasures();


#endif // DATA_OBJECTS_H_
