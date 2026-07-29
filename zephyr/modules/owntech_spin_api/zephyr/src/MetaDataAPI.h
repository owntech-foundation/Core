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


#ifndef METADATAAPI_H_
#define METADATAAPI_H_


/* Stdlib */
#include <stdint.h>

/**
 *  Constants definitions
 */

/* Fixed length, in bytes, of a serial number field (13 raw ASCII
 * characters, not null-terminated). */
static const uint8_t SPIN_SERIAL_LEN = 13;
static const uint8_t SHIELD_SERIAL_LEN = 13;

/* Fixed length, in bytes, of the shield password field (3 raw ASCII
 * characters, not null-terminated). */
static const uint8_t SHIELD_PASSWORD_LEN = 3;

/* Number of generic extra metadata slots, and max size of each. */
static const uint8_t METADATA_EXTRA_COUNT = 5;
static const uint8_t METADATA_EXTRA_MAX_LEN = 16;

/**
 *  Class definition
 */

/**
 * @brief Persist board/shield identity data (serial numbers, versions,
 *        shield password, and generic extra slots) to flash.
 *
 * @note This is NOT secure storage: data is written in plain form to the
 *       NVS partition, with no secure element or read-protection involved.
 *       The shield password field is only meant to gate casual shield
 *       mismatches, not to protect a real secret.
 */
class MetaDataAPI
{
public:

	/**
	 * @brief Store the Spin board serial number in persistent memory.
	 *
	 * @param[in] serial      Pointer to a buffer of exactly
	 *                        `SPIN_SERIAL_LEN` (13) raw ASCII bytes. The
	 *                        buffer does not need to be null-terminated,
	 *                        and no null terminator is written; if fewer
	 *                        than 13 meaningful characters are needed, the
	 *                        caller must pad the buffer itself (e.g. with
	 *                        spaces or zeros) up to 13 bytes.
	 * @param[in] serial_size Size of `serial` in bytes, must be exactly
	 *                        `SPIN_SERIAL_LEN` (13).
	 *
	 * @return `0` if the serial number was correctly stored, negative
	 *         value on error:
	 *
	 * - `-1`: underlying storage error,
	 *
	 * - `-2`: `serial_size` is not exactly `SPIN_SERIAL_LEN`.
	 */
	int8_t setSpinSerialNumber(const char* serial, uint8_t serial_size);

	/**
	 * @brief Retrieve the Spin board serial number from persistent memory.
	 *
	 * @param[in]  buffer      Buffer to receive the 13 raw ASCII bytes.
	 * @param[in]  buffer_size Size of `buffer`, must be at least
	 *                         `SPIN_SERIAL_LEN` (13).
	 *
	 * @return Number of bytes read (13) on success, negative value on error:
	 *
	 * - `-1`: underlying storage error (nothing stored yet, corrupted
	 *         data, or version mismatch),
	 *
	 * - `-2`: provided buffer is smaller than `SPIN_SERIAL_LEN`.
	 */
	int8_t getSpinSerialNumber(char* buffer, uint8_t buffer_size);

	/**
	 * @brief Store the shield serial number in persistent memory.
	 *
	 * @param[in] serial      Pointer to a buffer of exactly
	 *                        `SHIELD_SERIAL_LEN` (13) raw ASCII bytes.
	 *                        Same padding rules as `setSpinSerialNumber()`.
	 * @param[in] serial_size Size of `serial` in bytes, must be exactly
	 *                        `SHIELD_SERIAL_LEN` (13).
	 *
	 * @return `0` if the serial number was correctly stored, negative
	 *         value on error:
	 *
	 * - `-1`: underlying storage error,
	 *
	 * - `-2`: `serial_size` is not exactly `SHIELD_SERIAL_LEN`.
	 */
	int8_t setShieldSerialNumber(const char* serial, uint8_t serial_size);

	/**
	 * @brief Retrieve the shield serial number from persistent memory.
	 *
	 * @param[in]  buffer      Buffer to receive the 13 raw ASCII bytes.
	 * @param[in]  buffer_size Size of `buffer`, must be at least
	 *                         `SHIELD_SERIAL_LEN` (13).
	 *
	 * @return Number of bytes read (13) on success, negative value on error:
	 *
	 * - `-1`: underlying storage error,
	 *
	 * - `-2`: provided buffer is smaller than `SHIELD_SERIAL_LEN`.
	 */
	int8_t getShieldSerialNumber(char* buffer, uint8_t buffer_size);

	/**
	 * @brief Store the Spin board hardware version in persistent memory.
	 *
	 * @param[in] major Major version number.
	 * @param[in] minor Minor version number.
	 * @param[in] rev   Revision number.
	 *
	 * @return `0` if the version was correctly stored,
	 *         `-1` if there was an underlying storage error.
	 */
	int8_t setSpinVersion(uint8_t major, uint8_t minor, uint8_t rev);

