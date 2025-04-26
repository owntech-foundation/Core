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
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef COMMUNICATIONAPI_H
#define COMMUNICATIONAPI_H

#include "../src/AnalogCommunication.h"
#include "../src/CanCommunication.h"
#include "../src/Rs485Communication.h"
#include "../src/SyncCommunication.h"

/**
 * @brief Main communication API interface.
 *
 * This class provides access to all supported communication interfaces,
 * including analog, CAN, RS485, and real-time synchronization protocols.
 *
 * - `analog`: provides analog communication functions.
 *
 * - `can`: provides CAN bus communication functions.
 *
 * - `rs485`: provides RS485 serial communication functions.
 *
 * - `sync`: provides real-time synchronization functions.
 *
 */
class CommunicationAPI
{
    public :
        /**
         * @brief Contains all the functions for Analog communication
         */
        AnalogCommunication analog;

        /**
         * @brief Contains all the functions for CAN communication
         */
        CanCommunication can;

        /**
         * @brief Contains all the functions for RS485 communication
         */
        Rs485Communication rs485;

        /**
         * @brief Contains all the functions for Real Time synchronization
         */
        SyncCommunication sync;

};

extern CommunicationAPI communication;

#endif /* COMMUNICATIONAPI_H */