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
 * @date   2023
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */
#ifndef RS485_H_
#define RS485_H_

#include <zephyr/kernel.h>

#include <stm32_ll_usart.h>

typedef void (*dma_callbackRXfunc_t)();

typedef enum{
    OVER8 = LL_USART_OVERSAMPLING_8,
    OVER16 = LL_USART_OVERSAMPLING_16
}usart_oversampling_t;

/**
 * @brief Initialize user transmission and reception buffer
 *
 * @param[in] tx_buffer Transmission buffer
 * @param[in] rx_buffer Reception buffer
 */
void init_usrBuffer(uint8_t* tx_buffer, uint8_t* rx_buffer);

/**
 * @brief Initialize user function called every RX callback
 *
 * @param[in] fnc_callback Void function with no parameters, use NULL if there is no function to call
 */
void init_usrFunc(dma_callbackRXfunc_t fnc_callback);

/**
 * @brief Initialize baudrate with user choice.
 *
 * @param[in] usr_baud Baud in bits per second
 * @note Baudrate is initialized by default to 10Mbps if this function is not used
 */
void init_usrBaudrate(uint32_t usr_baud);

/**
 * @brief Initialize DMA data size to send and receive
 *
 * @param[in] size Size of the data in byte (max 65535)
 */
void init_usrDataSize(uint16_t size);

/**
 * @brief Initialize USART3
 */
void serial_init(void);

/**
 * @brief Initialize Driver Enable mode for RS485 hardware flowcontrol.
 *        pin B14 is set to level high when data is transmitted.
 */
void init_DEmode(void);

/**
 * @brief Set oversampling which is by default oversampling_16. With oversampling_8 we increase speed
 *        by 2 but decrease USART sampling by half making it more sensitive to noise.
 *
 * @param[in] oversampling choice of oversampling
 *            @arg @ref OVER8 oversampling_8
 *            @arg @ref OVER16 oversampling_16
 */
void oversamp_set(usart_oversampling_t oversampling);

/**
 * @brief Initialize dma 1 channel 6 for sending data via USART3.
 */
void dma_channel_init_tx();

/**
 * @brief Initialize dma 1 channel 7 for receiving data in circular mode
 */
void dma_channel_init_rx();

/**
 * @brief Reload dma buffer TX
 * @note After reloading the buffer, USART will start sending datas
 */
void serial_tx_on();

/**
 * @brief Disable USART
 */
void serial_stop();

/**
 * @brief Enable USART
 */
void serial_start();

#endif /* RS485_H_ */
