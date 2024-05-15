/*
 * Copyright (c) 2022-2023 LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */

#ifndef UARTHAL_H_
#define UARTHAL_H_

/**
 * @brief  Handles usart 1 for the SPIN board
 *
 * @note   Use this element to initialize and send messages via usart 1
 */
class UartHAL
{
public:
	/**
	 * @brief Library initialization function for the usart communication.
	 *        This function is declared on the private section of the .c file
	 */
	void usart1Init();

	/**
	 * @brief This function sends back a single character waiting to be treated from the usart1
	 *
	 * @return This function returns a single char which is waiting to be treated
	 *          if no char is waiting it returns an 'x' which should be treated as an error
	 */
	char usart1ReadChar();

	/**
	 * @brief This function transmits a single character through the usart 1
	 *
	 * @param data single char to be sent out
	 */
	void usart1WriteChar(char data);

	/**
	 * @brief This function swaps the usart RX and TX pins. It should be called in conjunction with a board version setup.
	 */
	void usart1SwapRxTx();


};



#endif // UARTHAL_H_
