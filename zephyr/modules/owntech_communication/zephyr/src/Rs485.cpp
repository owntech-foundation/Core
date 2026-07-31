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

/*
 * @date   2023
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

/* LL drivers */
#include <stm32_ll_dma.h>
#include <stm32_ll_gpio.h>
#include <stm32_ll_bus.h>

/* Zephyr drivers */
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/irq.h>

/* Header */
#include "Rs485.h"

#define DMA_USART DMA1 /* DMA used */

/**
 *  HAL override is used because of undesired effect in zephyr's dma handler
 *  dma_stm32_irq_handler. We noticed that we called the callback only once,
 *  setting stream->busy to false then never called the callback again.
 *  This problem was only noticed in zephyr 3.3 and not in zephyr 2.7
 *  that platformIO is using. For the sake of safety, we'll use HAL override.
 */
#define STM32_DMA_HAL_OVERRIDE 0x7F

/**
 * Warning : if you change the channels number,
 * you'll have to change some code line manually
 */

/* Transmission DMA channel for zephyr driver */
#define ZEPHYR_DMA_CHANNEL_TX 6
/* Reception DMA channel for zephyr driver */
#define ZEPHYR_DMA_CHANNEL_RX 7

/**
 * Warning : if you change the channels,
 * you'll have to change some code line manually
 */

/* Transmission DMA channel for LL driver */
#define LL_DMA_CHANNEL_TX LL_DMA_CHANNEL_6
/* Reception DMA channel for LL driver */
#define LL_DMA_CHANNEL_RX LL_DMA_CHANNEL_7

/* DT definitions */
static const struct device *dma1 = DEVICE_DT_GET(DT_NODELABEL(dma1));
static const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(usart3));

/* USART initialization parameters */

/* Initial baudrate to  10.625Mhz */
static uint32_t baud = 21250000 / (2);
struct uart_config uart_cfg;
struct uart_event evt;

/* DMA buffer for transmission and reception */
static uint8_t* tx_usart_val;
static uint8_t* rx_usart_val;

static uint16_t dma_buffer_size;

/* User function to call in RX callback */
static dma_callbackRXfunc_t user_fnc = NULL;

/* Private functions */

/**
 *  DMA callback TX: called by Zephyr DMA driver on transfer completion.
 */
static void _dma_callback_tx(const struct device *dev,
                             void *user_data,
                             uint32_t channel,
                             int status)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(user_data);
    ARG_UNUSED(channel);
    ARG_UNUSED(status);

    LL_USART_ClearFlag_TXFE(USART3);
    LL_USART_ClearFlag_TC(USART3);
}

/**
 *  DMA callback RX direct ISR (ZLI path).
 *  With cyclic DMA configured through Zephyr, both HT and TC IRQs can be
 *  raised. We clear HT and only trigger user callback on TC.
 */
ISR_DIRECT_DECLARE(_dma_callback_rx_direct)
{
    bool is_half_transfer = (LL_DMA_IsActiveFlag_HT7(DMA_USART) != 0U);
    bool is_transfer_complete = (LL_DMA_IsActiveFlag_TC7(DMA_USART) != 0U);

    if (is_half_transfer)
    {
        LL_DMA_ClearFlag_HT7(DMA_USART);
    }

    if (is_transfer_complete)
    {
        LL_DMA_ClearFlag_TC7(DMA_USART);

        if (user_fnc != NULL)
        {
            user_fnc();
        }
    }

    /* No scheduling decision from this ISR path */
    return 0;
}

/**
 *  DMA callback RX wrapper for Zephyr DMA driver API.
 *  If IRQ_DIRECT_CONNECT is used for Zephyr DMA RX channel, 
 *  this callback will not be used and the direct ZLI ISR will 
 *  be triggered instead. Otherwise, this callback will be used 
 *  and will call the user function on transfer complete.
 */
static void _dma_callback_rx_zephyr(const struct device *dev,
                                    void *user_data,
                                    uint32_t channel,
                                    int status)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(user_data);
    ARG_UNUSED(channel);

    if (status == DMA_STATUS_COMPLETE)
    {
        if (user_fnc != NULL)
        {
            user_fnc();
        }
    }
}

