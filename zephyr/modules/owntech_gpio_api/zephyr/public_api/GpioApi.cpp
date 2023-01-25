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
 * @brief  Owntech GPIO API
 * @date   2023
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#include "GpioApi.h"

GpioApi gpio;

const struct device* const GPIO_A = DEVICE_DT_GET(DT_NODELABEL(gpioa));
const struct device* const GPIO_B = DEVICE_DT_GET(DT_NODELABEL(gpiob));
const struct device* const GPIO_C = DEVICE_DT_GET(DT_NODELABEL(gpioc));
const struct device* const GPIO_D = DEVICE_DT_GET(DT_NODELABEL(gpiod));

const gpio_flags_t INPUT        = GPIO_INPUT;
const gpio_flags_t INPUT_PULLUP = GPIO_INPUT | GPIO_PULL_UP;
const gpio_flags_t OUTPUT       = GPIO_OUTPUT;


void GpioApi::configurePin(pin_t pin, gpio_flags_t flags)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		gpio_pin_configure(port, pin_number, flags);
	}
}

void GpioApi::setPin(pin_t pin)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		gpio_pin_set(port, pin_number, 1);
	}
}

void GpioApi::resetPin(pin_t pin)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		gpio_pin_set(port, pin_number, 0);
	}
}

void GpioApi::togglePin(pin_t pin)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		gpio_pin_toggle(port, pin_number);
	}
}

void GpioApi::writePin(pin_t pin, uint8_t value)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		gpio_pin_set(port, pin_number, value);
	}
}

uint8_t GpioApi::readPin(pin_t pin)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		return (uint8_t) gpio_pin_get(port, pin_number);
	}

	return 0;
}

gpio_pin_t GpioApi::getPinNumber(pin_t pin)
{
	return (((uint8_t)pin) & 0x0F) + 1;
}

const struct device* GpioApi::getGpioDevice(pin_t pin)
{
	uint8_t deviceNumber = ((uint8_t)pin) & 0xF0;
	switch (deviceNumber)
	{
		case PA:
			return GPIO_A;
			break;
		case PB:
			return GPIO_B;
			break;
		case PC:
			return GPIO_C;
			break;
		case PD:
			return GPIO_D;
			break;
	}

	return nullptr;
}
