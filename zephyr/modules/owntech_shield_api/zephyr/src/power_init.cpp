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

uint32_t timer_frequency = DT_PROP(POWER_SHIELD_ID, default_frequency);

uint16_t dt_pwm_pin[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_PWM_PIN)};

uint16_t dt_pwm_x1_high[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_PWM_X1_HIGH)};

adc_t dt_adc[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_ADC)};

uint32_t dt_adc_decim[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_ADC_DECIM)};

hrtim_cnt_t dt_modulation[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_MODULATION)};

hrtim_adc_edgetrigger_t dt_edge_trigger[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_EDGE_TRIGGER)};

uint16_t dt_rising_deadtime[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_RISING_DT)};

uint16_t dt_falling_deadtime[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_FALLING_DT)};

int16_t dt_phase_shift[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_PHASE)};

uint8_t dt_leg_count =
    DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_COUNTER);

uint8_t dt_output1_inactive[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_OUTPUT1)};

uint8_t dt_output2_inactive[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_OUTPUT2)};

uint16_t dt_current_pin[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_CURRENT_PIN)};

uint16_t dt_pin_driver[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_DRIVER_PIN)};

uint16_t dt_pin_capacitor[] =
    {DT_FOREACH_CHILD_STATUS_OKAY(POWER_SHIELD_ID, LEG_CAPACITOR_PIN)};
