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
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef RS485COMMUNICATION_H_
#define RS485COMMUNICATION_H_

#include <stdint.h>

#ifdef CONFIG_OWNTECH_COMMUNICATION_ENABLE_RS485

/////
// Static class definition

class Rs485Communication
{
    public :
    void configureDefault(uint8_t *transmission_bufer, uint8_t *reception_buffer, uint16_t data_size, void (*user_function)());
    void configure(uint8_t *transmission_bufer, uint8_t *reception_buffer, uint16_t data_size, void (*user_function)(void), uint32_t baudrate, bool oversampling_8);
    void startTransmission();
    void communicationStart();
    void communicationStop();
};

/////
// Public object to interact with the class

extern Rs485Communication rs485Communication;

#endif // CONFIG_OWNTECH_COMMUNICATION_ENABLE_RS485

#endif // RS485COMMUNICATION_H_
