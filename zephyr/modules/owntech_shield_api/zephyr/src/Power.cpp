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
 * @date   2024
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 * @author Jean Alinei <jean.alinei@owntech.org>
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#include "power_init.h"
#include "Power.h"
#include "SpinAPI.h"

#define DUTY_SEQ_MAX_LEN CONFIG_OWNTECH_POWER_DUTY_SEQ_MAX_LEN

/**
 * 100 % in a sequence: CMP1 is placed this far above the period so that it
 * never matches (one HRTIM clock in high-resolution counts), without
 * exceeding the highest compare value allowed by the HRTIM.
 */
#define SEQ_CMP_ABOVE_PERIOD 32U
#define SEQ_CMP_MAX          0xFFDFU

/**
 * Duty-cycle sequences, one per leg (`ALL` is the number of legs).
 *
 * seq_raw is the buffer read by the DMA. In double-buffered mode it holds
 * two copies of the sequence: updates go to seq_stage and are copied into
 * each half by the DMA interrupt once that half has been played.
 */
static float32_t seq_duty[ALL][DUTY_SEQ_MAX_LEN];
static uint32_t seq_raw[ALL][2 * DUTY_SEQ_MAX_LEN];
static uint32_t seq_stage[ALL][DUTY_SEQ_MAX_LEN];
static uint16_t seq_len[ALL] = {0};
static bool seq_double[ALL] = {false};

/* Leg index driven by each timing unit, for the DMA callback */
static int8_t seq_tu_leg[HRTIM_STU_NUMOF] = {-1, -1, -1, -1, -1, -1};

/* DMA interrupt: half `finished_half` has been played, refresh it */
static void seq_half_done(hrtim_tu_number_t tu_number, uint8_t finished_half)
{
    int8_t i = seq_tu_leg[tu_number];

    if (i < 0 || !seq_double[i])
    {
        return;
    }

    uint32_t *half = &seq_raw[i][finished_half * seq_len[i]];

    for (uint16_t k = 0; k < seq_len[i]; k++)
    {
        half[k] = seq_stage[i][k];
    }
}

/* Writes the staged sequence of a leg into every part of its DMA buffer */
static void seq_copy_stage_to_all(int8_t i)
{
    uint8_t copies = seq_double[i] ? 2 : 1;

    for (uint8_t c = 0; c < copies; c++)
    {
        for (uint16_t k = 0; k < seq_len[i]; k++)
        {
            seq_raw[i][c * seq_len[i] + k] = seq_stage[i][k];
        }
    }
}


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

void PowerAPI::initMode(leg_t leg,
                        hrtim_switch_convention_t leg_convention,
                        hrtim_pwm_mode_t leg_mode)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;
    uint16_t period = 0;

   /*  If ALL is selected, loop through all legs */
    if (leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        /* Configure PWM frequency */
        spin.pwm.initVariableFrequency(timer_frequency, timer_min_frequency);

        /* Set modulation */
        spin.pwm.setModulation(spinNumberToTu(dt_pwm_pin[i]),
                               dt_modulation[i]);

        /* Configure ADC rollover in center aligned mode */
        spin.pwm.setAdcEdgeTrigger(spinNumberToTu(dt_pwm_pin[i]),
                                   dt_edge_trigger[i]);

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
            if (dt_current_pin[i] == CM_DAC3)
            {
                spin.pwm.setEev(spinNumberToTu(dt_pwm_pin[i]), EEV4);
            }
            else if (dt_current_pin[i] == CM_DAC1)
            {
                spin.pwm.setEev(spinNumberToTu(dt_pwm_pin[i]), EEV5);
            }

            /* Configure current mode */
            spin.pwm.setMode(spinNumberToTu(dt_pwm_pin[i]), CURRENT_MODE);
        }

        /* Choose which output of the timer unit to control with duty cycle */
        spin.pwm.setSwitchConvention(spinNumberToTu(dt_pwm_pin[i]),
                                     leg_convention);

        /* Initialize leg unit */
        spin.pwm.initUnit(spinNumberToTu(dt_pwm_pin[i]));

        /* Configure PWM initial phase shift */
        spin.pwm.setPhaseShift(spinNumberToTu(dt_pwm_pin[i]),
                               dt_phase_shift[i]);

        /* Configure PWM dead time */
        spin.pwm.setDeadTime(spinNumberToTu(dt_pwm_pin[i]),
                             dt_rising_deadtime[i],
                             dt_falling_deadtime[i]);

        /**
         * Configure PWM ADC trigger.
         */
        if (dt_adc[i] != UNKNOWN_ADC)
        {
            spin.pwm.setAdcDecimation(spinNumberToTu(dt_pwm_pin[i]),
                                      dt_adc_decim[i]);

            spin.pwm.setAdcTrigger(spinNumberToTu(dt_pwm_pin[i]),
                                   dt_adc[i]);

            spin.pwm.enableAdcTrigger(spinNumberToTu(dt_pwm_pin[i]));

            spin.data.configureTriggerSource(dt_adc[i], TRIG_PWM);
        }

        /**
         * Choose which DAC controls the leg in current mode
         */
        if (leg_mode == CURRENT_MODE)
        {
            if (dt_current_pin[i] == CM_DAC1)
            {
                spin.dac.currentModeInit(
                    1,
                    tu_channel[spinNumberToTu(dt_pwm_pin[i])]->pwm_conf.pwm_tu
                );

                spin.comp.initialize(3);
            }
            else if (dt_current_pin[i] == CM_DAC3)
            {
                spin.dac.currentModeInit(
                    3,
                    tu_channel[spinNumberToTu(dt_pwm_pin[i])]->pwm_conf.pwm_tu
                );

                spin.comp.initialize(1);
            }
        }

        /**
         * Only relevant for twist and ownverter hardware, to enable optocouplers
         * for MOSFET driver and connection to electrolytic capacitor
         */
        if (dt_pin_driver[i] != 0)
        {
            spin.gpio.configurePin(dt_pin_driver[i], OUTPUT);
        }
        if (dt_pin_capacitor[i] != 0)
        {
            spin.gpio.configurePin(dt_pin_capacitor[i], OUTPUT);
        }
    }
}


