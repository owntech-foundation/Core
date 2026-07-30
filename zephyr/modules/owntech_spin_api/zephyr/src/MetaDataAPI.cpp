/*
 * Copyright (c) 2026-present LAAS-CNRS
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
 * @date   2026
 *
 * @author Luiz Villa <luiz.villa@laas.fr>
 */


/* Current class header */
#include "MetaDataAPI.h"

/* Low-level flash storage API */
#include "nvs_storage.h"


/**
 * Sub-addressing of the individual metadata fields within the
 * BOARD_METADATA NVS category, following the same bit-packing convention
 * used for ADC_CALIBRATION in data_conversion.cpp: the category occupies
 * the upper byte of the 16-bit NVS key, the field id the lower byte.
 */
enum : uint16_t
{
	META_SPIN_SERIAL     = 0x00,
	META_SHIELD_SERIAL   = 0x01,
	META_SPIN_VERSION    = 0x02,
	META_SHIELD_VERSION  = 0x03,
	META_SHIELD_PASSWORD = 0x04,
	META_SPIN_PASSWORD   = 0x05,
	META_EXTRA_0         = 0x06,
	/* META_EXTRA_0 .. META_EXTRA_0 + METADATA_EXTRA_COUNT - 1 (0x06-0x0A)
	 * are reserved for the generic extra slots. */
};

static const uint16_t VERSION_FIELD_LEN = 3; /* major, minor, rev */

int8_t MetaDataAPI::setSpinSerialNumber(const char* serial, uint8_t serial_size)
{
	if (serial_size != SPIN_SERIAL_LEN)
	{
		return -2;
	}

	int ret = nvs_storage_store_data(BOARD_METADATA | META_SPIN_SERIAL,
	                                  serial, SPIN_SERIAL_LEN);
	return (ret < 0) ? -1 : 0;
}

int8_t MetaDataAPI::getSpinSerialNumber(char* buffer, uint8_t buffer_size)
{
	if (buffer_size < SPIN_SERIAL_LEN)
	{
		return -2;
	}

	int ret = nvs_storage_retrieve_data(BOARD_METADATA | META_SPIN_SERIAL,
	                                     buffer, buffer_size);
	return (ret < 0) ? -1 : ret;
}

int8_t MetaDataAPI::setShieldSerialNumber(const char* serial, uint8_t serial_size)
{
	if (serial_size != SHIELD_SERIAL_LEN)
	{
		return -2;
	}

	int ret = nvs_storage_store_data(BOARD_METADATA | META_SHIELD_SERIAL,
	                                  serial, SHIELD_SERIAL_LEN);
	return (ret < 0) ? -1 : 0;
}

int8_t MetaDataAPI::getShieldSerialNumber(char* buffer, uint8_t buffer_size)
{
	if (buffer_size < SHIELD_SERIAL_LEN)
	{
		return -2;
	}

	int ret = nvs_storage_retrieve_data(BOARD_METADATA | META_SHIELD_SERIAL,
	                                     buffer, buffer_size);
	return (ret < 0) ? -1 : ret;
}

int8_t MetaDataAPI::setSpinVersion(uint8_t major, uint8_t minor, uint8_t rev)
{
	uint8_t version[VERSION_FIELD_LEN] = {major, minor, rev};

	int ret = nvs_storage_store_data(BOARD_METADATA | META_SPIN_VERSION,
	                                  version, VERSION_FIELD_LEN);
	return (ret < 0) ? -1 : 0;
}

int8_t MetaDataAPI::getSpinVersion(uint8_t* major, uint8_t* minor, uint8_t* rev)
{
	uint8_t version[VERSION_FIELD_LEN];

	int ret = nvs_storage_retrieve_data(BOARD_METADATA | META_SPIN_VERSION,
	                                     version, VERSION_FIELD_LEN);
	if (ret < 0)
	{
		return -1;
	}

	*major = version[0];
	*minor = version[1];
	*rev   = version[2];
	return 0;
}

int8_t MetaDataAPI::setShieldVersion(uint8_t major, uint8_t minor, uint8_t rev)
{
	uint8_t version[VERSION_FIELD_LEN] = {major, minor, rev};

	int ret = nvs_storage_store_data(BOARD_METADATA | META_SHIELD_VERSION,
	                                  version, VERSION_FIELD_LEN);
	return (ret < 0) ? -1 : 0;
}

