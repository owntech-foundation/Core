/*
 * Copyright (c) 2024 LAAS-CNRS
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
 * @date   2024
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


/* Header */
#include "AnalogCommunication.h"

/* OwnTech Power API */
#include "ShieldAPI.h"
#include "SpinAPI.h"

/* LL drivers */
#include "stm32_ll_gpio.h"

#define ADC_NUM 2
#define DAC_NUM 2
#define DAC_CHAN 1

void AnalogCommunication::init()
{
    /* Initialize the GPIO PC4 (pin number 35) to analog mode
       to use the ADC */
    LL_GPIO_SetPinMode      (GPIOC, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinSpeed     (GPIOC, LL_GPIO_PIN_4, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_4, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull      (GPIOC, LL_GPIO_PIN_4, LL_GPIO_PULL_NO);

    shield.sensors.enableShieldChannel(ADC_NUM, ANALOG_COMM);

    /* Initialize the DAC */
    spin.dac.initConstValue(ADC_NUM);
    spin.dac.setConstValue(DAC_NUM, DAC_CHAN, 0);
}

float32_t AnalogCommunication::getAnalogCommValue()
{
    float32_t ret =  shield.sensors.getLatest(ANALOG_COMM);
    return ret;
}

void AnalogCommunication::setAnalogCommValue(uint32_t analog_bus_value)
{
    spin.dac.setConstValue(DAC_NUM, DAC_CHAN, analog_bus_value);
}
