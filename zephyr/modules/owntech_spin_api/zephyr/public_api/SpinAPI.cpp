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
 * @author Jean Alinei <jean.alinei@owntech.org>
 */


// Current class header
#include "SpinAPI.h"


SpinAPI spin;

#ifdef CONFIG_OWNTECH_GPIO_API
GpioHAL SpinAPI::gpio;
#endif

LedHAL SpinAPI::led;

DacHAL SpinAPI::dac;

DataAPI SpinAPI::data;

CompHAL SpinAPI::comp;

PwmHAL SpinAPI::pwm;

#ifdef CONFIG_OWNTECH_UART_API
UartHAL SpinAPI::uart;
#endif

TimerHAL SpinAPI::timer;