/* Public functions */


/**
 *  Initialize RX and TX buffer by user
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
 *  Initialize baudrate by user
 */
void init_usrBaudrate(uint32_t usr_baud)
{
    baud = usr_baud;
}

/**
 *  Set the size of data we send ie. the number of bytes.
 *  eg. : size = 5 means that we send 5 byte (40 bits) of data to USART 3.
 */
void init_usrDataSize(uint16_t size)
{
    dma_buffer_size = size;
}

/**
 *  This function initialise USART3 peripheral
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
    LL_USART_EnableOverrunDetect(USART3);

    /* Enable DMA request*/
    LL_USART_EnableDMAReq_TX(USART3);
    LL_USART_EnableDMAReq_RX(USART3);

    /* Disable Interrupts for TX (not used for DMA) */

    /* Disable Transmission Complete Interrupt */
    LL_USART_DisableIT_TC(USART3);

    /* Disable Transmission Data Register Empty Interrupt
     * for DMA to provide data.
     * Disable interrupts for RX (not used with DMA) */
    LL_USART_DisableIT_TXE_TXFNF(USART3);
    /* Disable Receiver Data Register
     * Not Empty Interrupt for DMA to fetch data */
    LL_USART_DisableIT_RXNE_RXFNE(USART3);

    LL_USART_Enable(USART3);
}

/**
 *  Initialize driver enable mode for RS485 flowcontrol.
 *  See RM0440 37.5.20 for more details.
 */
void init_DEmode(void)
{
    LL_USART_Disable(USART3);

    /* GPIO initialization and GPIO clock set-up */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    /* Set GPIO_InitStruct */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Enable DE mode */
    LL_USART_EnableDEMode(USART3);
    /* Polarity is high ie. PB14 will be at high level when sending datas */
    LL_USART_SetDESignalPolarity(USART3, LL_USART_DE_POLARITY_HIGH);

    /* Assertion time is set to maximum */
    LL_USART_SetDEAssertionTime(USART3, 31);
    LL_USART_SetDEDeassertionTime(USART3, 31);

    LL_USART_Enable(USART3);
}


/**
 *  Set the oversampling.
 *  See RM0440 37.5.7 for more details.
 */
void oversamp_set(usart_oversampling_t oversampling)
{
    LL_USART_Disable(USART3);
    LL_USART_SetOverSampling(USART3, oversampling);
    LL_USART_Enable(USART3);
}

/**
 *  This is the TX dma channel initialization.
 *  The channel is not enabled here to avoid sending data unexpectedly,
 *  this channel is enabled only with serial_tx_on when data must be sent.
 */
void dma_channel_init_tx()
{
    struct dma_block_config dma_block_config_s = {0};
    struct dma_config dma_config_s = {0};

    /* Memory-to-peripheral transfer configuration (TX buffer -> USART TDR). */
    dma_block_config_s.source_address = (uint32_t)(tx_usart_val);
    dma_block_config_s.dest_address = (uint32_t)(&(USART3->TDR));
    dma_block_config_s.block_size = dma_buffer_size;
    dma_block_config_s.source_addr_adj = DMA_ADDR_ADJ_INCREMENT;
    dma_block_config_s.dest_addr_adj = DMA_ADDR_ADJ_NO_CHANGE;
    dma_block_config_s.source_reload_en = 0;
    dma_block_config_s.dest_reload_en = 0;

    dma_config_s.dma_slot = LL_DMAMUX_REQ_USART3_TX;
    dma_config_s.channel_direction = MEMORY_TO_PERIPHERAL;
    dma_config_s.channel_priority = 3;
    dma_config_s.source_data_size = 1;
    dma_config_s.dest_data_size = 1;
    dma_config_s.source_burst_length = 1;
    dma_config_s.dest_burst_length = 1;
    dma_config_s.block_count = 1;
    dma_config_s.head_block = &dma_block_config_s;
    dma_config_s.dma_callback = _dma_callback_tx;

    dma_config(dma1, ZEPHYR_DMA_CHANNEL_TX, &dma_config_s);
}


/**
 *  DMA channel RX initialization, this channel is set on circular mode.
 */