	/**
	 * @brief Retrieve the Spin board hardware version from persistent
	 *        memory.
	 *
	 * @param[out] major Pointer to receive the major version number.
	 * @param[out] minor Pointer to receive the minor version number.
	 * @param[out] rev   Pointer to receive the revision number.
	 *
	 * @return `0` if the version was correctly retrieved,
	 *         `-1` if there was an underlying storage error (nothing
	 *         stored yet, corrupted data, or version mismatch).
	 */
	int8_t getSpinVersion(uint8_t* major, uint8_t* minor, uint8_t* rev);

	/**
	 * @brief Store the shield hardware version in persistent memory.
	 *
	 * @param[in] major Major version number.
	 * @param[in] minor Minor version number.
	 * @param[in] rev   Revision number.
	 *
	 * @return `0` if the version was correctly stored,
	 *         `-1` if there was an underlying storage error.
	 */
	int8_t setShieldVersion(uint8_t major, uint8_t minor, uint8_t rev);

	/**
	 * @brief Retrieve the shield hardware version from persistent memory.
	 *
	 * @param[out] major Pointer to receive the major version number.
	 * @param[out] minor Pointer to receive the minor version number.
	 * @param[out] rev   Pointer to receive the revision number.
	 *
	 * @return `0` if the version was correctly retrieved,
	 *         `-1` if there was an underlying storage error.
	 */
	int8_t getShieldVersion(uint8_t* major, uint8_t* minor, uint8_t* rev);

	/**
	 * @brief Store the shield password in persistent memory.
	 *
	 * @note This is not a secure secret store: the password is written in
	 *       plain form to flash, retrievable via `getShieldPassword()`.
	 *       It is only meant to gate casual shield/board mismatches.
	 *
	 * @param[in] password      Pointer to a buffer of exactly
	 *                          `SHIELD_PASSWORD_LEN` (3) raw ASCII bytes.
	 * @param[in] password_size Size of `password` in bytes, must be
	 *                          exactly `SHIELD_PASSWORD_LEN` (3).
	 *
	 * @return `0` if the password was correctly stored, negative value
	 *         on error:
	 *
	 * - `-1`: underlying storage error,
	 *
	 * - `-2`: `password_size` is not exactly `SHIELD_PASSWORD_LEN`.
	 */
	int8_t setShieldPassword(const char* password, uint8_t password_size);

	/**
	 * @brief Retrieve the shield password from persistent memory.
	 *
	 * @param[in]  buffer      Buffer to receive the 3 raw ASCII bytes.
	 * @param[in]  buffer_size Size of `buffer`, must be at least
	 *                         `SHIELD_PASSWORD_LEN` (3).
	 *
	 * @return Number of bytes read (3) on success, negative value on error:
	 *
	 * - `-1`: underlying storage error,
	 *
	 * - `-2`: provided buffer is smaller than `SHIELD_PASSWORD_LEN`.
	 */
	int8_t getShieldPassword(char* buffer, uint8_t buffer_size);

	/**
	 * @brief Store one of the generic extra metadata slots.
	 *
	 * @param[in] index     Slot index, in `[0, METADATA_EXTRA_COUNT - 1]`.
	 * @param[in] data      Pointer to the data to store.
	 * @param[in] data_size Size of `data` in bytes, at most
	 *                      `METADATA_EXTRA_MAX_LEN` (16).
	 *
	 * @return `0` if the data was correctly stored, negative value on error:
	 *
	 * - `-1`: underlying storage error,
	 *
	 * - `-2`: `data_size` exceeds `METADATA_EXTRA_MAX_LEN`,
	 *
	 * - `-3`: `index` is out of range.
	 */
	int8_t setExtraData(uint8_t index, const uint8_t* data, uint8_t data_size);

	/**
	 * @brief Retrieve one of the generic extra metadata slots.
	 *
	 * @param[in] index       Slot index, in `[0, METADATA_EXTRA_COUNT - 1]`.
	 * @param[in] buffer      Buffer to receive the stored data.
	 * @param[in] buffer_size Size of `buffer` in bytes, must be at least
	 *                        `METADATA_EXTRA_MAX_LEN` (16), since the
	 *                        actual stored size for this slot is not
	 *                        known ahead of the read.
	 *
	 * @return Number of bytes read on success, negative value on error:
	 *
	 * - `-1`: underlying storage error,
	 *
	 * - `-2`: `buffer_size` is smaller than `METADATA_EXTRA_MAX_LEN`,
	 *
	 * - `-3`: `index` is out of range.
	 */
	int8_t getExtraData(uint8_t index, uint8_t* buffer, uint8_t buffer_size);

	/**
	 * @brief Erase all board/shield metadata fields (serial numbers,
	 *        versions, password, and all extra slots).
	 *
	 * @note This only erases the metadata fields owned by this class. It
	 *       does not affect ADC calibration data or safety thresholds,
	 *       which live under different NVS categories in the same
	 *       partition.
	 *
	 * @return `0` if all fields were correctly erased,
	 *         `-1` if there was an underlying storage error.
	 */
	int8_t clearAllMetaData();

};


#endif /* METADATAAPI_H_ */
