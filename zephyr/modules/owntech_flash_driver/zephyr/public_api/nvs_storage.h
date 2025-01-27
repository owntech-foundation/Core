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

int8_t nvs_storage_store_data(uint16_t data_id,
							  const void* data,
							  uint8_t data_size);

int8_t nvs_storage_retrieve_data(uint16_t data_id,
								 void* data_buffer,
								 uint8_t data_buffer_size);

int8_t nvs_storage_clear_all_stored_data();

uint16_t nvs_storage_get_current_version();
uint16_t nvs_storage_get_version_in_nvs();


#ifdef __cplusplus
}
#endif

#endif /* NVS_STORAGE_H_ */
