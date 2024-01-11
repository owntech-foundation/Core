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
 * @date 2023
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 *
 * @brief This file is based on the device tree to initialize legs controlled by the HRTIM
 */

#ifndef POWER_H_
#define POWER_H_

#include <zephyr/kernel.h>
#include "arm_math.h"
#include "hrtim_enum.h"

#define LEG_TOKEN(node_id) DT_STRING_TOKEN(node_id, leg_name),

/* All the leg with okay status in the device tree */
typedef enum
{
	DT_FOREACH_CHILD_STATUS_OKAY(DT_NODELABEL(power_shield), LEG_TOKEN)
} leg_t;

/* Enum to define the hardware version */
typedef enum
{
	shield_TWIST_V1_2 = 0,
	shield_TWIST_V1_3,
	shield_ownverter,
	shield_other,
} twist_version_t;

class TwistAPI
{
private:
	twist_version_t twist_version = shield_other; // shield version
	bool twist_init = false;						// check if shield version has been initalized or not

	hrtim_tu_number_t spinNumberToTu(uint16_t spin_number); // return timing unit from spin pin number

public:
	/**
	 * @brief Set the hardware version of the board.
	 *
	 * @param shield shield version - shield_TWIST_V1_2, shield_TWIST_V1_3, shield_ownverter or shield_other
	 *
	 * @warning You need to call this function BEFORE initializing a leg, after that it is not possible
	 *          to change the shield version
	 */
	void setVersion(twist_version_t twist_ver);

	/**
	 * @brief Initialize the power mode for a given leg.
	 *
	 * This function configures modulation, frequency, phase shift, dead time, and other parameters
	 * for power control of a specified leg.
	 *
	 * @param leg The leg to initialize
	 * @param leg_convention PWM Switch to be driven by the duty cycle. The other will be complementary - PWMx1 or PWMx2
	 * @param leg_mode PWM mode - VOLTAGE_MODE or CURRENT_MODE
	 */
	void initLegMode(leg_t leg, hrtim_switch_convention_t leg_convention, hrtim_pwm_mode_t leg_mode);

	/**
	 * @brief Initialize power modes for all legs.
	 *
	 * This function initializes the power modes  for each leg with the specified 'buck'
	 * and 'voltage_mode' settings.
	 *
	 * @param leg_convention PWM Switch to be driven by the duty cycle. The other will be complementary - PWMx1 or PWMx2
	 * @param leg_mode PWM mode - VOLTAGE_MODE or CURRENT_MODE
	 */
	void initAllMode(hrtim_switch_convention_t leg_convention, hrtim_pwm_mode_t leg_mode);


	/**
	 * @brief Set the duty cycle for a specific leg's power control.
	 *
	 * This function sets the duty cycle for the power control of a specified leg. The duty cycle
	 * determines the ON/OFF ratio of the power signal for the leg.
	 *
	 * @param leg The leg for which to set the duty cycle.
	 * @param duty_leg The duty cycle value to set (a floating-point number between 0.1 and 0.9).
	 */
	void setLegDutyCycle(leg_t leg, float32_t duty_leg);

	/**
	 * @brief Set the duty cycle for power control of all the legs.
	 *
	 * This function sets the same duty cycle for power control of all the legs. The duty cycle determines
	 * the ON/OFF ratio of the power signal for all legs.
	 *
	 * @param duty_all The duty cycle value to set (a floating-point number between 0.1 and 0.9).
	 */
	void setAllDutyCycle(float32_t duty_all);

	/**
	 * @brief Start power output for a specific leg.
	 *
	 * This function initiates the power output for a specified leg by starting the PWM subunits associated
	 * with the leg's output pins. If the leg's output pins are not declared inactive in the device tree,
	 * the corresponding subunits are started.
	 * eg. If output1 is declared inactive in the device tree, PWMA1 will not start
	 *
	 * @param leg The leg for which to start the power output.
	 */
	void startLeg(leg_t leg);


	/**
	 * @brief Start power output for all legs.
	 */
	void startAll();

	/**
	 * @brief Stop power output for a specific leg.
	 *
	 * @param leg The leg for which to stop the power output.
	 */
	void stopLeg(leg_t leg);

	/**
	 * @brief Stop power output for all legs.
	 */
	void stopAll();

	/**
	 * @brief Set the trigger value for a specific leg's ADC trigger.
	 *
	 * This function sets the trigger value for the ADC trigger of a specified leg. The trigger
	 * value determines the level at which the ADC trigger is activated.
	 *
	 * @param leg The leg for which to set the ADC trigger value.
	 * @param trigger_value The trigger value to set  between 0.05 and 0.95.
	 */
	void setLegTriggerValue(leg_t leg, float32_t trigger_value);