void PowerAPI::setDutyCycle(leg_t leg, float32_t duty_value)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    if (leg == ALL)
    {
        startIndex = 0;
        endIndex = dt_leg_count;
    }
    else
    {
        startIndex = leg;
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[i]);
        uint16_t period = tu_channel[leg_tu]->pwm_conf.period;
        uint16_t value = duty_value * period;
        setDutyCycleRaw(static_cast<leg_t>(i), value);
    }
}

void PowerAPI::setDutyCycleRaw(leg_t leg, uint16_t duty_value)
{
    uint16_t period;
    uint8_t swap_state;
    hrtim_tu_number_t leg_tu;
    uint16_t duty_cycle_max_raw;
    uint16_t duty_cycle_min_raw;

    int8_t startIndex = 0;
    int8_t endIndex = 0;    

    /*  If ALL is selected, loop through all legs */
    if (leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        startIndex = leg; /* Treat `leg` as the specific leg index */
        endIndex = leg + 1; /* Only iterate for this specific leg */
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        leg_tu = spinNumberToTu(dt_pwm_pin[i]);

        /* A running duty-cycle sequence owns the duty cycle of this leg */
        if (spin.pwm.isDutySequenceRunning(leg_tu))
        {
            continue;
        }

        duty_cycle_max_raw = tu_channel[leg_tu]->pwm_conf.duty_max_user;
        duty_cycle_min_raw = tu_channel[leg_tu]->pwm_conf.duty_min_user;

        /* Clamp the duty cycle to be within the range min to max */
        if (duty_value > duty_cycle_max_raw)
        {
            duty_value = duty_cycle_max_raw;
        }
        else if (duty_value < duty_cycle_min_raw)
        {
            duty_value = duty_cycle_min_raw;
        }
        
        period = tu_channel[leg_tu]->pwm_conf.period;
        swap_state = tu_channel[leg_tu]->pwm_conf.duty_swap;

        /* Implements a logic that allows for a duty cycle of 100% */
        if (duty_value >= period-3){
            duty_value = 0;
            hrtim_duty_cycle_set(leg_tu, duty_value);
        
            if(swap_state == false){ 
                hrtim_output_hot_swap(leg_tu);
            }        
        }
        else
        {    
            if(swap_state == true) {
                hrtim_duty_cycle_set(leg_tu, duty_value);
                hrtim_output_hot_swap(leg_tu);
            }else{
                hrtim_duty_cycle_set(leg_tu, duty_value);
            }
        }
    }
}

