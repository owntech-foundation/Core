/*
 * Copyright (c) 2021 LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 *
 * @brief  This is an ad-hoc ADC driver for OwnTech's
 * application. It supports differential channel setup
 * unlike Zephyr's STM32 driver.
 * It configures ADC 1 and ADC 2, using a common clock
 * which is AHB clock with a prescaler division by 4.
 *
 * ** Summary **
 *
 * To use this driver, first call adc_init(), then call
 * required configuration functions, then call adc_enable(),
 * and finally adc_start().
 *
 * ** Call order **
 *
 * All configuration functions provided by this driver must
 * be called (if required) before calling adc_enable().
 * If addtionnal configuration not provided by this driver
 * is required, this must be done after calling adc_enable(),
 * but before calling adc_start().
 */


// Current file header
#include "adc_core.h"

// STM32 LL
#include <stm32g4xx_ll_bus.h>


/////
// Private functions

/**
 * ADC wake-up.
 * Refer to RM 21.4.6
 */
static void _adc_core_wakeup(ADC_TypeDef* adc)
{
    // Disable deep power down
    LL_ADC_DisableDeepPowerDown(adc);

    // Enable internal regulator
    LL_ADC_EnableInternalRegulator(adc);

    // Wait for ADC voltage regulator start-up time (20us for g474)
    // See also constant LL_ADC_DELAY_INTERNAL_REGUL_STAB_US
    k_busy_wait(30);
}

/**
 * ADC calibration.
 * Refer to RM 21.4.8
 */
static void _adc_core_calibrate(ADC_TypeDef* adc)
{
    // Single ended calibration
    LL_ADC_StartCalibration(adc, LL_ADC_SINGLE_ENDED);
    while ( LL_ADC_IsCalibrationOnGoing(adc) ) { /* Wait */ }

    // Seems to require an additionnal delay between calibrations
    // TODO: undocumented???
    k_busy_wait(10);

    // Differential ended calibration
    LL_ADC_StartCalibration(adc, LL_ADC_DIFFERENTIAL_ENDED);
    while ( LL_ADC_IsCalibrationOnGoing(adc) ) { /* Wait */ }
}


/////
// Public API

/**
 * ADC differential channel set-up:
 * Applies differential mode to specified channel.
 * Must be done *before* enabling ADC.
 * Refer to RM 21.4.7
 */
void adc_core_set_channel_differential(ADC_TypeDef* adc, uint8_t channel)
{
    LL_ADC_SetChannelSingleDiff(adc,
                                __LL_ADC_DECIMAL_NB_TO_CHANNEL(channel),
                                LL_ADC_DIFFERENTIAL_ENDED
                               );
}

/**
 * ADC internal paths configuration.
 * Enables VTS, VBAT and VREF.
 * Refer to RM 21.4.31 to 21.4.33
 */
void adc_core_configure_internal_paths(uint8_t vts, uint8_t vbat, uint8_t vref)
{
    uint32_t path = LL_ADC_PATH_INTERNAL_NONE;
    if (vts == 1)
        path |= LL_ADC_PATH_INTERNAL_TEMPSENSOR;
    if (vbat == 1)
        path |= LL_ADC_PATH_INTERNAL_VBAT;
    if (vref == 1)
        path |= LL_ADC_PATH_INTERNAL_VREFINT;

    LL_ADC_SetCommonPathInternalCh(ADC12_COMMON, path);
}

/**
 * ADC dual mode: enables ADC 1/ADC 2 synchronization.
 * When ADC 1 acquisition is triggered, it simultaneously
 * triggers an acquisition on ADC 2.
 * Refer to RM 21.4.30
 */
void adc_core_set_dual_mode()
{
    LL_ADC_SetMultimode(ADC12_COMMON, LL_ADC_MULTI_DUAL_REG_SIMULT);
}

/**
 * ADC enable.
 * Refer to RM 21.4.9
 */
void adc_core_enable(ADC_TypeDef* adc)
{
    // Enable ADC and wait for it to be ready
    LL_ADC_ClearFlag_ADRDY(adc);
    LL_ADC_Enable(adc);
    while (LL_ADC_IsActiveFlag_ADRDY(adc) == 0) { /* Wait */ }
}

/**
 * ADC start.
 * Refer to RM 21.4.15
 */
void adc_core_start(ADC_TypeDef* adc)
{
    LL_ADC_REG_StartConversion(adc);
}

/**
 * ADC initialization procedure for
 * both ADC 1 and ADC 2.
 */
void adc_core_init()
{
    // Enable ADC1/2 clock
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);

    // Wake-up ADCs
    _adc_core_wakeup(ADC1);
    _adc_core_wakeup(ADC2);

    // Set common clock
    // Refer to RM 21.4.3 and 21.7.2
    LL_ADC_SetCommonClock(ADC12_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV4);

    // Calibrate ADCs
    _adc_core_calibrate(ADC1);
    _adc_core_calibrate(ADC2);
}
