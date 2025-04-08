/*
 * Copyright (c) 2023-present LAAS-CNRS
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

/**
 * @date   2024
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Jean Alinei <jean.alinei@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef NVS_STORAGE_H_
#define NVS_STORAGE_H_


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Types definition
 */

/**
 *  NVS categories must be on the upper half
 *  of the 2-bytes value, hence end with 00
 */

typedef enum
{
	VERSION          = 0x0100,
	ADC_CALIBRATION  = 0x0200,
	MEASURE_THRESHOLD = 0x0300,
}nvs_category_t;

/**
 *  API
 */

 /**
 * @brief Store a data item in non-volatile storage (NVS).
 *
 * Stores a block of data under a given identifier (data_id) in flash memory.
 * If the data already exists, it is overwritten. Useful for persisting configuration.
 *
 * @param data_id     Identifier for the data item.
 * @param data        Pointer to the data to be stored.
 * @param data_size   Size of the data in bytes.
 *
 * @return 0 on success, negative value on error.
 */
int8_t nvs_storage_store_data(uint16_t data_id,
							  const void* data,
							  uint8_t data_size);

/**
 * @brief Retrieve a data item from non-volatile storage (NVS).
 *
 * Reads the stored data associated with the given identifier (data_id)
 * and copies it into the provided buffer.
 *
 * @param data_id           Identifier for the stored data.
 * @param data_buffer       Pointer to the buffer where data will be copied.
 * @param data_buffer_size  Size of the buffer in bytes.
 *
 * @return Number of bytes read on success, negative value on error.
 */							  
int8_t nvs_storage_retrieve_data(uint16_t data_id,
								 void* data_buffer,
								 uint8_t data_buffer_size);

/**
 * @brief Clear all data stored in the NVS partition.
 *
 * Erases all key-value entries in the configured NVS area. Use with caution.
 *
 * @return 0 on success, negative value on error.
 */								 
int8_t nvs_storage_clear_all_stored_data();

/**
 * @brief Get the current in-code version of the NVS layout.
 *
 * This version corresponds to the structure of data expected by the firmware.
 * Used to detect incompatibility between NVS layout and firmware logic.
 *
 * @return Current version defined in code.
 */
uint16_t nvs_storage_get_current_version();

/**
 * @brief Get the version stored in the NVS flash memory.
 *
 * Used to compare with the in-code version to validate compatibility.
 *
 * @return Stored version value from flash.
 */
uint16_t nvs_storage_get_version_in_nvs();


#ifdef __cplusplus
}
#endif

#endif /* NVS_STORAGE_H_ */