void PowerAPI::start(leg_t leg)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /* If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterates for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t leg_index = startIndex; leg_index < endIndex; leg_index++)
    {
        /**
         * Only relevant for twist hardware, to enable optocouplers
         * for mosfet driver
         */
        if(dt_pin_driver[leg_index] != 0) {
            spin.gpio.setPin(dt_pin_driver[leg_index]);
        }

        if (!dt_output1_inactive[leg_index])
        {
            spin.pwm.startSingleOutput(spinNumberToTu(dt_pwm_pin[leg_index]),
                                       TIMING_OUTPUT1);
        }
        if (!dt_output2_inactive[leg_index])
        {
            spin.pwm.startSingleOutput(spinNumberToTu(dt_pwm_pin[leg_index]),
                                       TIMING_OUTPUT2);
        }
    }
}

void PowerAPI::stop(leg_t leg)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    /* Stopping the outputs also ends any running duty-cycle sequence */
    stopDutySequence(leg);

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        /* Stop PWM */
        spin.pwm.stopDualOutput(spinNumberToTu(dt_pwm_pin[i]));

        /**
         * Only relevant for twist hardware, to disable optocouplers for mosfet
         * driver
         */
        if(dt_pin_driver[i] != 0)
        {
            spin.gpio.resetPin(dt_pin_driver[i]);
        }
    }
}

#ifdef CONFIG_SHIELD_TWIST

void PowerAPI::connectCapacitor(leg_t leg)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        if(dt_pin_capacitor[i] != 0)
        {
            spin.gpio.resetPin(dt_pin_capacitor[i]);
        }
    }
}

void PowerAPI::disconnectCapacitor(leg_t leg)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        if(dt_pin_capacitor[i] != 0)
        {
            spin.gpio.setPin(dt_pin_capacitor[i]);
        }
    }
}

#endif

#ifndef CONFIG_SHIELD_O2

void PowerAPI::connectDriver(leg_t leg)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        endIndex = dt_leg_count; /* retrieves the total number of legs */
    }
    else
    {
        startIndex = leg; /* Treat `leg` as the specific leg index */
        endIndex = leg + 1; /* Only iterate for this specific leg */
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        if(dt_pin_driver[i] != 0) {
            spin.gpio.setPin(dt_pin_driver[i]);
        }
    }
}

void PowerAPI::disconnectDriver(leg_t leg)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        if(dt_pin_driver[i] != 0) {
            spin.gpio.setPin(dt_pin_driver[i]);
        }
    }
}

#endif

void PowerAPI::setSlopeCompensation(leg_t leg,
                                    float32_t set_voltage,
                                    float32_t reset_voltage)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        switch (dt_current_pin[i])
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
}

void PowerAPI::setTriggerValue(leg_t leg, float32_t trigger_value)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /* Clamp the trigger value within the acceptable range */
    if (trigger_value > 0.95)
    {
        trigger_value = 0.95;
    }
    else if (trigger_value < 0.05)
    {
        trigger_value = 0.05;
    }

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        spin.pwm.setAdcTriggerInstant(spinNumberToTu(dt_pwm_pin[i]),
                                      trigger_value);
    }
}

void PowerAPI::setPhaseShift(leg_t leg, int16_t phase_shift)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        spin.pwm.setPhaseShift(spinNumberToTu(dt_pwm_pin[i]), phase_shift);
    }
}

void PowerAPI::setDeadTime(leg_t leg,
                           uint16_t ns_rising_dt,
                           uint16_t ns_falling_dt)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        spin.pwm.setDeadTime(spinNumberToTu(dt_pwm_pin[i]),
                             ns_rising_dt, ns_falling_dt);
    }
}

