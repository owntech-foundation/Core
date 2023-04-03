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
 * SPDX-License-Identifier: LGLPV2.1
 */

/**
 * @date   2023
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


// STM 32 LL
#include <stm32_ll_lpuart.h>

// Zephyr
#include <drivers/uart.h>

// Current file header
#include "HardwareConfiguration.h"


/////
// USART 1 defines

#define CONFIG_OWNTECH_SERIAL_TX_BUF_SIZE 5
#define CONFIG_OWNTECH_SERIAL_RX_BUF_SIZE 5
#define USART1_DEVICE DT_PROP(DT_NODELABEL(usart1), label)

static const struct device* uart_dev = DEVICE_DT_GET(DT_NODELABEL(usart1));
static char buf_req[CONFIG_OWNTECH_SERIAL_RX_BUF_SIZE];
static bool command_flag = false;


/////
// USART 1 private functions

static void _uart_usart1_process_input(const struct device *dev, void* user_data)
{
	uint8_t c;

	if (!uart_irq_update(uart_dev)) {
		printk("no data \n");
		return;
	}

	while (uart_irq_rx_ready(uart_dev) && command_flag == false) {
		uart_fifo_read(uart_dev, &c, 1);
		printk("received %c \n",c);
		buf_req[0] = c;
		command_flag = true;
	}
}

/////
// USART 1 public functions

void HardwareConfiguration::extraUartInit()
{
	const struct uart_config usart1_config =
	{
		.baudrate  = 9600,
		.parity    = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
	};

	if (device_is_ready(uart_dev) == true)
	{
		uart_configure(uart_dev, &usart1_config);
		uart_irq_callback_user_data_set(uart_dev, _uart_usart1_process_input, NULL);
		uart_irq_rx_enable(uart_dev);
	}
}

char HardwareConfiguration::extraUartReadChar()
{
	if (command_flag){
		command_flag = false;
		return buf_req[0];
	} else {
		return 'x';		// returns an x to signal there is no command waiting to be treated
	}
}

void HardwareConfiguration::extraUartWriteChar(char data)
{
	if (device_is_ready(uart_dev) == true)
	{
		uart_poll_out(uart_dev,data);
	}
}

void HardwareConfiguration::uart1SwapRxTx()
{
	LL_LPUART_Disable(LPUART1);
	LL_LPUART_SetTXRXSwap(LPUART1, LL_LPUART_TXRX_SWAPPED);
	LL_LPUART_Enable(LPUART1);
}
