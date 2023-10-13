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
 * @date   2023
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#include "power.h"
#include "../src/power_init.h"

#include "HardwareConfiguration.h"
#include "GpioApi.h"

PowerDriver power;

hrtim_tu_number_t PowerDriver::spinNumberToTu(uint16_t spin_number)
{
    if(spin_number == 12 || spin_number == 14)
    {
        return PWMA;
    }
    else if(spin_number == 15)
    {
        return PWMB;
    }
    else if(spin_number == 2 || spin_number == 4)
    {
        return PWMC;
    }
    else if(spin_number == 5 || spin_number == 6)
    {
        return PWMD;
    }
    else if(spin_number == 10 || spin_number == 11)
    {
        return PWME;
    }
    else if(spin_number == 7 || spin_number == 9)
    {
        return PWMF;
    }
    else
    {
        return PWMA;
    }
}

/**
 * @brief Set the hardware version of the board.
 *
 * @param shield shield version - shield_TWIST_V1_2, shield_TWIST_V1_3, shield_ownverter or shield_other
 *
 * @warning You need to call this function BEFORE initializing a leg, after that it is not possible
 *          to change the shield version
 */
void PowerDriver::setShieldVersion(shield_version_t shield)
{
    if (shield_init == false)
    {
        shield_version = shield;
        shield_init = true;
    }
}

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
void PowerDriver::initLegMode(leg_t leg, hrtim_switch_convention_t leg_convention, hrtim_pwm_mode_t leg_mode)
{
    hwConfig.pwmSetFrequency(timer_frequency); // Configure PWM frequency

    hwConfig.pwmSetModulation(spinNumberToTu(dt_pwm_pin[leg]), dt_modulation[leg]); // Set modulation

    hwConfig.pwmSetAdcEdgeTrigger(spinNumberToTu(dt_pwm_pin[leg]), dt_edge_trigger[leg]); // Configure ADC rollover in center aligned mode

    /**
     * Configure which External Event will reset the timer for current mode.
     *
     *   COMPARATOR1_INP/PA1 ----------> + - <----------   DAC3 ch1
     *                                    |
     *                                    |
     *                                    v
     *	                                EEV4
     *
     *   COMPARATOR3_INP/PC1 ----------> + - <----------   DAC1 ch1
     *                                    |
     *                                    |
     *                                    v
     *	                                EEV5
     *
     * /!\ WARNING : Current mode is currently only supported for BUCK /!\
     */

    if (leg_mode == CURRENT_MODE)
    {
        if (dt_current_pin[leg] == CM_DAC3)
            hwConfig.pwmSetEev(spinNumberToTu(dt_pwm_pin[leg]), EEV4);
        else if (dt_current_pin[leg] == CM_DAC1)
            hwConfig.pwmSetEev(spinNumberToTu(dt_pwm_pin[leg]), EEV5);

        /* Configure current mode */
        hwConfig.pwmSetMode(spinNumberToTu(dt_pwm_pin[leg]), CURRENT_MODE);
    }

    hwConfig.pwmSetSwitchConvention(spinNumberToTu(dt_pwm_pin[leg]), leg_convention); // choose which output of the timer unit to control whith duty cycle

    hwConfig.pwmInit(spinNumberToTu(dt_pwm_pin[leg])); // Initialize leg

    hwConfig.pwmSetPhaseShift(spinNumberToTu(dt_pwm_pin[leg]), dt_phase_shift[leg]); // Configure PWM initial phase shift

    hwConfig.pwmSetDeadTime(spinNumberToTu(dt_pwm_pin[leg]), dt_rising_deadtime[leg], dt_falling_deadtime[leg]); // Configure PWM dead time

    /**
     * Configure PWM adc trigger.
     * ADC_TRIG1 trigger ADC1, and ADC_TRIG3 trigger ADC2
     */
    if (dt_adc[leg] != ADCTRIG_NONE)
    {
        hwConfig.pwmSetAdcDecimation(spinNumberToTu(dt_pwm_pin[leg]), dt_adc_decim[leg]);

        hwConfig.pwmSetAdcTrig(spinNumberToTu(dt_pwm_pin[leg]), dt_adc[leg]);

        hwConfig.pwmAdcTriggerEnable(spinNumberToTu(dt_pwm_pin[leg]));
    }

    /**
     * Choose which dac control the leg in current mode
     */
    if (leg_mode == CURRENT_MODE)
    {

        if (dt_current_pin[leg] == CM_DAC1)
        {
            hwConfig.dacConfigDac1CurrentmodeInit(tu_channel[spinNumberToTu(dt_pwm_pin[leg])]->pwm_conf.pwm_tu);
            hwConfig.comparator3Initialize();
        }

        else if (dt_current_pin[leg] == CM_DAC3)
        {
            hwConfig.dacConfigDac3CurrentmodeInit(tu_channel[spinNumberToTu(dt_pwm_pin[leg])]->pwm_conf.pwm_tu);
            hwConfig.comparator1Initialize();
        }
    }
    /**
     * Only relevant for twist and ownverter hardware, to enable optocouplers for mosfet driver
     */
    if ((shield_version == shield_TWIST_V1_2 || shield_version == shield_ownverter || shield_version == shield_TWIST_V1_3) && spinNumberToTu(dt_pwm_pin[leg]) == PWMA)
        gpio.configurePin(PC12, OUTPUT);
    else if ((shield_version == shield_TWIST_V1_2 || shield_version == shield_ownverter || shield_version == shield_TWIST_V1_3) && spinNumberToTu(dt_pwm_pin[leg]) == PWMC)
        gpio.configurePin(PC13, OUTPUT);
    else if (shield_version == shield_ownverter && spinNumberToTu(dt_pwm_pin[leg]) == PWME)
        gpio.configurePin(PB7, OUTPUT);

    if (shield_init == false)
        shield_init = true; // When a leg has been initialized, shield version should not be modified
}