int8_t MetaDataAPI::getShieldVersion(uint8_t* major, uint8_t* minor, uint8_t* rev)
{
	uint8_t version[VERSION_FIELD_LEN];

	int ret = nvs_storage_retrieve_data(BOARD_METADATA | META_SHIELD_VERSION,
	                                     version, VERSION_FIELD_LEN);
	if (ret < 0)
	{
		return -1;
	}

	*major = version[0];
	*minor = version[1];
	*rev   = version[2];
	return 0;
}

int8_t MetaDataAPI::setSpinPassword(const char* password, uint8_t password_size)
{
	if (password_size != SPIN_PASSWORD_LEN)
	{
		return -2;
	}

	int ret = nvs_storage_store_data(BOARD_METADATA | META_SPIN_PASSWORD,
	                                  password, SPIN_PASSWORD_LEN);
	return (ret < 0) ? -1 : 0;
}

int8_t MetaDataAPI::getSpinPassword(char* buffer, uint8_t buffer_size)
{
	if (buffer_size < SPIN_PASSWORD_LEN)
	{
		return -2;
	}

	int ret = nvs_storage_retrieve_data(BOARD_METADATA | META_SPIN_PASSWORD,
	                                     buffer, buffer_size);
	return (ret < 0) ? -1 : ret;
}

int8_t MetaDataAPI::setShieldPassword(const char* password, uint8_t password_size)
{
	if (password_size != SHIELD_PASSWORD_LEN)
	{
		return -2;
	}

	int ret = nvs_storage_store_data(BOARD_METADATA | META_SHIELD_PASSWORD,
	                                  password, SHIELD_PASSWORD_LEN);
	return (ret < 0) ? -1 : 0;
}

int8_t MetaDataAPI::getShieldPassword(char* buffer, uint8_t buffer_size)
{
	if (buffer_size < SHIELD_PASSWORD_LEN)
	{
		return -2;
	}

	int ret = nvs_storage_retrieve_data(BOARD_METADATA | META_SHIELD_PASSWORD,
	                                     buffer, buffer_size);
	return (ret < 0) ? -1 : ret;
}

int8_t MetaDataAPI::setExtraData(uint8_t index, const uint8_t* data, uint8_t data_size)
{
	if (index >= METADATA_EXTRA_COUNT)
	{
		return -3;
	}

	if (data_size > METADATA_EXTRA_MAX_LEN)
	{
		return -2;
	}

	uint16_t field_id = BOARD_METADATA | (META_EXTRA_0 + index);
	int ret = nvs_storage_store_data(field_id, data, data_size);
	return (ret < 0) ? -1 : 0;
}

int8_t MetaDataAPI::getExtraData(uint8_t index, uint8_t* buffer, uint8_t buffer_size)
{
	if (index >= METADATA_EXTRA_COUNT)
	{
		return -3;
	}

	if (buffer_size < METADATA_EXTRA_MAX_LEN)
	{
		return -2;
	}

	uint16_t field_id = BOARD_METADATA | (META_EXTRA_0 + index);
	int ret = nvs_storage_retrieve_data(field_id, buffer, buffer_size);
	return (ret < 0) ? -1 : ret;
}

int8_t MetaDataAPI::clearAllMetaData()
{
	static const uint16_t metadata_field_ids[] = {
		META_SPIN_SERIAL,
		META_SHIELD_SERIAL,
		META_SPIN_VERSION,
		META_SHIELD_VERSION,
		META_SHIELD_PASSWORD,
		META_EXTRA_0 + 0,
		META_EXTRA_0 + 1,
		META_EXTRA_0 + 2,
		META_EXTRA_0 + 3,
		META_EXTRA_0 + 4,
		META_SPIN_PASSWORD,
	};

	/* Writing with a size of 0 deletes the entry (see Zephyr's
	 * nvs_write() documentation), without touching any other module's
	 * data in the shared NVS partition. */
	uint8_t dummy = 0;
	int8_t result = 0;
	for (uint16_t field_id : metadata_field_ids)
	{
		int ret = nvs_storage_store_data(BOARD_METADATA | field_id, &dummy, 0);
		if (ret < 0)
		{
			result = -1;
		}
	}

	return result;
}
