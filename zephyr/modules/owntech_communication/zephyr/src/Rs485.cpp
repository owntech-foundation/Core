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

/* ll drivers */
#include <stm32_ll_dma.h>
#include <stm32_ll_gpio.h>
#include <stm32_ll_bus.h>

/* Zephyr drivers */
#include <drivers/uart.h>
#include <drivers/dma.h>

/* Header */
#include "Rs485.h"

#define DMA_USART DMA1 // DMA used

/**
 * HAL override is used because of undesired effect in zephyr's dma handler dma_stm32_irq_handler.
 * We noticed that we called the callback only once, setting stream->busy to false then never called
 * the callback again. This problem was only noticed in zephyr 3.3 and not in zephyr 2.7
 * that platformIO is using. For the sake of safety, we'll use Hal override.
**/
#define STM32_DMA_HAL_OVERRIDE 0x7F

/* Warning : if you change the channels number, you'll have to change some code line manually*/
#define ZEPHYR_DMA_CHANNEL_TX 6 // transmission dma channel for zephyr driver
#define ZEPHYR_DMA_CHANNEL_RX 7 // reception dma channel for zephyr driver

/* Warning : if you change the channels, you'll have to change some code line manually*/
#define LL_DMA_CHANNEL_TX LL_DMA_CHANNEL_6 // transmission dma channel for LL driver
#define LL_DMA_CHANNEL_RX LL_DMA_CHANNEL_7 // reception dma channel for LL driver

/* DT definitions */
static const struct device *dma1 = DEVICE_DT_GET(DT_NODELABEL(dma1));
static const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(usart3));

/* USART initialization parameters */
static uint32_t baud = 21250000 / (2); /* initial baudrate to  10.625Mhz */
struct uart_config uart_cfg;
struct uart_event evt;

/* DMA buffer for transmission and reception */
static uint8_t* tx_usart_val;
static uint8_t* rx_usart_val;

static uint16_t dma_buffer_size;

static dma_callbackRXfunc_t user_fnc = NULL; // user function to call in RX callback

//// Private functions

/**
 * DMA callback TX clear transmission flag, and disabled DMA channel TX.
*/
static void _dma_callback_tx(const struct device *dev, void *user_data, uint32_t channel, int status)
{
    LL_DMA_DisableChannel(DMA_USART, LL_DMA_CHANNEL_TX); // Disable DMA channel after sending datas

    LL_USART_ClearFlag_TXFE(USART3);
    LL_USART_ClearFlag_TC(USART3); // clear transmission complete flag USART
    LL_DMA_ClearFlag_TC6(DMA_USART); // clear transmission complete dma channel TX
}

/**
 * DMA callback RX clear reception flag, then call user functions
*/
static void _dma_callback_rx(const struct device *dev, void *user_data, uint32_t channel, int status)
{
    LL_DMA_ClearFlag_TC7(DMA_USART); // clear transmission complete flag

    if(user_fnc != NULL){
        user_fnc();
    }
}

//// Public functions

/**
 * Initialize RX and TX buffer by user
*/
void init_usrBuffer(uint8_t* tx_buffer, uint8_t* rx_buffer)
{
    tx_usart_val = tx_buffer;
    rx_usart_val = rx_buffer;
}

/**
 *  Initialize usr function to use in the RX callback
 */
void init_usrFunc(dma_callbackRXfunc_t fnc_callback)
{
    user_fnc = fnc_callback;
}

/**
 * Initialize baudrate by user
*/
void init_usrBaudrate(uint32_t usr_baud)
{
    baud = usr_baud;
}

/**
 * Set the size of data we send ie. the number of bytes.
 * eg. : size = 5 means that we send 5 byte (40 bits) of data to USART 3.
*/
void init_usrDataSize(uint16_t size)
{
    dma_buffer_size = size;
}

/**
 * This function initialise USART3 peripheral
*/
void serial_init(void)
{
    uart_config_get(uart_dev, &uart_cfg);
    uart_cfg.baudrate = baud;
    uart_cfg.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;
    uart_cfg.data_bits = UART_CFG_DATA_BITS_8;
    uart_cfg.parity = UART_CFG_PARITY_NONE;
    uart_cfg.stop_bits = UART_CFG_STOP_BITS_1;

    uart_configure(uart_dev, &uart_cfg);

    LL_USART_ConfigAsyncMode(USART3);

    /* Enable DMA request*/
    LL_USART_EnableDMAReq_TX(USART3);
    LL_USART_EnableDMAReq_RX(USART3);

    // disable Interrupts for TX (not used for DMA)
    LL_USART_DisableIT_TC(USART3);         // Disable Transmission Complete Interrupt
    LL_USART_DisableIT_TXE_TXFNF(USART3);  // Disable Transmission Data Register Empty Interrupt for DMA to provide data
                                           //  disable interrupts for RX (not used with DMA)
    LL_USART_DisableIT_RXNE_RXFNE(USART3); // Disable Receiver Data Register Not Empty Interrupt for DMA to fetch data

    LL_USART_Enable(USART3);
}

/**
 * Initialize driver enable mode for RS485 flowcontrol.
 * See RM0440 37.5.20 for more details.
*/
void init_DEmode(void)
{
    LL_USART_Disable(USART3);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; // GPIO initialization

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB); // Enable GPIO clock

    /* Set GPIO_InitStruct */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    LL_USART_EnableDEMode(USART3); // Enable DE mode

    LL_USART_SetDESignalPolarity(USART3, LL_USART_DE_POLARITY_HIGH); // Polarity is high ie. PB14 will be at high level when sending datas

    /* Assertion time is set to maximum */
    LL_USART_SetDEAssertionTime(USART3, 31);
    LL_USART_SetDEDeassertionTime(USART3, 31);

    LL_USART_Enable(USART3);
}


