/*
 * Copyright (c) 2023-present LAAS-CNRS
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

/*
 * @date 2024
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 * @author Jean Alinei <jean.alinei@owntech.org
 *
 * @brief This file is based on the device tree to initialize legs controlled
 * 		  by the HRTIM
 */

#ifndef POWER_H_
#define POWER_H_

#include <zephyr/kernel.h>
#include "arm_math.h"
#include "hrtim_enum.h"

#define LEG_TOKEN(node_id) DT_STRING_TOKEN(node_id, leg_name),

/**  @brief Parses all the legs with okay status in the device tree and fills 
 * 			this type def. 
 * 
 * 			- `LEG1` to `LEG5` - values supported by the SPIN
 * 
 * 			- `ALL` - Applies the function to all legs in the list 
 * 
*/
typedef enum
{
	DT_FOREACH_CHILD_STATUS_OKAY(DT_NODELABEL(powershield), LEG_TOKEN)
	ALL
} leg_t;

class PowerAPI
{
private:
	/* return timing unit from spin pin number */
	hrtim_tu_number_t spinNumberToTu(uint16_t spin_number);


public:
	/**
	 * @brief Initialize the power mode for a given leg.
	 *
	 * This function configures modulation, frequency, phase shift, dead time,
	 * and other parameters for power control of a specified leg.
	 *
	 * @param leg The leg to initialize:`LEG1` to `ALL`
	 * @param leg_convention PWM Switch to be driven by the duty cycle: 
	 * 						 `PWMx1`, `PWMx2`
	 * @param leg_mode PWM mode: `VOLTAGE_MODE`, `CURRENT_MODE`
	 */
	void initMode(leg_t leg,
				  hrtim_switch_convention_t leg_convention,
				  hrtim_pwm_mode_t leg_mode);

	/**
	 * @brief Set the duty cycle for a specific leg's power control.
	 *
	 * This function sets the duty cycle for the power control of a specified 
	 * leg. 
	 * 
	 * The duty cycle determines the ON/OFF ratio of the power signal for the 
	 * leg.
	 *
	 * @param leg The leg for which to set the duty cycle: `LEG1` to `ALL` 
	 * @param duty_value The duty cycle value to set
	 * 				   (a floating-point number between `0.1` and `0.9`).
	 */
	void setDutyCycle(leg_t leg, float32_t duty_value);

	/**
	 * @brief Set the duty cycle for a specific leg's power control.
	 *
	 * This function sets the duty cycle for the power control of a specified 
	 * leg. 
	 * 
	 * The duty cycle determines the ON/OFF time ratio of the power signal 
	 * for the leg.
	 *
	 * @param leg The leg for which to set the duty cycle: `LEG1` to `ALL` 
	 * @param duty_value The duty cycle value to set an unsigned integer number 
	 * 				   between `duty_cycle_min_raw` and `duty_cycle_max_raw`.
	 */
	void setDutyCycleRaw(leg_t leg, uint16_t duty_value);


	/**
	 * @brief Start power output for a specific leg.
	 *
	 * This function initiates the power output for a specified leg by starting
	 * the PWM subunits associated with the leg's output pins. 
	 * 
	 * If the leg's output pins are not declared inactive in the device tree, 
	 * the corresponding subunits are started. eg. 
	 * 
	 * If output1 is declared inactive in the device tree, PWMA1 will not start.
	 *
	 * @param leg The leg for which to start the power output: `LEG1` to `ALL`
	 */
	void start(leg_t leg);

	/**
	 * @brief Stop power output for a specific leg.
	 *
	 * @param leg The leg for which to stop the power output: `LEG1` to `ALL`
	 */
	void stop(leg_t leg);

	/**
	 * @brief Connect the electrolytic capacitor.
	 *
	 * @param leg The selected leg for which the capacitor will be connected:
	 * 		  `LEG1` to `ALL`
	 *
	 * @warning This function can only be called AFTER initializing the `LEG`.
	 */
	void connectCapacitor(leg_t leg);

	/**
	 * @brief Disconnect the electrolytic capacitor.
	 *
	 * @param leg The selected leg for which the capacitor will be disconnected:
	 * 			  `LEG1` to `ALL`
	 *
	 * @warning This function can only be called AFTER initializing the `LEG`.
	 */
	void disconnectCapacitor(leg_t leg);

	/**
	 * @brief Turns the driver of the leg ON. This enables control over the
	 * 		  power switches.
	 *
	 * @param leg The leg for which the driver will be turned on: `LEG1` to `ALL`
	 *
	 * @warning This function can only be called AFTER initializing the LEG.
	 */
	void connectDriver(leg_t leg);

	/**
	 * @brief Turns the driver of the leg OFF. 
	 * 
	 * 		  This disables control over the power switches.
	 *
	 * @param leg The leg for which the driver will be turned off: `LEG1` to `ALL`
	 *
	 * @warning This function can only be called AFTER initializing the LEG.
	 */
	void disconnectDriver(leg_t leg);