/**
 * @brief Initialize power modes for all legs.
 *
 * This function initializes the power modes  for each leg with the specified 'buck'
 * and 'voltage_mode' settings.
 *
 * @param leg_convention PWM Switch to be driven by the duty cycle. The other will be complementary - PWMx1 or PWMx2
 * @param leg_mode PWM mode - VOLTAGE_MODE or CURRENT_MODE
 */
void PowerDriver::initAllMode(hrtim_switch_convention_t leg_convention, hrtim_pwm_mode_t leg_mode)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        initLegMode(static_cast<leg_t>(i), leg_convention, leg_mode);
    }
}

/**
 * @brief Set the duty cycle for a specific leg's power control.
 *
 * This function sets the duty cycle for the power control of a specified leg. The duty cycle
 * determines the ON/OFF ratio of the power signal for the leg.
 *
 * @param leg The leg for which to set the duty cycle.
 * @param duty_leg The duty cycle value to set (a floating-point number between 0.1 and 0.9).
 */
void PowerDriver::setLegDutyCycle(leg_t leg, float32_t duty_leg)
{
    if (duty_leg > 0.9)
        duty_leg = 0.9;
    else if (duty_leg < 0.1)
        duty_leg = 0.1;
    uint16_t value = duty_leg * tu_channel[spinNumberToTu(dt_pwm_pin[leg])]->pwm_conf.period;
    hrtim_duty_cycle_set(spinNumberToTu(dt_pwm_pin[leg]), value);
}

/**
 * @brief Set the duty cycle for power control of all the legs.
 *
 * This function sets the same duty cycle for power control of all the legs. The duty cycle determines
 * the ON/OFF ratio of the power signal for all legs.
 *
 * @param duty_all The duty cycle value to set (a floating-point number between 0.1 and 0.9).
 */
void PowerDriver::setAllDutyCycle(float32_t duty_all)
{
    if (duty_all > 0.9)
        duty_all = 0.9;
    else if (duty_all < 0.1)
        duty_all = 0.1;

    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegDutyCycle(static_cast<leg_t>(i), duty_all);
    }
}

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
void PowerDriver::startLeg(leg_t leg)
{
    /**
     * Only relevant for twist hardware, to enable optocouplers for mosfet driver
     */
    if ((shield_version == shield_TWIST_V1_2 || shield_version == shield_ownverter || shield_version == shield_TWIST_V1_3) && spinNumberToTu(dt_pwm_pin[leg]) == PWMA)
        gpio.setPin(PC12);
    else if ((shield_version == shield_TWIST_V1_2 || shield_version == shield_ownverter || shield_version == shield_TWIST_V1_3) && spinNumberToTu(dt_pwm_pin[leg]) == PWMC)
        gpio.setPin(PC13);
    else if (shield_version == shield_ownverter && spinNumberToTu(dt_pwm_pin[leg]) == PWME)
        gpio.setPin(PB7);

    /* start PWM*/
    if (!dt_output1_inactive[leg])
        hwConfig.pwmStartSubUnit(spinNumberToTu(dt_pwm_pin[leg]), TIMING_OUTPUT1);
    if (!dt_output2_inactive[leg])
        hwConfig.pwmStartSubUnit(spinNumberToTu(dt_pwm_pin[leg]), TIMING_OUTPUT2);
}