/**
 * Set the oversampling.
 * See RM0440 37.5.7 for more details.
*/
void oversamp_set(usart_oversampling_t oversampling)
{
    LL_USART_Disable(USART3);
    LL_USART_SetOverSampling(USART3, oversampling);
    LL_USART_Enable(USART3);
}

/**
 * This is the TX dma channel initialization. The channel is not enabled here to avoid
 * sending data unexpectedly, this channel is enabled only with serial_tx_on when data
 * must be sent.
*/
void dma_channel_init_tx()
{
    /*Configure DMA */
    struct dma_config dma_config_s = {0};
    LL_DMA_InitTypeDef DMA_InitStruct = {0};

    dma_config_s.dma_callback = _dma_callback_tx;          // Callback
    dma_config_s.linked_channel = STM32_DMA_HAL_OVERRIDE;  // Hal override

    /* DMA configuration with LL drivers */
    DMA_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)(&(USART3->TDR));
    DMA_InitStruct.MemoryOrM2MDstAddress = (uint32_t)(tx_usart_val);
    DMA_InitStruct.Mode = LL_DMA_MODE_NORMAL;
    DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
    DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
    DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    DMA_InitStruct.PeriphRequest = LL_DMAMUX_REQ_USART3_TX;
    DMA_InitStruct.NbData = dma_buffer_size;

    dma_config(dma1, ZEPHYR_DMA_CHANNEL_TX, &dma_config_s); // Indicates Callback function to zephyr driver

    LL_DMA_DisableChannel(DMA_USART, LL_DMA_CHANNEL_TX); // Disabling channel for initial set-up

    /* initialize DMA */
    LL_DMA_SetDataLength(DMA_USART, LL_DMA_CHANNEL_TX, dma_buffer_size); // DMA data size
    LL_DMA_SetChannelPriorityLevel(DMA_USART, LL_DMA_CHANNEL_TX, LL_DMA_PRIORITY_VERYHIGH); // DMA channel priority
    LL_DMA_Init(DMA_USART, LL_DMA_CHANNEL_TX, &DMA_InitStruct);

    /* Clearing flag */
    LL_DMA_ClearFlag_TC6(DMA_USART);
    LL_DMA_ClearFlag_HT6(DMA_USART);

    LL_DMA_EnableIT_TC(DMA_USART, LL_DMA_CHANNEL_TX); // Enable transfert complete interruption

    LL_DMA_DisableIT_HT(DMA_USART, LL_DMA_CHANNEL_TX); // Disable half-transfert interruption
}


/**
 * DMA channel RX initialization, this channel is set on circular mode.
*/
void dma_channel_init_rx()
{
    /* Configure DMA */
    struct dma_config dma_config_s = {0};
    LL_DMA_InitTypeDef DMA_InitStruct = {0};

    dma_config_s.dma_callback = _dma_callback_rx;          // Callback
    dma_config_s.linked_channel = STM32_DMA_HAL_OVERRIDE;  // Hal override

    /* Initialization of DMA */
    DMA_InitStruct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)(&(USART3->RDR));
    DMA_InitStruct.MemoryOrM2MDstAddress = (uint32_t)(rx_usart_val);
    DMA_InitStruct.Mode = LL_DMA_MODE_CIRCULAR;
    DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
    DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
    DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    DMA_InitStruct.PeriphRequest = LL_DMAMUX_REQ_USART3_RX;
    DMA_InitStruct.NbData = dma_buffer_size;

    dma_config(dma1, ZEPHYR_DMA_CHANNEL_RX, &dma_config_s); // Indicates Callback function to zephyr driver

    LL_DMA_DisableChannel(DMA_USART, LL_DMA_CHANNEL_RX); // Disabling channel for initial set-up

     /* initialize DMA */
    LL_DMA_SetDataLength(DMA_USART, LL_DMA_CHANNEL_RX, dma_buffer_size); // DMA data size
    LL_DMA_SetChannelPriorityLevel(DMA_USART, LL_DMA_CHANNEL_RX, LL_DMA_PRIORITY_VERYHIGH); // DMA channel priority
    LL_DMA_Init(DMA_USART, LL_DMA_CHANNEL_RX, &DMA_InitStruct);

    /* Clearing flag */
    LL_DMA_ClearFlag_TC7(DMA_USART);
    LL_DMA_ClearFlag_HT7(DMA_USART);

    LL_DMA_EnableChannel(DMA_USART, LL_DMA_CHANNEL_RX); // Enabling channel

    LL_DMA_EnableIT_TC(DMA_USART, LL_DMA_CHANNEL_RX); // Enable transfert complete interruption

    LL_DMA_DisableIT_HT(DMA_USART, LL_DMA_CHANNEL_RX); // Disable half-transfert interruption
}


/**
 * Reload DMA TX buffer. This functions enable TX channel to start sending the datas
*/
void serial_tx_on()
{
    LL_DMA_ClearFlag_TC6(DMA_USART); // Making sure the flag is cleared before transmission

    LL_DMA_DisableChannel(DMA_USART, LL_DMA_CHANNEL_TX); // disable channel to reload TX buffer

    /* reloading TX buffer */
    LL_DMA_SetMemoryAddress(DMA_USART, LL_DMA_CHANNEL_TX, (uint32_t)(tx_usart_val));
    LL_DMA_SetDataLength(DMA_USART, LL_DMA_CHANNEL_TX, dma_buffer_size);

    LL_DMA_EnableChannel(DMA_USART, LL_DMA_CHANNEL_TX); // re-enable the channel
}

/**
 * to stop communication
*/
void serial_stop()
{
    LL_USART_Disable(USART3);
}

/**
 * to start or restart communication
*/
void serial_start()
{
    LL_USART_Enable(USART3);
}
