/*
 * Copyright (c) 2020 LAAS-CNRS
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
 * @date    2020
 * @author  Hugues Larrive <hugues.larrive@laas.fr>
 * @author  Antoine Boche <antoine.boche@laas.fr>
 */

#include "leg.h"
#include "owntech_leg_driver.h"

static uint16_t period, min_pw, max_pw, dead_time;

static leg_conf_t leg_conf[6]; /* a copy of leg_config with index
                                * corresponding to timing unit */

/**
 * This function Initialize the hrtim and all the legs
 * with the chosen convention for the switch controlled
 * on the power converter to a frequency of 200kHz
 * Must be initialized in first position
 */
uint16_t leg_init(bool upper_switch_convention)
{
    uint32_t freq = LEG_FREQ;

    /* ensures that timing_unit can be used as leg identifier */
    for (int i = 0; i < LEG_NUMOF; i++) 
    {
        leg_conf[leg_config[i].timing_unit] = leg_config[i];
    }

    period = hrtim_init(0, &freq, LEG_DEFAULT_DT,upper_switch_convention);
    dead_time = (period*LEG_DEFAULT_DT*leg_get_freq())/1000000;
    min_pw = (period * 0.1) + dead_time;
    max_pw = (period * 0.9) + dead_time;
    return period;
}

void leg_set(hrtim_tu_t timing_unit, uint16_t pulse_width, uint16_t phase_shift)
{
    //addition of the dead time for the rectification of the centered dead time configuration cf:hrtim_pwm_dt()
    pulse_width = pulse_width+dead_time;

    //Second check for duty cycle saturation
    if (pulse_width<min_pw)
    {
        pulse_width = max_pw;
    }
    else if (pulse_width > max_pw)
    {
        pulse_width = max_pw;
    }
    
    hrtim_pwm_set(  leg_conf[timing_unit].hrtim,
                    timing_unit,
                    pulse_width,
                    phase_shift);
    /* save the pulse_width */
    leg_conf[timing_unit].pulse_width = pulse_width;
}

void leg_stop(hrtim_tu_t timing_unit)
{
    hrtim_pwm_set(  leg_conf[timing_unit].hrtim,
                    leg_conf[timing_unit].timing_unit,
                    0,
                    0);
}

uint16_t leg_period(void)
{
    return period;
}

uint8_t leg_numof(void)
{
    return LEG_NUMOF;
}

leg_conf_t leg_get_conf(uint8_t leg)
{
    return leg_conf[leg_config[leg].timing_unit];
}

uint16_t leg_get_freq(void)
{
    return LEG_FREQ / 1000;
}