void PowerAPI::setFrequency(uint32_t frequency)
{
    /**
     * Raw sequence values depend on the period: stop running sequences,
     * rescale them once the new period is applied, then restart them.
     */
    hrtim_tu_number_t restart_tu[ALL];
    int8_t restart_leg[ALL];
    uint8_t restart_count = 0;

    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[i]);

        if (spin.pwm.isDutySequenceRunning(leg_tu))
        {
            spin.pwm.stopDutySequence(leg_tu);
            restart_tu[restart_count] = leg_tu;
            restart_leg[restart_count] = i;
            restart_count++;
        }
    }

    spin.pwm.setFrequency(frequency);

    /* spin.pwm.setFrequency() clamps below timer_min_frequency, so mirror
     * that clamping here to keep timer_frequency consistent with the
     * frequency actually applied to the PWM. */
    if (frequency < timer_min_frequency)
    {
        frequency = timer_min_frequency;
    }

    timer_frequency = frequency;

    for (int8_t i = 0; i < dt_leg_count; i++)
    {
        hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[i]);
        uint16_t period = tu_channel[leg_tu]->pwm_conf.period;

        tu_channel[leg_tu]->pwm_conf.duty_min_user =
            tu_channel[leg_tu]->pwm_conf.duty_min_user_float * period;
        tu_channel[leg_tu]->pwm_conf.duty_max_user =
            tu_channel[leg_tu]->pwm_conf.duty_max_user_float * period;
    }

    for (uint8_t k = 0; k < restart_count; k++)
    {
        int8_t i = restart_leg[k];

        for (uint16_t n = 0; n < seq_len[i]; n++)
        {
            seq_stage[i][n] = dutyToSequenceRaw(i, seq_duty[i][n]);
        }
        seq_copy_stage_to_all(i);
    }

    if (restart_count > 0)
    {
        spin.pwm.startDutySequence(restart_tu, restart_count);
    }
}

uint32_t PowerAPI::getFrequency()
{
    return timer_frequency;
}

uint32_t PowerAPI::getFrequencyMin()
{
    return timer_min_frequency;
}


void PowerAPI::setDutyCycleMin(leg_t leg, float32_t duty_cycle){
    int8_t startIndex = 0;
    int8_t endIndex = 0;
    hrtim_tu_number_t leg_tu;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        if (duty_cycle <= 1.0 && duty_cycle >=0.0){
            leg_tu = spinNumberToTu(dt_pwm_pin[i]);
            uint16_t period = tu_channel[leg_tu]->pwm_conf.period;
            tu_channel[leg_tu]->pwm_conf.duty_min_user = duty_cycle * period;
            tu_channel[leg_tu]->pwm_conf.duty_min_user_float = duty_cycle;
        }
    }
}

void PowerAPI::setDutyCycleMax(leg_t leg, float32_t duty_cycle){
    int8_t startIndex = 0;
    int8_t endIndex = 0;
    hrtim_tu_number_t leg_tu;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        if (duty_cycle <= 1.0 && duty_cycle >=0.0){
            leg_tu = spinNumberToTu(dt_pwm_pin[i]);
            uint16_t period = tu_channel[leg_tu]->pwm_conf.period;
            tu_channel[leg_tu]->pwm_conf.duty_max_user = duty_cycle * period;
            tu_channel[leg_tu]->pwm_conf.duty_max_user_float = duty_cycle;
        }
    }
}

void PowerAPI::setDutyCycleMinRaw(leg_t leg, uint16_t duty_cycle){
    int8_t startIndex = 0;
    int8_t endIndex = 0;
    hrtim_tu_number_t leg_tu;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        if (duty_cycle <= 0){
            duty_cycle = 0;
        }
        leg_tu = spinNumberToTu(dt_pwm_pin[i]);
        tu_channel[leg_tu]->pwm_conf.duty_min_user = duty_cycle;
        uint16_t period = tu_channel[leg_tu]->pwm_conf.period;
        tu_channel[leg_tu]->pwm_conf.duty_min_user_float = 
                                            (float32_t)duty_cycle /
                                            (float32_t)period;
        
    }

}

void PowerAPI::setDutyCycleMaxRaw(leg_t leg,uint16_t duty_cycle){

    int8_t startIndex = 0;
    int8_t endIndex = 0;
    hrtim_tu_number_t leg_tu;
    uint16_t period;
    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        leg_tu = spinNumberToTu(dt_pwm_pin[i]);
        period = tu_channel[leg_tu]->pwm_conf.period;
        if (duty_cycle >= period){
            duty_cycle = period; 
        }    
        tu_channel[leg_tu]->pwm_conf.duty_max_user = duty_cycle;
        tu_channel[leg_tu]->pwm_conf.duty_max_user_float = 
                                                (float32_t)duty_cycle /
                                                (float32_t)period;
    }
}

float32_t PowerAPI::getDutyCycleMax(leg_t leg){
    hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[leg]);
    return  tu_channel[leg_tu]->pwm_conf.duty_max_user_float; 
}

uint16_t PowerAPI::getDutyCycleMaxRaw(leg_t leg){
    hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[leg]);
    return  tu_channel[leg_tu]->pwm_conf.duty_max_user; 
}

