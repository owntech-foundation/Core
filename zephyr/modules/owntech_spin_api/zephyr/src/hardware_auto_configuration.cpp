/*
 * Copyright (c) 2022-present LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Jean	Alinei <jean.alinei@laas.fr>
 *
 * @brief  This file automatically performs some hardware configuration
 *         using Zephyr macros.
 *         Configuration done in this file is low-level peripheral configuration
 *         required for OwnTech board to operate, do not mess with it unless you
 *         are absolutely sure of what you're doing.
 *         This file does not contain any public function.
 */


/* Zephyr */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/console/console.h>

/* STM32 LL */
#include <stm32_ll_bus.h>
#include <stm32_ll_system.h>

/* Owntech driver */
#include "dac.h"

static const struct device* dac2 = DEVICE_DT_GET(DAC2_DEVICE);

/* Functions to be run */

/**
 * @brief Initialize the internal voltage reference buffer (VREFBUF).
 *
 * Enables the SYSCFG clock, configures the voltage scaling, disables
 * high-impedance mode, and activates the VREFBUF output.
 *
 * @return Always returns 0 (success).
 */
static int _vrefbuf_init()
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_VREFBUF_SetVoltageScaling(LL_VREFBUF_VOLTAGE_SCALE0);
	LL_VREFBUF_DisableHIZ();
	LL_VREFBUF_Enable();

	return 0;
}

/**
 * @brief Initialize DAC2 in constant voltage mode.
 *
 * If DAC2 is ready, this function sets an output value of 2048 (mid-scale),
 * configures it for external output, and starts the DAC.
 *
 * @return Always returns 0 (success).
 */
static int _dac2_init()
{
	if (device_is_ready(dac2) == true)
	{
		dac_set_const_value(dac2, 1, 2048U);
		dac_pin_configure(dac2, 1, dac_pin_external);
		dac_start(dac2, 1);
	}

	return 0;
}

/**
 * @brief Initialize the console backend (e.g., UART).
 *
 * Calls the console_init function defined elsewhere in the system.
 *
 * @return Always returns 0 (success).
 */
static int _console_init()
{
	console_init();

	return 0;
}

#ifdef CONFIG_BOOTLOADER_MCUBOOT
#include <zephyr/kernel.h>
#include <zephyr/dfu/mcuboot.h>

/**
 * @brief Validate and confirm the current firmware image in MCUBoot.
 *
 * If the image is not yet confirmed, this function writes the confirmation flag.
 * Useful in MCUboot-based systems to prevent rollback after boot.
 *
 * @return Always returns 0 (success).
 */
static int _img_validation()
{
	if (boot_is_img_confirmed() == false)
	{
		int rc = boot_write_img_confirmed();
		if (rc != 0)
		{
			printk("Failed to confirm image");
		}
	}

	return 0;
}
#endif /* CONFIG_BOOTLOADER_MCUBOOT */


#if defined(CONFIG_RETENTION_BOOT_MODE) && \
	defined(CONFIG_CDC_ACM_DTE_RATE_CALLBACK_SUPPORT) && \
	defined(CONFIG_USB_CDC_ACM)

#include <zephyr/retention/bootmode.h>
#include <zephyr/sys/reboot.h>

/**
 * @brief Submit a warm reboot into bootloader mode.
 *
 * Called by the 1200 baud callback to initiate a soft reset into DFU mode.
 *
 * @param work Pointer to work item (unused).
 */
void reboot_bootloader_task(struct k_work* work)
{
	bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);
	sys_reboot(SYS_REBOOT_WARM);
}

K_WORK_DEFINE(reboot_bootloader_work, reboot_bootloader_task);

/**
 * @brief Register the CDC ACM baud rate callback.
 *
 * Used in USB bootloader entry mechanism (e.g., Arduino-style 1200bps trick).
 *
 * @return Always returns 0 (success).
 */
void _cdc_rate_callback(const struct device* dev, uint32_t rate)
{
	if (rate == 1200)
	{
		k_work_submit(&reboot_bootloader_work);
	}
}

#include <zephyr/drivers/uart/cdc_acm.h>
#define CDC_ACM_DEVICE DT_NODELABEL(cdc_acm_uart0)
static const struct device* cdc_acm_console = DEVICE_DT_GET(CDC_ACM_DEVICE);

/**
 * @brief Register the CDC ACM baud rate callback.
 *
 * Used in USB bootloader entry mechanism (e.g., Arduino-style 1200bps trick).
 *
 * @return Always returns 0 (success).
 */
static int _register_cdc_rate_callback()
{
	cdc_acm_dte_rate_callback_set(cdc_acm_console, _cdc_rate_callback);

	return 0;
}
#endif /* CONFIG_RETENTION_BOOT_MODE &&
		* CONFIG_CDC_ACM_DTE_RATE_CALLBACK_SUPPORT &&
		* CONFIG_USB_CDC_ACM */


#ifdef CONFIG_SHIELD_O2
#include <stm32_ll_lpuart.h>
/**
 * @brief Swap TX and RX lines for USART1 (LPUART1).
 *
 * Disables the LPUART1 peripheral, swaps the TX/RX pins, and re-enables it.
 * 
 * Used with the O2 board. 
 *
 * @return Always returns 0 (success).
 */
static int _swap_usart1_tx_rx()
{
	LL_LPUART_Disable(LPUART1);
	LL_LPUART_SetTXRXSwap(LPUART1, LL_LPUART_TXRX_SWAPPED);
	LL_LPUART_Enable(LPUART1);

	return 0;
}
#endif /* SHIELD_O2 */

/**
 *  Zephyr macros to automatically run above functions
 */

/* To be run in the first init phase */
SYS_INIT(_vrefbuf_init,
         PRE_KERNEL_1,
         CONFIG_KERNEL_INIT_PRIORITY_DEVICE
        );

/* To be run in the second init phase (depends on DAC driver initialization)*/
SYS_INIT(_dac2_init,
         PRE_KERNEL_2,
         CONFIG_KERNEL_INIT_PRIORITY_DEVICE
        );

SYS_INIT(_console_init,
         APPLICATION,
         89
        );

#ifdef CONFIG_BOOTLOADER_MCUBOOT
SYS_INIT(_img_validation,
         APPLICATION,
         CONFIG_APPLICATION_INIT_PRIORITY
        );
#endif /* CONFIG_BOOTLOADER_MCUBOOT */

#if defined(CONFIG_RETENTION_BOOT_MODE) && \
	defined(CONFIG_CDC_ACM_DTE_RATE_CALLBACK_SUPPORT) && \
	defined(CONFIG_USB_CDC_ACM)

SYS_INIT(_register_cdc_rate_callback,
         APPLICATION,
         CONFIG_APPLICATION_INIT_PRIORITY
        );
#endif /* CONFIG_RETENTION_BOOT_MODE &&
		* CONFIG_CDC_ACM_DTE_RATE_CALLBACK_SUPPORT &&
		* CONFIG_USB_CDC_ACM */

#ifdef CONFIG_SHIELD_O2
/* To be run in the first init phase */
SYS_INIT(_swap_usart1_tx_rx,
         PRE_KERNEL_1,
         CONFIG_KERNEL_INIT_PRIORITY_DEVICE
        );
#endif /* SHIELD_O2*/
