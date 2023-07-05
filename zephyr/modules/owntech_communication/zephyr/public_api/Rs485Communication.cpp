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

#include "../src/Rs485.h"
#include "Rs485Communication.h"

Rs485Communication rs485Communication;

void Rs485Communication::configureDefault(uint8_t* transmission_bufer, uint8_t* reception_buffer, uint16_t data_size, void (*user_function)())
{
    init_usrBuffer(transmission_bufer, reception_buffer);
    init_usrFunc(user_function);
    init_usrDataSize(data_size);
    dma_channel_init_tx();
    dma_channel_init_rx();
    serial_init();
    init_DEmode();
}

void Rs485Communication::configure(uint8_t* transmission_bufer, uint8_t* reception_buffer, uint16_t data_size, void (*user_function)(void), uint32_t baudrate, bool oversampling_8)
{
    init_usrBuffer(transmission_bufer, reception_buffer);
    init_usrFunc(user_function);
    init_usrDataSize(data_size);
    init_usrBaudrate(baudrate);
    dma_channel_init_tx();
    dma_channel_init_rx();
    serial_init();
    init_DEmode();
    if(oversampling_8 == true) oversamp_set(OVER8);
    else oversamp_set(OVER16);
}

void Rs485Communication::startTransmission()
{
    serial_tx_on();
}

void Rs485Communication::communicationStart()
{
    serial_start();
}

void Rs485Communication::communicationStop()
{
    serial_stop();
}