float32_t PowerAPI::getDutyCycleMin(leg_t leg){
    hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[leg]);
    return  tu_channel[leg_tu]->pwm_conf.duty_min_user_float; 
}

uint16_t PowerAPI::getDutyCycleMinRaw(leg_t leg){
    hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[leg]);
    return  tu_channel[leg_tu]->pwm_conf.duty_min_user; 
}

uint16_t PowerAPI::getPeriod(leg_t leg){
    hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[leg]);
    return  tu_channel[leg_tu]->pwm_conf.period; 
}


void PowerAPI::setAdcDecim(leg_t leg, uint16_t adc_decim)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
         /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        spin.pwm.setAdcDecimation(spinNumberToTu(dt_pwm_pin[i]), adc_decim);
    }
}


void PowerAPI::initBuck(leg_t leg, hrtim_pwm_mode_t leg_mode)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        if (!dt_pwm_x1_high[i])
        {
            /* PWMx1 is connected in hardware to switch low */
            initMode(static_cast<leg_t>(i), PWMx2, leg_mode);
        }
        else
        {
            /* PWMx1 is connected in hardware to switch high */
            initMode(static_cast<leg_t>(i), PWMx1, leg_mode);
        }
    }
}

void PowerAPI::initBoost(leg_t leg)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if(leg == ALL)
    {
        startIndex = 0;
        /* retrieves the total number of legs */
        endIndex = dt_leg_count;
    }
    else
    {
        /* Treat `leg` as the specific leg index */
        startIndex = leg;
        /* Only iterate for this specific leg */
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        if (!dt_pwm_x1_high[i])
        {
            /* PWMx1 is connected in hardware to switch low */
            initMode(static_cast<leg_t>(i), PWMx1, VOLTAGE_MODE);
        }
        else
        {
            /* PWMx1 is connected in hardware to switch high */
            initMode(static_cast<leg_t>(i), PWMx2, VOLTAGE_MODE);
        }
    }
}

uint32_t PowerAPI::dutyToSequenceRaw(int8_t leg_index, float32_t duty)
{
    hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[leg_index]);
    uint16_t period = tu_channel[leg_tu]->pwm_conf.period;

    if (duty < 0.0F)
    {
        duty = 0.0F;
    }
    else if (duty > 1.0F)
    {
        duty = 1.0F;
    }

    uint32_t raw = duty * period;

    /* Same limits as setDutyCycleRaw */
    if (raw > tu_channel[leg_tu]->pwm_conf.duty_max_user)
    {
        raw = tu_channel[leg_tu]->pwm_conf.duty_max_user;
    }
    else if (raw < tu_channel[leg_tu]->pwm_conf.duty_min_user)
    {
        raw = tu_channel[leg_tu]->pwm_conf.duty_min_user;
    }

    /**
     * 100 %: in left-aligned mode the output is set on period and reset on
     * CMP1. A CMP1 equal to the period resets it at the same instant it is
     * set, and reset has priority (this is why CMP1 = 0 gives 0 %, which
     * setDutyCycleRaw relies on). CMP1 above the period never matches, so
     * the output stays set. Same threshold as setDutyCycleRaw.
     */
    if (raw >= (uint32_t)(period - 3))
    {
        raw = period + SEQ_CMP_ABOVE_PERIOD;
        if (raw > SEQ_CMP_MAX)
        {
            raw = SEQ_CMP_MAX;
        }
    }

    return raw;
}

int8_t PowerAPI::setDutySequence(leg_t leg,
                                 const float32_t *duties,
                                 uint16_t length,
                                 bool double_buffered)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    if (duties == NULL || length == 0 || length > DUTY_SEQ_MAX_LEN)
    {
        return -1;
    }

    /*  If ALL is selected, loop through all legs */
    if (leg == ALL)
    {
        startIndex = 0;
        endIndex = dt_leg_count;
    }
    else
    {
        startIndex = leg;
        endIndex = leg + 1;
    }

    hrtim_tu_number_t restart_tu[ALL];
    uint8_t restart_count = 0;
    int8_t result = 0;

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[i]);

        /* A running sequence is reloaded and restarted */
        if (spin.pwm.isDutySequenceRunning(leg_tu))
        {
            spin.pwm.stopDutySequence(leg_tu);
            restart_tu[restart_count++] = leg_tu;
        }

        seq_len[i] = length;
        seq_double[i] = double_buffered;
        seq_tu_leg[leg_tu] = i;

        for (uint16_t k = 0; k < length; k++)
        {
            seq_duty[i][k] = duties[k];
            seq_stage[i][k] = dutyToSequenceRaw(i, duties[k]);
        }
        seq_copy_stage_to_all(i);

        uint16_t dma_length = double_buffered ? 2 * length : length;
        hrtim_dma_cmp1_callback_t callback =
            double_buffered ? seq_half_done : NULL;

        if (spin.pwm.initDutySequence(leg_tu, seq_raw[i],
                                      dma_length, callback) != 0)
        {
            seq_len[i] = 0;
            result = -1;
        }
    }

    if (restart_count > 0)
    {
        spin.pwm.startDutySequence(restart_tu, restart_count);
    }

    return result;
}