/**
 * @brief Start power output for all legs.
 */
void PowerDriver::startAll()
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        startLeg(static_cast<leg_t>(i));
    }
}

/**
 * @brief Stop power output for a specific leg.
 *
 * @param leg The leg for which to stop the power output.
 */
void PowerDriver::stopLeg(leg_t leg)
{
    /* Stop PWM */
    hwConfig.pwmStop(spinNumberToTu(dt_pwm_pin[leg]));

    /**
     * Only relevant for twist hardware, to disable optocouplers for mosfet driver
     */
    if ((shield_version == shield_TWIST_V1_2 || shield_version == shield_ownverter || shield_version == shield_TWIST_V1_3) && spinNumberToTu(dt_pwm_pin[leg]) == PWMA)
        gpio.resetPin(PC12);
    else if ((shield_version == shield_TWIST_V1_2 || shield_version == shield_ownverter || shield_version == shield_TWIST_V1_3) && spinNumberToTu(dt_pwm_pin[leg]) == PWMC)
        gpio.resetPin(PC13);
    else if (shield_version == shield_ownverter && spinNumberToTu(dt_pwm_pin[leg]) == PWME)
        gpio.resetPin(PB7);
}

/**
 * @brief Stop power output for all legs.
 */
void PowerDriver::stopAll()
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        stopLeg(static_cast<leg_t>(i));
    }
}

/**
 * @brief Set the slope compensation in current mode for a leg
 *
 * @param leg The leg to initialize
 * @param set_voltage in volt, the DAC peak voltage for slope compensation (value between 0 and 2.048)
 * @param reset_voltage in volt, the DAC lowest voltage for slope compensation (value between 0 and 2.048)
 *
 * @warning Be sure that set_voltage > reset_voltage
 */
void PowerDriver::setLegSlopeCompensation(leg_t leg, float32_t set_voltage, float32_t reset_voltage)
{
    switch (dt_current_pin[leg])
    {
    case CM_DAC1:
        hwConfig.slopeCompensationDac1(set_voltage, reset_voltage);
        break;
    case CM_DAC3:
        hwConfig.slopeCompensationDac3(set_voltage, reset_voltage);
        break;
    default:
        break;
    }
}

/**
 * @brief Set the slope compensation in current mode for all the leg
 *
 * @param set_voltage in volt, the DAC peak voltage for slope compensation (value between 0 and 2.048)
 * @param reset_voltage in volt, the DAC lowest voltage for slope compensation (value between 0 and 2.048)
 *
 * @warning Be sure that set_voltage > reset_voltage
 */
void PowerDriver::setAllSlopeCompensation(float32_t set_voltage, float32_t reset_voltage)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegSlopeCompensation(static_cast<leg_t>(i), set_voltage, reset_voltage);
    }
}
/**
 * @brief Set the trigger value for a specific leg's ADC trigger.
 *
 * This function sets the trigger value for the ADC trigger of a specified leg. The trigger
 * value determines the level at which the ADC trigger is activated.
 *
 * @param leg The leg for which to set the ADC trigger value.
 * @param trigger_value The trigger value to set  between 0.05 and 0.95.
 */
void PowerDriver::setLegTriggerValue(leg_t leg, float32_t trigger_value)
{
    if (trigger_value > 0.95)
        trigger_value = 0.95;
    else if (trigger_value < 0.05)
        trigger_value = 0.05;
    hwConfig.pwmSetAdcTriggerInstant(spinNumberToTu(dt_pwm_pin[leg]), trigger_value);
}

