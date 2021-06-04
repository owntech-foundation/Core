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
 * @brief   ADC driver for stm32g474re
 *
 * @author  Hugues Larrive <hugues.larrive@laas.fr>
 * @author  Clément Foucher <clement.foucher@laas.fr>
 */


// Stdlib
#include <stdlib.h>

// Current file header
#include "adc_channels_private.h"

// OwnTech API
#include "adc_helper.h"
#include "adc_core.h"


/////
// Variables

static channel_prop_t channels_props[] =
{
    DT_FOREACH_CHILD(ADC_INPUTS_NODELABEL, CHANNEL_WRITE_PROP)
};

static uint8_t channels_in_adc1_count;
static uint8_t channels_in_adc2_count;

static channel_prop_t** adc1_channels_list;
static channel_prop_t** adc2_channels_list;


/////
// ADC channels private functions

/**
 * Counts device-tree configured channels in each ADC.
 */
static void _adc_channels_count()
{
    // Count total number of channels
    channels_in_adc1_count = 0;
    channels_in_adc2_count = 0;

    for (int i = 0 ; i < CHANNEL_COUNT ; i++)
    {
        ADC_TypeDef* adc = _get_adc_by_name(channels_props[i].adc);
        if (adc == ADC1)
        {
            channels_in_adc1_count++;
        }
        else if (adc == ADC2)
        {
            channels_in_adc2_count++;
        }
    }

    // Build a list of channels by ADC
    adc1_channels_list = malloc(sizeof(channel_prop_t*) * channels_in_adc1_count);
    adc2_channels_list = malloc(sizeof(channel_prop_t*) * channels_in_adc2_count);

    int adc1_index = 0;
    int adc2_index = 0;
    for (int i = 0 ; i < CHANNEL_COUNT ; i++)
    {
        ADC_TypeDef* adc = _get_adc_by_name(channels_props[i].adc);
        if (adc == ADC1)
        {
            adc1_channels_list[adc1_index] = &channels_props[i];
            adc1_index++;
        }
        else if (adc == ADC2)
        {
            adc2_channels_list[adc2_index] = &channels_props[i];
            adc2_index++;
        }
    }
}

/**
 * Differential channel setup.
 * Must be done before ADC is enabled.
 */
static void _adc_channels_differential_setup()
{
    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        if (channels_props[i].is_differential)
        {
            ADC_TypeDef* adc = _get_adc_by_name(channels_props[i].adc);
            if (adc != NULL)
            {
                adc_core_set_channel_differential(adc, channels_props[i].number);
            }
        }
    }
}

/**
 * Internal path setup.
 * Currently done before ADC is enabled,
 * but is is correct? No hint in RM on this.
 */
void _adc_channels_internal_path_setup()
{
    uint8_t vts  = 0;
    uint8_t vbat = 0;
    uint8_t vref = 0;

    for (int i = 0 ; i < CHANNEL_COUNT ; i++)
    {
        if (channels_props[i].number == __LL_ADC_CHANNEL_TO_DECIMAL_NB(LL_ADC_CHANNEL_TEMPSENSOR_ADC1))
            vts = 1;
        if (channels_props[i].number == __LL_ADC_CHANNEL_TO_DECIMAL_NB(LL_ADC_CHANNEL_VBAT))
            vbat = 1;
        if (channels_props[i].number == __LL_ADC_CHANNEL_TO_DECIMAL_NB(LL_ADC_CHANNEL_VREFINT))
            vref = 1;
    }

    adc_core_configure_internal_paths(vts, vbat, vref);
}


/////
// ADC channls public functions

/**
 * Performs internal data structures initialization
 * and pre-ADC enable init.
 * Must be called before adc_core_enable()
 */
void adc_channels_init()
{
    _adc_channels_count();
    _adc_channels_differential_setup();
    _adc_channels_internal_path_setup();
}

/**
 * ADC channel configuration.
 * Sets sequencer ranks and channels sampling time.
 */
void adc_channels_configure(ADC_TypeDef* adc)
{
    uint8_t channel;
    uint8_t rank = 1;
    ADC_TypeDef* channel_adc;

    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        channel = channels_props[i].number;
        channel_adc = _get_adc_by_name(channels_props[i].adc);
        // Regular sequencer
        if (channel_adc == adc)
        {
            LL_ADC_REG_SetSequencerRanks(adc,
                                         adc_decimal_nb_to_rank(rank),
                                         __LL_ADC_DECIMAL_NB_TO_CHANNEL(channel)
                                        );
            rank++;

            // Channels sampling time

            /* 000: 2.5 ADC clock cycles
             * 001: 6.5 ADC clock cycles
             * 010: 12.5 ADC clock cycles
             * 011: 24.5 ADC clock cycles
             * 100: 47.5 ADC clock cycles
             * 101: 92.5 ADC clock cycles
             * 110: 247.5 ADC clock cycles
             * 111: 640.5 ADC clock cycles
             */
            /* Vrefint minimum sampling time : 4us
             */
            /* Vts minimum sampling time : 5us
             */
            /* For 0b110:
             * Tadc_clk = 1 / 42.5 MHz = 23.5 ns
             * Tsar = 12.5 * Tadc_clk = 293.75 ns
             * Tsmpl = 247.5 * Tadc_clk = 5816.25 ns
             * Tconv = Tsmpl + Tsar = 6.11 us
             * -> Fconv up to 163.6 KSPS for 1 channel per ADC
             * Fconv up to 27.2 KSPS with the 6 channels actally
             * used on the ADC1
             *
             * For 0b001 (ok for voltage):
             * Tadc_clk = 1 / 42.5 MHz = 23.5 ns
             * Tsar = 12.5 * Tadc_clk = 293.75 ns
             * Tsmpl = 6.5 * Tadc_clk = 152.75 ns
             * Tconv = Tsmpl + Tsar = 446.4 ns
             * -> Fconv up to 2239 KSPS for 1 channel per ADC
             * Fconv up to 373 KSPS with the 6 channels actally
             * used on the ADC1
             *
             * For 0b101 (ok for current):
             * Tadc_clk = 1 / 42.5 MHz = 23.5 ns
             * Tsar = 12.5 * Tadc_clk = 293.75 ns
             * Tsmpl = 92.5 * Tadc_clk = 2173.75 ns
             * Tconv = Tsmpl + Tsar = 2.47 µs
             * -> Fconv up to 404 KSPS for 1 channel per ADC
             * Fconv up to 134 KSPS for 3 channels actally
             * used on each ADC
             */
            LL_ADC_SetChannelSamplingTime(adc,
                                          __LL_ADC_DECIMAL_NB_TO_CHANNEL(channel),
                                          LL_ADC_SAMPLINGTIME_92CYCLES_5
                                         );
        }
    }
}

char* adc_channels_get_channel_name(uint8_t adc_num, uint8_t channel_rank)
{
    if (adc_num == 1)
        return adc1_channels_list[channel_rank]->name;
    else if (adc_num == 2)
        return adc2_channels_list[channel_rank]->name;
    else
        return NULL;
}

uint8_t adc_channels_get_channels_count(uint8_t adc_num)
{
    if (adc_num == 1)
        return channels_in_adc1_count;
    else if (adc_num == 2)
        return channels_in_adc2_count;
    else
        return 0xFF;
}