int8_t PowerAPI::startDutySequence(leg_t leg)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if (leg == ALL)
    {
        startIndex = 0;
        endIndex = dt_leg_count;
    }
    else
    {
        startIndex = leg;
        endIndex = leg + 1;
    }

    hrtim_tu_number_t start_tu[ALL];
    uint8_t start_count = 0;

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[i]);

        if (seq_len[i] > 0 && !spin.pwm.isDutySequenceRunning(leg_tu))
        {
            start_tu[start_count++] = leg_tu;
        }
    }

    if (start_count == 0)
    {
        return -1;
    }

    return spin.pwm.startDutySequence(start_tu, start_count);
}

void PowerAPI::stopDutySequence(leg_t leg)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    /*  If ALL is selected, loop through all legs */
    if (leg == ALL)
    {
        startIndex = 0;
        endIndex = dt_leg_count;
    }
    else
    {
        startIndex = leg;
        endIndex = leg + 1;
    }

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        hrtim_tu_number_t leg_tu = spinNumberToTu(dt_pwm_pin[i]);

        if (spin.pwm.isDutySequenceRunning(leg_tu))
        {
            spin.pwm.stopDutySequence(leg_tu);
            /* Do not leave the last sequence value (possibly 100 %) applied */
            setDutyCycleRaw(static_cast<leg_t>(i),
                            tu_channel[leg_tu]->pwm_conf.duty_min_user);
        }
    }
}

bool PowerAPI::isDutySequenceRunning(leg_t leg)
{
    return spin.pwm.isDutySequenceRunning(spinNumberToTu(dt_pwm_pin[leg]));
}

uint16_t PowerAPI::getDutySequenceIndex(leg_t leg)
{
    if (seq_len[leg] == 0)
    {
        return 0;
    }

    /* Both halves of a double-buffered sequence hold the same pattern */
    return spin.pwm.getDutySequenceIndex(spinNumberToTu(dt_pwm_pin[leg])) %
           seq_len[leg];
}

int8_t PowerAPI::updateDutySequence(leg_t leg, const float32_t *duties)
{
    int8_t startIndex = 0;
    int8_t endIndex = 0;

    if (duties == NULL)
    {
        return -1;
    }

    /*  If ALL is selected, loop through all legs */
    if (leg == ALL)
    {
        startIndex = 0;
        endIndex = dt_leg_count;
    }
    else
    {
        startIndex = leg;
        endIndex = leg + 1;
    }

    int8_t result = 0;

    for (int8_t i = startIndex; i < endIndex; i++)
    {
        if (seq_len[i] == 0)
        {
            result = -1;
            continue;
        }

        if (!seq_double[i])
        {
            /* Single buffer: stop, reload and restart */
            if (setDutySequence(static_cast<leg_t>(i), duties,
                                seq_len[i], false) != 0)
            {
                result = -1;
            }
            continue;
        }

        for (uint16_t k = 0; k < seq_len[i]; k++)
        {
            seq_duty[i][k] = duties[k];
            seq_stage[i][k] = dutyToSequenceRaw(i, duties[k]);
        }

        /* Not running: no interrupt will copy the stage, do it now */
        if (!isDutySequenceRunning(static_cast<leg_t>(i)))
        {
            seq_copy_stage_to_all(i);
        }
    }

    return result;
}

uint32_t PowerAPI::getDutySequenceErrors(leg_t leg)
{
    return spin.pwm.getDutySequenceErrors(spinNumberToTu(dt_pwm_pin[leg]));
}
