/*
 * Copyright (c) 2023 LAAS-CNRS
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
 * SPDX-License-Identifier: LGLPV2.1
 */

/**
 * @date   2023
 *
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef SYNCCOMMUNICATION_H_
#define SYNCCOMMUNICATION_H_

#ifdef CONFIG_OWNTECH_COMMUNICATION_ENABLE_SYNC

#include <stdint.h>


/////
// Static class definition

class SyncCommunication
{

public:
	static void initMaster();
	static void initSlave();
};


/////
// Public object to interact with the class

extern SyncCommunication syncCommunication;


#endif // CONFIG_OWNTECH_COMMUNICATION_ENABLE_SYNC

#endif // SYNCCOMMUNICATION_H_