void dma_channel_init_rx()
{
    struct dma_block_config dma_block_config_s = {0};
    struct dma_config dma_config_s = {0};

    /* Circular peripheral-to-memory transfer configuration */
    dma_block_config_s.source_address = (uint32_t)(&(USART3->RDR));
    dma_block_config_s.dest_address = (uint32_t)(rx_usart_val);
    dma_block_config_s.block_size = dma_buffer_size;
    dma_block_config_s.source_addr_adj = DMA_ADDR_ADJ_NO_CHANGE;
    dma_block_config_s.dest_addr_adj = DMA_ADDR_ADJ_INCREMENT;
    dma_block_config_s.source_reload_en = 1;
    dma_block_config_s.dest_reload_en = 1;

    dma_config_s.dma_slot = LL_DMAMUX_REQ_USART3_RX;
    dma_config_s.channel_direction = PERIPHERAL_TO_MEMORY;
    dma_config_s.channel_priority = 3;
    dma_config_s.source_data_size = 1;
    dma_config_s.dest_data_size = 1;
    dma_config_s.source_burst_length = 1;
    dma_config_s.dest_burst_length = 1;
    dma_config_s.block_count = 1;
    dma_config_s.head_block = &dma_block_config_s;
    dma_config_s.dma_callback = _dma_callback_rx_zephyr;

    if (dma_config(dma1, ZEPHYR_DMA_CHANNEL_RX, &dma_config_s) != 0)
    {
        return;
    }

    /* Bind DMA1 channel IRQ line (17) to the direct zero-latency ISR. */
    IRQ_DIRECT_CONNECT(17, 0, _dma_callback_rx_direct, IRQ_ZERO_LATENCY);
    irq_enable(17);

    /* Start RX DMA channel through Zephyr API */
    dma_start(dma1, ZEPHYR_DMA_CHANNEL_RX);
}


/**
 *  Reload DMA TX buffer.
 *  This functions enable TX channel to start sending the datas
 */
void serial_tx_on()
{
    if (dma_reload(dma1,
                   ZEPHYR_DMA_CHANNEL_TX,
                   (uint32_t)(tx_usart_val),
                   (uint32_t)(&(USART3->TDR)),
                   dma_buffer_size) != 0)
    {
        return;
    }

    dma_start(dma1, ZEPHYR_DMA_CHANNEL_TX);
}

/**
 *  To stop communication
 */
void serial_stop()
{
    LL_USART_Disable(USART3);
}

/**
 *  To start or restart communication
 */
void serial_start()
{
    LL_USART_Enable(USART3);
}

/**
 * This function is used to recover the RX DMA if an overrun error occurs.
 * It checks if the overrun error flag is set, and if so, it clears the overrun state and 
 * re-arms the RX DMA channel to continue receiving data. 
 * See https://github.com/owntech-foundation/MMC/issues/6
 */
int8_t reinitialize_rx_dma_if_overrun()
{
    if (LL_USART_IsActiveFlag_ORE(USART3) == 0U)
    {
        return -1;
    }

    /* Clear UART RX overrun state first. */
    LL_USART_RequestRxDataFlush(USART3);
    LL_USART_ClearFlag_ORE(USART3);

    /* Re-arm circular RX DMA from the beginning */
    LL_DMA_DisableChannel(DMA_USART, LL_DMA_CHANNEL_RX);
    LL_DMA_ClearFlag_TC7(DMA_USART);
    LL_DMA_ClearFlag_HT7(DMA_USART);
    LL_DMA_ClearFlag_TE7(DMA_USART);
    LL_DMA_SetMemoryAddress(DMA_USART,
                            LL_DMA_CHANNEL_RX,
                            (uint32_t)(rx_usart_val));
    LL_DMA_SetDataLength(DMA_USART, LL_DMA_CHANNEL_RX, dma_buffer_size);
    LL_DMA_EnableIT_TC(DMA_USART, LL_DMA_CHANNEL_RX);
    LL_DMA_DisableIT_HT(DMA_USART, LL_DMA_CHANNEL_RX);
    LL_DMA_EnableChannel(DMA_USART, LL_DMA_CHANNEL_RX);

    return 0;
}
