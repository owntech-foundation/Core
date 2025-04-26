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

/*
 * @date   2023
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Jean Alinei <jean.alinei@laas.fr>
 */


/**
 *  Includes
 */

/* Zephyr */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/fs/nvs.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>

/* CMSIS */
#include <arm_math.h>

/* Current file header */
#include "nvs_storage.h"


/* Constants and variables */
static const uint16_t current_storage_version = 0x0001; 
static uint16_t storage_version_in_nvs = 0;
static bool initialized = false;

/* Device-tree related macros */
#define NVS_PARTITION storage_partition
#define STORAGE_NODE  DT_NODE_BY_FIXED_PARTITION_LABEL(NVS_PARTITION)

/** @brief Flash memory file system */
static struct nvs_fs fs =
{
	.offset       = FIXED_PARTITION_OFFSET(NVS_PARTITION),
	.flash_device = FIXED_PARTITION_DEVICE(NVS_PARTITION)
};


/**
 * @brief PRIVATE FUNCTION - Store the current NVS (Non-Volatile Storage) 
 *        version if needed.
 *
 * This function ensures that the storage version recorded in NVS is consistent
 * with the current API's storage version.
 *
 * - If the stored version matches the current version, nothing is done.
 *
 * - If no version exists in NVS (first use), it writes the current version into
 *   NVS.
 *
 * - If a different version exists, it treats the mismatch as an error and
 *   signals that manual clearing is required.
 *
 * @return `0` if version is already correct or successfully written, 
 *         `-1` on failure.
 *
 */
static int8_t _nvs_storage_store_version()
{
	if (storage_version_in_nvs == current_storage_version)
	{
		/* Ok, nothing to do */
		return 0;
	}
	else if (storage_version_in_nvs == 0)
	{
		/* No version in NVS: this is the first use of NVS,
		 * store current version number. */
		int rc = nvs_write(&fs, VERSION, &current_storage_version, 2);

		if (rc == 2)
		{
			storage_version_in_nvs = current_storage_version;
			return 0;
		}

		return -1;
	}
	else
	{
		/* There is already a version number in NVS,
		 * but it differs from current API version.
		 * This is currently treated as an error and requires
		 * to explicitly clear NVS. */
		return -1;
	}
}

/**
 * @brief PRIVATE FUNCTION - Initialize the NVS (Non-Volatile Storage) subsystem.
 *
 * This function sets up the flash storage environment used to store ADC parameters.
 *
 * - Verifies if initialization has already been completed.
 *
 * - Checks if the flash device is ready for operations.
 *
 * - Retrieves flash page information to determine sector size and count.
 *
 * - Mounts the NVS file system.
 *
 * - Checks and validates the stored version against the current module version.
 *
 * @return `0` if initialization succeeds,
 *         `-1` if initialization fails,
 *         `-2` if a version mismatch is detected.
 *
 */
static int8_t _nvs_storage_init()
{
	if (initialized == true)
		return 0;

	if (!device_is_ready(fs.flash_device))
	{
		printk("Flash device %s is not ready\n", fs.flash_device->name);
		return -1;
	}

	/**
	 *  Handle non volatile memory used to store ADC parameters
	 *  Flash partition reserved to user data storage is 4kB long
	 *  We have to mount the file system then we extract one
	 *  memory page that is 2kB long that contains the data
	 *  and read the relevant addresses to init ADC parameters.
	 */

	struct flash_pages_info info;
	int rc = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
	if (rc != 0)
	{
		printk("Unable to get page info\n");
		return -1;
	}
	fs.sector_size = info.size;
	fs.sector_count = 2U;

	rc = nvs_mount(&fs);
	if (rc != 0)
	{
		printk("Flash Init failed\n");
		return -1;
	}

	/* Init OK */
	initialized = true;

	/* Check version in storage */
	rc = nvs_storage_retrieve_data(VERSION, &storage_version_in_nvs, 2);

	if (rc < 0)
	{
		/* No version in NVS: this is the first use of NVS. */
		storage_version_in_nvs = 0;
	}
	else if (storage_version_in_nvs != current_storage_version)
	{
		printk("WARNING: stored version in NVS is different from \
				current module version! \
				Stored data may not have the expected format.\n");

		/* -2 indicates that the current version stored in NVS
		 * is different from current code version. */
		return -2;
	}

	return 0;
}


/* Public functions */

int8_t nvs_storage_store_data(uint16_t data_id,
							  const void* data,
							  uint8_t data_size)
{
	if (initialized == false)
	{
		int8_t error = _nvs_storage_init();
		if (error != 0) return error;
	}

	int rc = _nvs_storage_store_version();
	if (rc != 0)
	{
		return rc;
	}

	rc = nvs_write(&fs, data_id, data, data_size);

	return rc;
}

int8_t nvs_storage_retrieve_data(uint16_t data_id,
								 void* data_buffer,
								 uint8_t data_buffer_size)
{
	if (initialized == false)
	{
		int8_t error = _nvs_storage_init();
		if (error != 0) return error;
	}

	int rc = nvs_read(&fs, data_id, data_buffer, 1);

	if (rc > 1) /* There is more than 1 byte of data */
	{
		if (rc > data_buffer_size)
		{
			/* Indicate that provided buffer is too small to retrieve data */
			return -1;
		}

		rc = nvs_read(&fs, data_id, data_buffer, rc);
	}

	return rc;
}

int8_t nvs_storage_clear_all_stored_data()
{
	if (initialized == false)
	{
		int8_t error = _nvs_storage_init();
		if (error != 0) return 0;
	}

	return nvs_clear(&fs);
}

uint16_t nvs_storage_get_current_version()
{
	if (initialized == false)
	{
		int8_t error = _nvs_storage_init();
		if (error != 0) return 0;
	}

	return current_storage_version;
}

uint16_t nvs_storage_get_version_in_nvs()
{
	if (initialized == false)
	{
		int8_t error = _nvs_storage_init();
		if (error != 0) return 0;
	}

	return storage_version_in_nvs;
}
