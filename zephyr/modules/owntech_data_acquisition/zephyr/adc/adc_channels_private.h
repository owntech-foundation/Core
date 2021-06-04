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
 * @brief  This is the private include file for adc_channels.c.
 * It should only be included in C files from the current folder.
 */

#ifndef ADC_CHANNELS_PRIVATE_H_
#define ADC_CHANNELS_PRIVATE_H_


// Zephyr
#include <zephyr.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    char*   name;
    bool    is_differential;
    uint8_t number;
    char*   adc;
} channel_prop_t;

#define ADC_INPUTS_NODELABEL DT_NODELABEL(mychannels)

// Channel properties
#define CHANNEL_NAME(node_id)    DT_LABEL(node_id)
#define CHANNEL_IS_DIFF(node_id) DT_PROP(node_id, differential)
#define CHANNEL_NUMBER(node_id)  DT_PHA_BY_IDX(node_id, io_channels, 0, input)
#define CHANNEL_ADC(node_id)     DT_PROP_BY_PHANDLE_IDX(node_id, io_channels, 0, label)

#define CHANNEL_WRITE_PROP(node_id)                \
    {                                              \
        .name=CHANNEL_NAME(node_id),               \
        .is_differential=CHANNEL_IS_DIFF(node_id), \
        .number=CHANNEL_NUMBER(node_id),           \
        .adc=CHANNEL_ADC(node_id)                  \
    },

// Channel count
#define CHANNEL_COUNTER(node_id) +1
#define CHANNEL_COUNT DT_FOREACH_CHILD(ADC_INPUTS_NODELABEL, CHANNEL_COUNTER)


#ifdef __cplusplus
}
#endif

#endif // ADC_CHANNELS_PRIVATE_H_
