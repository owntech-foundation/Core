/*
 * Copyright (c) 2020-2023 LAAS-CNRS
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
 * @file
 * @brief   PWM management layer by inverter leg
 * @date    2023
 * @author  Hugues Larrive <hugues.larrive@laas.fr>
 * @author  Antoine Boche <antoine.boche@laas.fr>
 * @author  Luiz Villa <luiz.villa@laas.fr>
 * @author  Clément Foucher <clement.foucher@laas.fr>
 * @author  Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#include "leg.h"
#include "owntech_leg_driver.h"
#include "hrtim_voltage_mode.h"
#include "../current_mode/hrtim_current_mode.h"

// Saturation values used for the PWM duty cycle
#define LOW_DUTY 0.03
#define HIGH_DUTY 0.97


static uint16_t period, min_pw, max_pw, dead_time;

// default configurations of the HRTIMER
static uint32_t  frequency = 200000;
static float32_t min_duty_cycle = 0.1;
static float32_t max_duty_cycle = 0.9;


static leg_conf_t leg_conf[6]; /* a copy of leg_config with index
                                * corresponding to timing unit */

static uint8_t _TU_num(hrtim_tu_t tu){

    switch(tu){
        case TIMA:
            return 0;

        case TIMB:
             return 1;

        case TIMC:
             return 2;

        case TIMD:
             return 3;
             break;

        case TIME:
             return 4;

        case TIMF:
             return 5;

        default:
            return 100;

    }
}

/**
 * This function Initialize the hrtim and all the legs
 * with the chosen convention for the switch controlled
 * on the power converter to a frequency of 200kHz
 * Must be initialized in first position
 */
uint16_t leg_init(bool leg1_upper_switch_convention, bool leg2_upper_switch_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
    /* ensures that timing_unit can be used as leg identifier */
    for (unsigned int i = 0; i < LEG_NUMOF; i++)
    {
        leg_conf[_TU_num(leg_config[i].timing_unit)] = leg_config[i];
    }

    period = hrtim_init(0, &frequency, LEG_DEFAULT_DT,leg1_upper_switch_convention,leg2_upper_switch_convention, leg1_tu, leg2_tu);
    dead_time = (uint16_t)((((double)period)*LEG_DEFAULT_DT*((double)frequency))/1000000000.0); //this line is overflow safe
    min_pw = (period * 0.1) + dead_time;
    max_pw = (period * 0.9) + dead_time;
    return period;
}

/**
 * This function Initialize the hrtim and all the legs
 * with the chosen convention for the switch controlled
 * on the power converter to a frequency of 200kHz
 * with the counter on up-down mode (center-alligned)
 * Must be initialized in first position
 */
uint16_t leg_init_center_aligned(bool leg1_upper_switch_convention, bool leg2_upper_switch_convention,hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
    /* ensures that timing_unit can be used as leg identifier */
    for (unsigned int i = 0; i < LEG_NUMOF; i++)
    {
        leg_conf[_TU_num(leg_config[i].timing_unit)] = leg_config[i];
    }

    period = hrtim_init_updwn(0, &frequency, LEG_DEFAULT_DT,leg1_upper_switch_convention,leg2_upper_switch_convention, leg1_tu, leg2_tu);
    dead_time = (uint16_t)((((double)period)*LEG_DEFAULT_DT*((double)frequency))/1000000000.0); //this line is overflow safe
    min_pw = (period * min_duty_cycle) + dead_time;
    max_pw = (period * max_duty_cycle) + dead_time;
    return period;
}

/**
 * This function Initialize in current mode the hrtim and all the legs
 * with the chosen convention for the switch controlled on the power 
 * converter to a frequency of 200kHz
 */
uint16_t leg_init_CM(bool leg1_upper_switch_convention, bool leg2_upper_switch_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
    /* ensures that timing_unit can be used as leg identifier */
    for (unsigned int i = 0; i < LEG_NUMOF; i++)
    {
        leg_conf[_TU_num(leg_config[i].timing_unit)] = leg_config[i];
    }
    
    period = CM_hrtim_init(&frequency, LEG_DEFAULT_DT,leg1_upper_switch_convention,leg2_upper_switch_convention, leg1_tu, leg2_tu);

    return period; 
}

void leg_set(hrtim_tu_t timing_unit, uint16_t pulse_width, uint16_t phase_shift)
{
    //addition of the dead time for the rectification of the centered dead time configuration cf:hrtim_pwm_dt()
    pulse_width = pulse_width+dead_time;

    //Second check for duty cycle saturation
    if (pulse_width<min_pw)
    {
        pulse_width = min_pw;
    }
    else if (pulse_width > max_pw)
    {
        pulse_width = max_pw;
    }

    hrtim_pwm_set(  leg_conf[_TU_num(timing_unit)].hrtim,
                    timing_unit,
                    pulse_width,
                    phase_shift);
    /* save the pulse_width */
    leg_conf[_TU_num(timing_unit)].pulse_width = pulse_width;
}

void CM_leg_set(hrtim_tu_t timing_unit, uint16_t phase_shift)
{
    CM_hrtim_pwm_set( timing_unit,phase_shift);
}

void leg_set_dt(hrtim_tu_t timing_unit, uint16_t rise_ns, uint16_t fall_ns)
{
    //addition of the dead time for the rectification of the centered dead time configuration cf:hrtim_pwm_dt()
    hrtim_pwm_dt(leg_conf[_TU_num(timing_unit)].hrtim, leg_conf[_TU_num(timing_unit)].timing_unit, rise_ns, fall_ns);

    /* save the pulse_width */
    leg_conf[_TU_num(timing_unit)].rise_dead_time = rise_ns;
    leg_conf[_TU_num(timing_unit)].fall_dead_time = fall_ns;
}

void leg_stop(hrtim_tu_t timing_unit)
{
    hrtim_out_dis(leg_conf[_TU_num(timing_unit)].hrtim, leg_conf[_TU_num(timing_unit)].timing_unit, OUT1);
    hrtim_out_dis(leg_conf[_TU_num(timing_unit)].hrtim, leg_conf[_TU_num(timing_unit)].timing_unit, OUT2);
}

void leg_start(hrtim_tu_t timing_unit)
{
    hrtim_out_en(leg_conf[_TU_num(timing_unit)].hrtim, leg_conf[_TU_num(timing_unit)].timing_unit, OUT1);
    hrtim_out_en(leg_conf[_TU_num(timing_unit)].hrtim, leg_conf[_TU_num(timing_unit)].timing_unit, OUT2);
}

uint16_t leg_period(void)
{
    return period;
}

uint32_t leg_get_period_us()
{
    return period*184e-6;
}

uint8_t leg_numof(void)
{
    return LEG_NUMOF;
}

leg_conf_t leg_get_conf(uint8_t leg)
{
    return leg_conf[leg_config[leg].timing_unit];
}

uint32_t leg_get_freq(void)
{
    return frequency;
}

void leg_set_freq(uint32_t frequency_Hz)
{
    frequency = frequency_Hz;
}

void leg_set_min_duty_cycle(float32_t duty_cycle)
{
    if (duty_cycle>=LOW_DUTY) {
        min_duty_cycle = duty_cycle;
    }else{
        min_duty_cycle = LOW_DUTY;
    }
}

void leg_set_max_duty_cycle(float32_t duty_cycle)
{
    if (duty_cycle<=HIGH_DUTY) {
        max_duty_cycle = duty_cycle;
    }else{
        max_duty_cycle = HIGH_DUTY;
    }
}