	/**
	 * @brief Set the trigger value for a specific leg's ADC trigger.
	 *
	 * This function sets the trigger value for the ADC trigger of a specified
	 * leg. 
	 * 
	 * The trigger value determines the level at which the ADC trigger is 
	 * activated.
	 *
	 * @param leg The leg for which to set the ADC trigger value: `LEG1` to `ALL`
	 * @param trigger_value The trigger value to set between 0.05 and 0.95.
	 */
	void setTriggerValue(leg_t leg, float32_t trigger_value);

	/**
	 * @brief Set the phase shift value for a specific leg's power control.
	 *
	 * @param leg The leg for which to set the phase shift value: `LEG1` to `ALL`
     * @param[in] phase_shift	phase shift value in degrees 
	 * 							between `-360` and `360`
	 */
	void setPhaseShift(leg_t leg, int16_t phase_shift);

	/**
	 * @brief Set the slope compensation in current mode for a leg
	 *
	 * @param leg The leg to initialize: `LEG1` to `ALL`
	 * @param set_voltage in volt, the DAC peak voltage for slope compensation
	 * 					  (value between `0` and `2.048`)
	 * @param reset_voltage in volt, the DAC lowest voltage for
	 * 						slope compensation (value between `0` and `2.048`)
	 *
	 * @warning Be sure that `set_voltage > reset_voltage`
	 */
	void setSlopeCompensation(leg_t leg,
							  float32_t set_voltage,
							  float32_t reset_voltage);

	/**
	 * @brief set the dead time value for a leg
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `ALL`
	 * @param ns_rising_dt  rising dead time value in nanoseconds
	 * @param ns_falling_dt falling dead time value in nanoseconds
	 */
	void setDeadTime(leg_t leg,
					 uint16_t ns_rising_dt,
					 uint16_t ns_falling_dt);

	/**
	 * @brief sets the Minimum Duty Cycle Limit
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `ALL`
	 * @param duty_cycle    - new minimum duty cycle value between 0.0 and 1.0
	*/
	void setDutyCycleMin(leg_t leg, float32_t duty_cycle);

	/**
	 * @brief sets the Minimum Duty Cycle Limit as an unsigned int
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `ALL`
	 * @param duty_cycle - new minimum duty cycle value between `0` and `period`
	*/
	void setDutyCycleMinRaw(leg_t leg, uint16_t duty_cycle);

	/**
	 * @brief sets the Maximum Duty Cycle Limit as an unsigned int
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `ALL`
	 * @param duty_cycle - new minimum duty cycle value between `0` and `period`
	*/
	void setDutyCycleMaxRaw(leg_t leg, uint16_t duty_cycle);

	/**
	 * @brief sets the Maximum Duty Cycle Limit
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `ALL`
	 * @param duty_cycle    - new maximum duty cycle value between 0.0 and 1.0
	*/
	void setDutyCycleMax(leg_t leg, float32_t duty_cycle);

	/**
	 * @brief gets the Maximum Duty Cycle Limit as a float
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `LEG5`.
	 * @warning `ALL` is NOT supported !
	*/
	float32_t getDutyCycleMax(leg_t leg);

	/**
	 * @brief gets the Maximum Duty Cycle Limit as an unsigned integer.
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `LEG5`.
	 * @warning `ALL` is NOT supported !
	*/
	uint16_t getDutyCycleMaxRaw(leg_t leg);

	/**
	 * @brief gets the Minimum Duty Cycle Limit as a float
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `LEG5`.
	 * @warning `ALL` is NOT supported !
	*/
	float32_t getDutyCycleMin(leg_t leg);

	/**
	 * @brief gets the Minimum Duty Cycle Limit as an unsigned integer.
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `LEG5`.
	 * @warning `ALL` is NOT supported !
	*/
	uint16_t getDutyCycleMinRaw(leg_t leg);

	/**
	 * @brief returns the value of the leg period as an unsigned integer
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `LEG5`.
	 * @warning `ALL` is NOT supported !
	*/
	uint16_t getPeriod(leg_t leg);


	/**
	 * @brief Sets ADC decimator for a leg
	 *
	 * This function sets the number of event which will be ignored between
	 * two events. ie. you divide the number of trigger in a fixed period.
	 * 
	 * For example if adc_decim = 1, nothing changes but with adc_decims = 2
	 * you have twice less adc trigger.
	 *
	 * @param leg leg for which to set adc decimator value: `LEG1` to `ALL`
	 * @param adc_decim adc decimator, a number between 1 and 32
	 *
	 */
	void setAdcDecim(leg_t leg, uint16_t adc_decim);

	/**
	 * @brief Initialise a leg for buck topology
	 *
	 * @param leg Leg to initialize: `LEG1` to `ALL`
	 * @param leg_mode PWM mode - `VOLTAGE_MODE` or `CURRENT_MODE`
	 * 
	 * @note `CURRENT_MODE` only available to `LEG1` and `LEG2` for the 
	 * 	      Twist board
	 *
	 */
	void initBuck(leg_t leg, hrtim_pwm_mode_t leg_mode = VOLTAGE_MODE);

	/**
	 * @brief Initialise all the legs for boost topology, current mode is not
	 * supported for boost.
	 *
	 * @param leg Leg to initialize: `LEG1` to `ALL`
	 */
	void initBoost(leg_t leg);

};

#endif /* POWER_H_ */