	/**
	 * @brief Set the trigger value for the ADC trigger of all the legs.
	 *
	 * This function sets the same trigger value for the ADC trigger of all the legs. The trigger value
	 * determines the level at which the ADC trigger is activated for all legs.
	 *
	 * @param trigger_value The trigger value to set for all the legs between 0.0 and 1.0.
	 */
	void setAllTriggerValue(float32_t trigger_value);

	/**
	 * @brief Set the phase shift value for a specific leg's power control.
	 *
	 * @param leg The leg for which to set the phase shift value.
	 * @param phase_shift The phase shift value to set.
	 */
	void setLegPhaseShift(leg_t leg, int16_t phase_shift);

	/**
	 * @brief Set the phase shift value for all the legs.
	 *
	 * @param phase_shift The phase shift value to set.
	 */
	void setAllPhaseShift(int16_t phase_shift);

	/**
	 * @brief Set the slope compensation in current mode for a leg
	 *
	 * @param leg The leg to initialize
	 * @param set_voltage in volt, the DAC peak voltage for slope compensation (value between 0 and 2.048)
	 * @param reset_voltage in volt, the DAC lowest voltage for slope compensation (value between 0 and 2.048)
	 *
	 * @warning Be sure that set_voltage > reset_voltage
	 */
	void setLegSlopeCompensation(leg_t leg, float32_t set_voltage, float32_t reset_voltage);

	/**
	 * @brief Set the slope compensation in current mode for all the leg
	 *
	 * @param set_voltage in volt, the DAC peak voltage for slope compensation (value between 0 and 2.048)
	 * @param reset_voltage in volt, the DAC lowest voltage for slope compensation (value between 0 and 2.048)
	 *
	 * @warning Be sure that set_voltage > reset_voltage
	 */
	void setAllSlopeCompensation(float32_t set_voltage, float32_t reset_voltage);

	/**
	 * @brief set the dead time value for a leg
	 *
	 * @param leg           - the leg for which to set dead time value
	 * @param ns_rising_dt  - rising dead time value in ns
	 * @param ns_falling_dt - falling dead time value in ns
	*/
	void setLegDeadTime(leg_t leg, uint16_t ns_rising_dt, uint16_t ns_falling_dt);


	/**
	 * @brief set the dead time value for all legs
	 *
	 * @param ns_rising_dt  - rising dead time value in ns
	 * @param ns_falling_dt - falling dead time value in ns
	*/
	void setAllDeadTime(uint16_t ns_rising_dt, uint16_t ns_falling_dt);

	/**
	 * @brief set ADC decimator for a leg
	 *
	 * this function sets the number of event which will be ignored between two events.
	 * ie. you divide the number of trigger in a fixed period. For example if adc_decim = 1,
	 * nothing changes but with adc_decims = 2 you have twice less adc trigger.
	 *
	 * @param leg       - leg for which to set adc decimator value
	 * @param adc_decim - adc decimator, a number between 1 and 32
	 *
	*/
	void setLegAdcDecim(leg_t leg, uint16_t adc_decim);


	/**
	 * @brief set ADC decimator for all legs
	 *
	 * this function sets the number of event which will be ignored between two events.
	 * ie. you divide the number of trigger in a fixed period. For example if adc_decim = 1,
	 * nothing changes but with adc_decims = 2 you have twice less adc trigger.
	 *
	 * @param adc_decim - adc decimator, a number between 1 and 32
	 *
	*/
	void setAllAdcDecim(uint16_t adc_decim);

	/**
	 * @brief Initialise a leg for buck topology
	 *
	 * @param leg Leg to initialize
	 * @param leg_mode PWM mode - VOLTAGE_MODE or CURRENT_MODE
	 *
	 */
	void initLegBuck(leg_t leg, hrtim_pwm_mode_t leg_mode = VOLTAGE_MODE);

	/**
	 * @brief Initialise all the legs for buck topology
	 *
	 * @param leg_mode PWM mode - VOLTAGE_MODE or CURRENT_MODE
	 */
	void initAllBuck(hrtim_pwm_mode_t leg_mode = VOLTAGE_MODE);

	/**
	 * @brief Initialise all the legs for boost topology, current mode is not supported for boost.
	 *
	 * @param leg Leg to initialize
	 */
	void initLegBoost(leg_t leg);

	/**
	 * @brief Initialise all the legs for boost topology, current mode is not supported for boost.
	 */
	void initAllBoost();
};

/////
// Public object to interact with the class
extern TwistAPI twist;

#endif // TWISTAPI_H_
