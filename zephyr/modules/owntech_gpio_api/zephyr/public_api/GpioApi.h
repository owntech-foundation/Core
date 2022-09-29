/*
 * Copyright (c) 2022 LAAS-CNRS
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
 * @date   2022
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef GPIOAPI_H_
#define GPIOAPI_H_

#include <drivers/gpio.h>

extern const struct device* const GPIO_A;
extern const struct device* const GPIO_B;
extern const struct device* const GPIO_C;
extern const struct device* const GPIO_D;

const gpio_flags_t INPUT        = GPIO_INPUT;
const gpio_flags_t INPUT_PULLUP = GPIO_INPUT | GPIO_PULL_UP;
const gpio_flags_t OUTPUT       = GPIO_OUTPUT;

const uint8_t PA = 0x00;
const uint8_t PB = 0x10;
const uint8_t PC = 0x20;
const uint8_t PD = 0x30;

const uint8_t P1  = 0x0;
const uint8_t P2  = 0x1;
const uint8_t P3  = 0x2;
const uint8_t P4  = 0x3;
const uint8_t P5  = 0x4;
const uint8_t P6  = 0x5;
const uint8_t P7  = 0x6;
const uint8_t P8  = 0x7;
const uint8_t P9  = 0x8;
const uint8_t P10 = 0x9;
const uint8_t P11 = 0xA;
const uint8_t P12 = 0xB;
const uint8_t P13 = 0xC;
const uint8_t P14 = 0xD;
const uint8_t P15 = 0xE;
const uint8_t P16 = 0xF;

typedef enum
{
	PA1  = PA | P1,
	PA2  = PA | P2,
	PA3  = PA | P3,
	PA4  = PA | P4,
	PA5  = PA | P5,
	PA6  = PA | P6,
	PA7  = PA | P7,
	PA8  = PA | P8,
	PA9  = PA | P9,
	PA10 = PA | P10,
	PA11 = PA | P11,
	PA12 = PA | P12,
	PA13 = PA | P13,
	PA14 = PA | P14,
	PA15 = PA | P15,
	PA16 = PA | P16,
	PB1  = PB | P1,
	PB2  = PB | P2,
	PB3  = PB | P3,
	PB4  = PB | P4,
	PB5  = PB | P5,
	PB6  = PB | P6,
	PB7  = PB | P7,
	PB8  = PB | P8,
	PB9  = PB | P9,
	PB10 = PB | P10,
	PB11 = PB | P11,
	PB12 = PB | P12,
	PB13 = PB | P13,
	PB14 = PB | P14,
	PB15 = PB | P15,
	PB16 = PB | P16,
	PC1  = PC | P1,
	PC2  = PC | P2,
	PC3  = PC | P3,
	PC4  = PC | P4,
	PC5  = PC | P5,
	PC6  = PC | P6,
	PC7  = PC | P7,
	PC8  = PC | P8,
	PC9  = PC | P9,
	PC10 = PC | P10,
	PC11 = PC | P11,
	PC12 = PC | P12,
	PC13 = PC | P13,
	PC14 = PC | P14,
	PC15 = PC | P15,
	PC16 = PC | P16,
	PD1  = PD | P1,
	PD2  = PD | P2,
	PD3  = PD | P3
} pin_t;

class GpioApi
{
public:
	void configurePin(pin_t pin, gpio_flags_t flags);
	void setPin(pin_t pin);
	void resetPin(pin_t pin);
	void togglePin(pin_t pin);
	void writePin(pin_t pin, uint8_t value);
	uint8_t readPin(pin_t pin);

private:
	gpio_pin_t getPinNumber(pin_t pin);
	const struct device* getGpioDevice(pin_t pin);
};

extern GpioApi gpio;

#endif // GPIOAPI_H_
