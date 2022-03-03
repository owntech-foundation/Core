/*
 * Copyright (c) 2022 LAAS-CNRS
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
 * @date   2022
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef UART_CONFIGURATION_H_
#define UART_CONFIGURATION_H_


/**
 * @brief Library initialization function for the usart communication.
 *        This function is declared on the private section of the .c file
 */
void uart_usart1_init();

/**
 * @brief This function sends back a single character waiting to be treated from the usart1
 *
 * @return This function returns a single char which is waiting to be treated
 *          if no char is waiting it returns an 'x' which should be treated as an error
 */
char uart_usart1_get_data();

/**
 * @brief This function transmits a single character through the usart 1
 *
 * @param data single char to be sent out
 */
void uart_usart1_write_single(char data);


/**
 * Swap RX and TX to correct hardware mistake.
 */
void uart_lpuart1_swap_rx_tx();

#endif // UART_CONFIGURATION_H_
