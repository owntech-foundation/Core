/*
 * Copyright (c) 2023-2024 LAAS-CNRS
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
 * @author Jean Alinei <jean.alinei@owntech.org>
 */

#include "power_init.h"
#include "Power.h"
#include "SpinAPI.h"


hrtim_tu_number_t PowerAPI::spinNumberToTu(uint16_t spin_number)
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

void PowerAPI::initLegMode(leg_t leg,                                       \
                           hrtim_switch_convention_t leg_convention,         \
                           hrtim_pwm_mode_t leg_mode)
{
    /* Configure PWM frequency */
    spin.pwm.initFrequency(timer_frequency);

    /* Set modulation */
    spin.pwm.setModulation(spinNumberToTu(dt_pwm_pin[leg]),                  \
                                          dt_modulation[leg]);

    /* Configure ADC rollover in center aligned mode */
    spin.pwm.setAdcEdgeTrigger(spinNumberToTu(dt_pwm_pin[leg]),              \
                                dt_edge_trigger[leg]);

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
        {
            spin.pwm.setEev(spinNumberToTu(dt_pwm_pin[leg]), EEV4);
        }
        else if (dt_current_pin[leg] == CM_DAC1)
        {
            spin.pwm.setEev(spinNumberToTu(dt_pwm_pin[leg]), EEV5);
        }

        /* Configure current mode */
        spin.pwm.setMode(spinNumberToTu(dt_pwm_pin[leg]), CURRENT_MODE);

    }
    /* choose which output of the timer unit to control whith duty cycle */
    spin.pwm.setSwitchConvention(spinNumberToTu(dt_pwm_pin[leg]),           \
                                                leg_convention);

    /* Initialize leg unit */
    spin.pwm.initUnit(spinNumberToTu(dt_pwm_pin[leg]));

    /* Configure PWM initial phase shift */
    spin.pwm.setPhaseShift(spinNumberToTu(dt_pwm_pin[leg]),                 \
                                          dt_phase_shift[leg]);

    /* Configure PWM dead time */
    spin.pwm.setDeadTime(spinNumberToTu(dt_pwm_pin[leg]),                   \
                                        dt_rising_deadtime[leg],            \
                                        dt_falling_deadtime[leg]);

    /**
     * Configure PWM adc trigger.
     * ADC_TRIG1 trigger ADC1, and ADC_TRIG3 trigger ADC2
     */
    if (dt_adc[leg] != ADCTRIG_NONE)
    {
        spin.pwm.setAdcDecimation(spinNumberToTu(dt_pwm_pin[leg]),          \
                                                 dt_adc_decim[leg]);

        spin.pwm.setAdcTrigger(spinNumberToTu(dt_pwm_pin[leg]),             \
                                              dt_adc[leg]);

        spin.pwm.enableAdcTrigger(spinNumberToTu(dt_pwm_pin[leg]));
    }

    /**
     * Choose which dac control the leg in current mode
     */
    if (leg_mode == CURRENT_MODE)
    {

        if (dt_current_pin[leg] == CM_DAC1)
        {
            spin.dac.currentModeInit(                                       \
                1,                                                          \
                tu_channel[spinNumberToTu(dt_pwm_pin[leg])]->pwm_conf.pwm_tu);

            spin.comp.initialize(3);
        }

        else if (dt_current_pin[leg] == CM_DAC3)
        {
            spin.dac.currentModeInit(                                       \
                3,                                                          \
                tu_channel[spinNumberToTu(dt_pwm_pin[leg])]->pwm_conf.pwm_tu);

            spin.comp.initialize(1);
        }
    }

    /**
     * Only relevant for twist and ownverter hardware, to enable optocouplers
     * for mosfet driver and connection to electrolytic capacitor
     */
    for(uint8_t i = 0; i < dt_leg_count; i++)
    {
        if(dt_pin_driver[i] != 0)
        {
            spin.gpio.configurePin(dt_pin_driver[i], OUTPUT);
        }
        if(dt_pin_capacitor[i] != 0)
        {
            spin.gpio.configurePin(dt_pin_capacitor[i], OUTPUT);
        }
    }
}

void PowerAPI::initAllMode(hrtim_switch_convention_t leg_convention,        \
                           hrtim_pwm_mode_t leg_mode)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        initLegMode(static_cast<leg_t>(i), leg_convention, leg_mode);
    }
}

void PowerAPI::setLegDutyCycle(leg_t leg, float32_t duty_leg)
{
    if (duty_leg > 0.9)
    {
        duty_leg = 0.9;
    }

    else if (duty_leg < 0.1)
    {
        duty_leg = 0.1;
    }

    uint16_t value =
        duty_leg * tu_channel[spinNumberToTu(dt_pwm_pin[leg])]->pwm_conf.period;

    hrtim_duty_cycle_set(spinNumberToTu(dt_pwm_pin[leg]), value);
}

void PowerAPI::setAllDutyCycle(float32_t duty_all)
{
    if (duty_all > 0.9)
    {
        duty_all = 0.9;
    }
    else if (duty_all < 0.1)
    {
        duty_all = 0.1;
    }

    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegDutyCycle(static_cast<leg_t>(i), duty_all);
    }
}

void PowerAPI::startLeg(leg_t leg)
{
    /**
     * Only relevant for twist hardware, to enable optocouplers
     * for mosfet driver
     */
    if(dt_pin_driver[leg] != 0) spin.gpio.setPin(dt_pin_driver[leg]);

    /* start PWM*/
    if (!dt_output1_inactive[leg])
    {
        spin.pwm.startSingleOutput(spinNumberToTu(dt_pwm_pin[leg]),          \
                                                  TIMING_OUTPUT1);
    }
    if (!dt_output2_inactive[leg])
    {
        spin.pwm.startSingleOutput(spinNumberToTu(dt_pwm_pin[leg]),          \
                                                  TIMING_OUTPUT2);
    }
}

