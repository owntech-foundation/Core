/*
 * Copyright (c) 2024-present LAAS-CNRS
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
 * @date   2024
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


#ifndef CANCOMMUNICATION_H_
#define CANCOMMUNICATION_H_

#include <stdint.h>
#include <arm_math.h>

/* Static class definition */

class CanCommunication
{

public:
	/**
	 * @brief Get the CAN node address.
	 *
	 * @return The CAN node address.
	 */
	static uint16_t getCanNodeAddr();

		/**
	 * @brief Set the CAN node address.
	 *
	 * @param addr The CAN node address to set.
	 */
	static void setCanNodeAddr(uint16_t addr);

#ifdef CONFIG_THINGSET_CAN_CONTROL_REPORTING

	/**
	 * @brief Get the control enable status.
	 *
	 * @return True if control is enabled, false otherwise.
	 */
	static bool getCtrlEnable();

	/**
	 * @brief Get the control reference value.
	 *
	 * @return The control reference value.
	 */
	static float32_t getCtrlReference();

	/**
	 * @brief Get the start - stop current order
	 *
	 * @return A boolean that is 1 is current order is start, 0 if stop.
	 */
	static float32_t getStartStopState();
	/**
	 * @brief Get the control period.
	 *
	 * @return The control period in milli seconds (ms).
	 */
	static uint16_t getControlPeriod();

	/**
	 * @brief Set the control enable status.
	 *
	 * @param enable True to enable control, false to disable it.
	 */
	static void setCtrlEnable(bool enable);

	/**
	 * @brief Set the control reference value.
	 *
	 * @param reference The control reference value to set.
	 */
	static void setCtrlReference(float32_t reference);

	/**
	 * @brief Set the order to start slave devices
	 */
	static void startSlaveDevice();

	/**
	 * @brief Set the order to stop slave devices
	 */
	static void stopSlaveDevice();

	/**
	 * @brief Set the control period.
	 *
	 * @param time_ms The control period in milli seconds (ms).
	 */
	static void setControlPeriod(uint16_t time_ms);

#endif

#ifdef CONFIG_THINGSET_SUBSET_LIVE_METRICS

	/**
	 * @brief Get the Broadcasting enable status.
	 *
	 * @return True if broadcasting is enabled, false otherwise.
	 */
	static bool getBroadcastEnable();

	/**
	 * @brief Get the broadcast period.
	 *
	 * @return The broadcast period in seconds (s).
	 */
	static uint16_t getBroadcastPeriod();

	/**
	 * @brief Set the Broadcasting enable status.
	 *
	 * @param enable True to enable broadcasting, false to disable it.
	 */
	static void setBroadcastEnable(bool enable);

	/**
	 * @brief Set the broadcast period.
	 *
	 * @param time_s The broadcast period in seconds (s).
	 */
	static void setBroadcastPeriod(uint16_t time_s);

#endif

private:
	static struct thingset_can *ts_can_inst;
};

#endif /* CANCOMMUNICATION_H_ */
