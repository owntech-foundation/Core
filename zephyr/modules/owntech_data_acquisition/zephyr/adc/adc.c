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
 */


// STM32 LL
#include <stm32g4xx_ll_adc.h>

// OwnTech API
#include "adc_channels.h"
#include "adc_core.h"


/////
// Private functions

/**
 * ADC 1 OwnTech's specific configuration.
 */
static void _adc_configure_adc_1()
{
    // Set regular sequence length
    LL_ADC_REG_SetSequencerLength(ADC1, adc_channels_get_channels_count(1)-1);

    // TO TEST
    // Set discontinuous mode: first event triggers first channel conversion,
    // next event will convert the next channel in the sequence, and so on
    // until all channels are converted, then restart from fisrt channel.
    // RM: 21.4.20
    //LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_1RANK);

    // Enable dma and circular mode
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

    /////
    // Set trigger source: only for ADC 1 as we operate in dual mode
    // (ADC 2 triggered by ADC 1)

    // Enable external trigger on hrtim_adc_trg1
    LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

    // RM Table 163. adc_ext_trg21 hrtim_adc_trg1 EXTSEL = 0x10101
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_EXT_HRTIM_TRG1);
}

/**
 * ADC 2 OwnTech's specific configuration.
 */
static void _adc_configure_adc_2()
{
    // Set regular sequence length
    LL_ADC_REG_SetSequencerLength(ADC2, adc_channels_get_channels_count(2)-1);

    // TO TEST
    // Set discontinuous mode: first event triggers first channel conversion,
    // next event will convert the next channel in the sequence, and so on
    // until all channels are converted, then restart from fisrt channel.
    // RM: 21.4.20
    //LL_ADC_REG_SetSequencerDiscont(ADC2, LL_ADC_REG_SEQ_DISCONT_1RANK);

    // Enable dma and circular mode
    LL_ADC_REG_SetDMATransfer(ADC2, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
}


/////
// Public API

/**
 * Configure ADC and DMA according to OwnTech
 * board requirements.
 */
void adc_init()
{
    // Initialize ADC
    adc_core_init();
    adc_core_set_dual_mode();

    // Initialize channels
    adc_channels_init();

    // Enable ADC
    adc_core_enable(ADC1);
    adc_core_enable(ADC2);

    // Perform post-enable ADC configuration
    adc_channels_configure(ADC1);
    adc_channels_configure(ADC2);

    _adc_configure_adc_1();
    _adc_configure_adc_2();

    // Finally, start ADCs
    adc_core_start(ADC1);
    adc_core_start(ADC2);
}
