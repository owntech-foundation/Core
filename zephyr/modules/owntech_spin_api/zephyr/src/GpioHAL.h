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

/**
 * @brief  Owntech GPIO API
 * @date   2023
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef GPIOHAL_H_
#define GPIOHAL_H_

#include <zephyr/drivers/gpio.h>


/**
 *  Public constants
 */

extern const struct device* const GPIO_A;
extern const struct device* const GPIO_B;
extern const struct device* const GPIO_C;
extern const struct device* const GPIO_D;

extern const gpio_flags_t INPUT;
extern const gpio_flags_t INPUT_PULLUP;
extern const gpio_flags_t OUTPUT;

/* 0x80 is used to indicate nucleo-style pin */
static const uint8_t PA = 0x80 | 0x00;
static const uint8_t PB = 0x80 | 0x10;
static const uint8_t PC = 0x80 | 0x20;
static const uint8_t PD = 0x80 | 0x30;

static const uint8_t P0  = 0x0;
static const uint8_t P1  = 0x1;
static const uint8_t P2  = 0x2;
static const uint8_t P3  = 0x3;
static const uint8_t P4  = 0x4;
static const uint8_t P5  = 0x5;
static const uint8_t P6  = 0x6;
static const uint8_t P7  = 0x7;
static const uint8_t P8  = 0x8;
static const uint8_t P9  = 0x9;
static const uint8_t P10 = 0xA;
static const uint8_t P11 = 0xB;
static const uint8_t P12 = 0xC;
static const uint8_t P13 = 0xD;
static const uint8_t P14 = 0xE;
static const uint8_t P15 = 0xF;


/**
 *  Public types
 */

typedef enum : uint8_t
{
	PA0  = PA | P0,
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
	PB0  = PB | P0,
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
	PC0  = PC | P0,
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
	PD0  = PD | P0,
	PD1  = PD | P1,
	PD2  = PD | P2,
	PD3  = PD | P3
} pin_t;


/**
 *  Class definition
 */

class GpioHAL
{
public:

	/**
	 * @brief Configure an I/O pin. This must be done
	 *        prior to accessing any other function
	 *        from this API on the pin.
	 *
	 * @param pin Number of pin. Format allowed: 
	 * 
	 * - the Spin pin number from 1 to 58
	 * 
	 * - STM32-style pin name from `PA1` to `PA15`, `PB1` to `PB15`, 
	 * 								`PC1` to `PC15` and `PD1` to `PD3`
	 *   
	 * @param flags Pin configuration flags. Authorized values: 
	 *              `INPUT`, `INPUT_PULLUP`, `OUTPUT`
	 */
	void configurePin(uint8_t pin, gpio_flags_t flags);

	/**
	 * @brief Set the value of a pin configured as output to 1.
	 *
	 * @param pin Number of pin. Format allowed: 
	 * 
	 * - the Spin pin number from 1 to 58
	 * 
	 * - STM32-style pin name from `PA1` to `PA15`, `PB1` to `PB15`, 
	 * 								`PC1` to `PC15` and `PD1` to `PD3`
	 *   
	 */
	void setPin(uint8_t pin);

	/**
	 * @brief Reset the value of a pin configured as output to 0.
	 *
	 * @param pin Number of pin. Format allowed: 
	 * 
	 * - the Spin pin number from 1 to 58
	 * 
	 * - STM32-style pin name from `PA1` to `PA15`, `PB1` to `PB15`, 
	 * 								`PC1` to `PC15` and `PD1` to `PD3`
	 *   
	 */
	void resetPin(uint8_t pin);

	/**
	 * @brief Toggle the value of a pin configured as output:
	 * 
	 *        - if pin value is `1`, it will be set to `0`
	 * 
	 *        - if pin value is `0`, it will be set to `1`.
	 *
	 * @param pin Number of pin. Format allowed: 
	 * 
	 * - the Spin pin number from 1 to 58
	 * 
	 * - STM32-style pin name from `PA1` to `PA15`, `PB1` to `PB15`, 
	 * 								`PC1` to `PC15` and `PD1` to `PD3`
	 *   
	 */
	void togglePin(uint8_t pin);

	/**
	 * @brief Set the value of a pin configured as output
	 *        to a given value.
	 *
	 * @param pin Number of pin. Format allowed: 
	 * 
	 * - the Spin pin number from 1 to 58
	 * 
	 * - STM32-style pin name from `PA1` to `PA15`, `PB1` to `PB15`, 
	 * 								`PC1` to `PC15` and `PD1` to `PD3`
	 *   
	 * @param value Value (0 or 1) to assign to the pin.
	 */
	void writePin(uint8_t pin, uint8_t value);

	/**
	 * @brief Get the current value of a pin configured as input.
	 *
	 * @param pin Number of pin. Format allowed: 
	 * 
	 * - the Spin pin number from 1 to 58
	 * 
	 * - STM32-style pin name from `PA1` to `PA15`, `PB1` to `PB15`, 
	 * 								`PC1` to `PC15` and `PD1` to `PD3`   
	 *   
	 * @return Current value (0 or 1) of the pin.
	 */
	uint8_t readPin(uint8_t pin);

private:
	/**
	 * @brief Get the GPIO pin number associated with a logical shield pin.
	 *
	 * This function translates a shield pin or encoded Nucleo-style pin into the
	 * corresponding MCU GPIO pin number (e.g., `PA11` → `11`).
	 *
	 * @param pin Number of pin. Format allowed: 
	 * 
	 * - the Spin pin number from 1 to 58
	 * 
	 * - STM32-style pin name from `PA1` to `PA15`, `PB1` to `PB15`, 
	 * 								`PC1` to `PC15` and `PD1` to `PD3`   
	 * 
	 * @return GPIO pin number (0–15) or 0xFF if unknown or unsupported.
	 */
	gpio_pin_t getPinNumber(uint8_t pin);


	/**
	 * @brief Get the GPIO port device corresponding to a logical shield pin.
	 *
	 * If the pin is encoded in Nucleo format (0x8X, 0x9X, 0xAX, etc.), the 
	 * high nibble determines the port (PA, PB, etc.). 
	 * 
	 * Otherwise, the function maps shield pins to their appropriate GPIO device
	 * (e.g., GPIO_A, GPIO_B).
	 *
	 * @param pin Number of pin. Format allowed: 
	 * 
	 * - the Spin pin number from 1 to 58
	 * 
	 * - STM32-style pin name from `PA1` to `PA15`, `PB1` to `PB15`, 
	 * 								`PC1` to `PC15` and `PD1` to `PD3`
	 *   
	 * @return Pointer to the corresponding Zephyr GPIO device, 
	 * 		   or nullptr if invalid.
	 */
	const struct device* getGpioDevice(uint8_t pin);

};

#endif /* GPIOHAL_H_ */
