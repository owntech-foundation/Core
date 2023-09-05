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


void GpioApi::configurePin(uint8_t pin, gpio_flags_t flags)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		gpio_pin_configure(port, pin_number, flags);
	}
}

void GpioApi::setPin(uint8_t pin)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		gpio_pin_set(port, pin_number, 1);
	}
}

void GpioApi::resetPin(uint8_t pin)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		gpio_pin_set(port, pin_number, 0);
	}
}

void GpioApi::togglePin(uint8_t pin)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		gpio_pin_toggle(port, pin_number);
	}
}

void GpioApi::writePin(uint8_t pin, uint8_t value)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		gpio_pin_set(port, pin_number, value);
	}
}

uint8_t GpioApi::readPin(uint8_t pin)
{
	gpio_pin_t pin_number = this->getPinNumber(pin);
	const struct device* port = this->getGpioDevice(pin);
	if (port != nullptr)
	{
		return (uint8_t) gpio_pin_get(port, pin_number);
	}

	return 0;
}

gpio_pin_t GpioApi::getPinNumber(uint8_t pin)
{
	if ( (pin & 0x80) != 0) // Nucleo format
	{
		return (((uint8_t)pin) & 0x0F);
	}
	else // Pin number
	{
		if      (pin == 1)  return 11;
		else if (pin == 2)  return 12;
		else if (pin == 4)  return 13;
		else if (pin == 5)  return 14;
		else if (pin == 6)  return 15;
		else if (pin == 7)  return 6;
		else if (pin == 9)  return 7;
		else if (pin == 10) return 8;
		else if (pin == 11) return 9;
		else if (pin == 12) return 8;
		else if (pin == 14) return 9;
		else if (pin == 15) return 10;
		else if (pin == 16) return 10;
		else if (pin == 17) return 11;
		else if (pin == 19) return 12;
		else if (pin == 20) return 4;
		else if (pin == 21) return 9;
		else if (pin == 22) return 13;
		else if (pin == 24) return 0;
		else if (pin == 25) return 1;
		else if (pin == 26) return 2;
		else if (pin == 27) return 3;
		else if (pin == 29) return 0;
		else if (pin == 30) return 1;
		else if (pin == 31) return 0;
		else if (pin == 32) return 5;
		else if (pin == 34) return 6;
		else if (pin == 35) return 4;
		else if (pin == 37) return 1;
		else if (pin == 41) return 10;
		else if (pin == 42) return 2;
		else if (pin == 43) return 5;
		else if (pin == 44) return 7;
		else if (pin == 45) return 4;
		else if (pin == 46) return 13;
		else if (pin == 47) return 14;
		else if (pin == 48) return 15;
		else if (pin == 49) return 2;
		else if (pin == 50) return 3;
		else if (pin == 51) return 2;
		else if (pin == 52) return 3;
		else if (pin == 53) return 5;
		else if (pin == 55) return 6;
		else if (pin == 56) return 7;
		else if (pin == 58) return 8;
	}
	return 0xFF;
}

const struct device* GpioApi::getGpioDevice(uint8_t pin)
{
	if ( (pin & 0x80) != 0) // Nucleo format
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
	}
	else // Pin number
	{
		switch (pin)
		{
			case 12:
			case 14:
			case 15:
			case 29:
			case 30:
			case 32:
			case 34:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
			case 51:
			case 52:
				return GPIO_A;
				break;
			case 1:
			case 2:
			case 4:
			case 5:
			case 6:
			case 20:
			case 21:
			case 31:
			case 37:
			case 41:
			case 42:
			case 50:
			case 53:
			case 55:
			case 56:
			case 58:
				return GPIO_B;
				break;
			case 7:
			case 9:
			case 10:
			case 11:
			case 16:
			case 17:
			case 19:
			case 22:
			case 24:
			case 25:
			case 26:
			case 27:
			case 35:
			case 43:
				return GPIO_C;
				break;
			case 49:
				return GPIO_D;
				break;
		}
	}

	return nullptr;
}