void PowerAPI::connectLegCapacitor(leg_t leg)
{
    if(dt_pin_capacitor[leg] != 0)
    {
        spin.gpio.setPin(dt_pin_capacitor[leg]);
    }
}

void PowerAPI::startAll()
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        startLeg(static_cast<leg_t>(i));
    }
}

void PowerAPI::connectAllCapacitor()
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        connectLegCapacitor(static_cast<leg_t>(i));
    }
}

void PowerAPI::stopLeg(leg_t leg)
{
    /* Stop PWM */
    spin.pwm.stopDualOutput(spinNumberToTu(dt_pwm_pin[leg]));

    /**
     * Only relevant for twist hardware, to disable optocouplers for mosfet
     * driver
     */
    if(dt_pin_driver[leg] != 0)
    {
        spin.gpio.resetPin(dt_pin_driver[leg]);
    }
}

void PowerAPI::disconnectLegCapacitor(leg_t leg)
{
    if(dt_pin_capacitor[leg] != 0)
    {
        spin.gpio.resetPin(dt_pin_capacitor[leg]);
    }
}

void PowerAPI::stopAll()
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        stopLeg(static_cast<leg_t>(i));
    }
}

void PowerAPI::disconnectAllCapacitor()
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        disconnectLegCapacitor(static_cast<leg_t>(i));
    }
}

void PowerAPI::setLegSlopeCompensation(leg_t leg,                           \
                                       float32_t set_voltage,               \
                                       float32_t reset_voltage)
{
    switch (dt_current_pin[leg])
    {
    case CM_DAC1:
        spin.dac.slopeCompensation(1, set_voltage, reset_voltage);
        break;
    case CM_DAC3:
        spin.dac.slopeCompensation(3, set_voltage, reset_voltage);
        break;
    default:
        break;
    }
}

void PowerAPI::setAllSlopeCompensation(float32_t set_voltage,               \
                                       float32_t reset_voltage)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegSlopeCompensation(static_cast<leg_t>(i),                      \
                                set_voltage,                                \
                                reset_voltage);
    }
}

void PowerAPI::setLegTriggerValue(leg_t leg, float32_t trigger_value)
{
    if (trigger_value > 0.95)
    {
        trigger_value = 0.95;
    }
    else if (trigger_value < 0.05)
    {
        trigger_value = 0.05;
    }

    spin.pwm.setAdcTriggerInstant(spinNumberToTu(dt_pwm_pin[leg]),          \
                                  trigger_value);

}

void PowerAPI::setAllTriggerValue(float32_t trigger_value)
{
    if (trigger_value > 0.95)
    {
        trigger_value = 0.95;
    }
    else if (trigger_value < 0.05)
    {
        trigger_value = 0.05;
    }
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegTriggerValue(static_cast<leg_t>(i), trigger_value);
    }
}

void PowerAPI::setLegPhaseShift(leg_t leg, int16_t phase_shift)
{
    spin.pwm.setPhaseShift(spinNumberToTu(dt_pwm_pin[leg]), phase_shift);

}

void PowerAPI::setAllPhaseShift(int16_t phase_shift)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegPhaseShift(static_cast<leg_t>(i), phase_shift);
    }
}

void PowerAPI::setLegDeadTime(leg_t leg,                                    \
                              uint16_t ns_rising_dt,                        \
                              uint16_t ns_falling_dt)
{
    spin.pwm.setDeadTime(spinNumberToTu(dt_pwm_pin[leg]),                   \
                                        ns_rising_dt,                       \
                                        ns_falling_dt);
}

void PowerAPI::setAllDeadTime(uint16_t ns_rising_dt, uint16_t ns_falling_dt)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegDeadTime(static_cast<leg_t>(i), ns_rising_dt, ns_falling_dt);
    }
}

void PowerAPI::setLegAdcDecim(leg_t leg, uint16_t adc_decim)
{
    spin.pwm.setAdcDecimation(spinNumberToTu(dt_pwm_pin[leg]), adc_decim);
}

void PowerAPI::setAllAdcDecim(uint16_t adc_decim)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        setLegAdcDecim(static_cast<leg_t>(i), adc_decim);
    }
}

void PowerAPI::initLegBuck(leg_t leg, hrtim_pwm_mode_t leg_mode)
{
    if (!dt_pwm_x1_high[leg])
    {
        /* PWMx1 is connected in hardware to switch low*/
        initLegMode(leg, PWMx2, leg_mode);
    }
    else
    {
        /* PWMx1 is connected in hardware to switch high*/
        initLegMode(leg, PWMx1, leg_mode);
    }
}

void PowerAPI::initAllBuck(hrtim_pwm_mode_t leg_mode)
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        initLegBuck(static_cast<leg_t>(i), leg_mode);
    }
}

void PowerAPI::initLegBoost(leg_t leg)
{
    if (!dt_pwm_x1_high[leg])
    {
        /* PWMx1 is connected in hardware to switch low*/
        initLegMode(leg, PWMx1, VOLTAGE_MODE);
    }
    else
    {
        /* PWMx1 is connected in hardware to switch high*/
        initLegMode(leg, PWMx2, VOLTAGE_MODE);
    }
}

void PowerAPI::initAllBoost()
{
    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        initLegBoost(static_cast<leg_t>(i));
    }
}