/**
 * @brief Set the trigger value for the ADC trigger of all the legs.
 *
 * This function sets the same trigger value for the ADC trigger of all the legs. The trigger value
 * determines the level at which the ADC trigger is activated for all legs.
 *
 * @param trigger_value The trigger value to set for all the legs between 0.0 and 1.0.
 */
void PowerDriver::setAllTriggerValue(float32_t trigger_value)
{
    if (trigger_value > 0.95)
        trigger_value = 0.95;
    else if (trigger_value < 0.05)
        trigger_value = 0.05;
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegTriggerValue(static_cast<leg_t>(i), trigger_value);
    }
}

/**
 * @brief Set the phase shift value for a specific leg's power control.
 *
 * @param leg The leg for which to set the phase shift value.
 * @param phase_shift The phase shift value to set.
 */
void PowerDriver::setLegPhaseShift(leg_t leg, int16_t phase_shift)
{
    hwConfig.pwmSetPhaseShift(spinNumberToTu(dt_pwm_pin[leg]), phase_shift);
}

/**
 * @brief Set the phase shift value for all the legs.
 *
 * @param phase_shift The phase shift value to set.
 */
void PowerDriver::setAllPhaseShift(int16_t phase_shift)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegPhaseShift(static_cast<leg_t>(i), phase_shift);
    }
}

/**
 * @brief set the dead time value for a leg
 *
 * @param leg           - the leg for which to set dead time value
 * @param ns_rising_dt  - rising dead time value in ns
 * @param ns_falling_dt - falling dead time value in ns
*/
void PowerDriver::setLegDeadTime(leg_t leg, uint16_t ns_rising_dt, uint16_t ns_falling_dt)
{
    hwConfig.pwmSetDeadTime(spinNumberToTu(dt_pwm_pin[leg]), ns_rising_dt, ns_falling_dt);
}

/**
 * @brief set the dead time value for all legs
 *
 * @param ns_rising_dt  - rising dead time value in ns
 * @param ns_falling_dt - falling dead time value in ns
*/
void PowerDriver::setAllDeadTime(uint16_t ns_rising_dt, uint16_t ns_falling_dt)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegDeadTime(static_cast<leg_t>(i), ns_rising_dt, ns_falling_dt);
    }
}

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
void PowerDriver::setLegAdcDecim(leg_t leg, uint16_t adc_decim)
{
    hwConfig.pwmSetAdcDecimation(spinNumberToTu(dt_pwm_pin[leg]), adc_decim);
}


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
void PowerDriver::setAllAdcDecim(uint16_t adc_decim)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegAdcDecim(static_cast<leg_t>(i), adc_decim);
    }
}

/**
 * @brief Initialise a leg for buck topology
 *
 * @param leg Leg to initialize
 * @param leg_mode PWM mode - VOLTAGE_MODE or CURRENT_MODE
 *
 */
void PowerDriver::initLegBuck(leg_t leg, hrtim_pwm_mode_t leg_mode)
{
    if (spinNumberToTu(dt_pwm_pin[leg]) == PWMA && shield_version == shield_TWIST_V1_2)
        initLegMode(leg, PWMx2, leg_mode);
    else
        initLegMode(leg, PWMx1, leg_mode);
}

/**
 * @brief Initialise all the legs for buck topology
 *
 * @param leg_mode PWM mode - VOLTAGE_MODE or CURRENT_MODE
 */
void PowerDriver::initAllBuck(hrtim_pwm_mode_t leg_mode)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        initLegBuck(static_cast<leg_t>(i), leg_mode);
    }
}

/**
 * @brief Initialise all the legs for boost topology, current mode is not supported for boost.
 *
 * @param leg Leg to initialize
 */
void PowerDriver::initLegBoost(leg_t leg)
{
    if (spinNumberToTu(dt_pwm_pin[leg]) == PWMA && shield_version == shield_TWIST_V1_2)
        initLegMode(leg, PWMx1, VOLTAGE_MODE);
    else
        initLegMode(leg, PWMx2, VOLTAGE_MODE);
}

/**
 * @brief Initialise all the legs for boost topology, current mode is not supported for boost.
 */
void PowerDriver::initAllBoost()
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        initLegBoost(static_cast<leg_t>(i));
    }
}