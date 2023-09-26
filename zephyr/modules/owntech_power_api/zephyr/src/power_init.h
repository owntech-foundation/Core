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
 * @brief This file save all the parameters set in the device tree
 */

#ifndef POWER_INIT_H_
#define POWER_INIT_H_

#include "zephyr.h"
#include "hrtim.h"

/* Define a macro LEG_PWM_PIN that retrieves the PWM gpio pin
 property from the Device Tree node with the given 'node_id'. */
#define LEG_PWM_PIN(node_id)	DT_PROP(node_id, pwm_pin_num),

/* Define a macro LEG_CURRENT_PIN that retrieves the pin on which we have current measure
property from the Device Tree node with the given 'node_id'. */
#define LEG_CURRENT_PIN(node_id)	DT_PROP(node_id, current_pin_num),

/* Define a macro LEG_ADC that retrieves the 'adc_trigger'
property from the Device Tree node with the given 'node_id'. */
#define LEG_ADC(node_id)	DT_STRING_TOKEN(node_id, adc_trigger),

/* Define a macro LEG_ROLLOVER that retrieves the roll_over (ie. when the adc is triggered)
property from the Device Tree node with the given 'node_id'. */
#define LEG_EDGE_TRIGGER(node_id)	DT_STRING_TOKEN(node_id, edge_trigger),

/* Define a macro LEG_MODULATION that retrieves the modulation type
property from the Device Tree node with the given 'node_id'. */
#define LEG_MODULATION(node_id)	DT_STRING_TOKEN(node_id, modulation),

/* Define a macro LEG_RISING_DT that retrieves the first element of the 'dead_time'
property (ie. rising dead time) from the Device Tree node with the given 'node_id'. */
#define LEG_RISING_DT(node_id)	DT_PROP_BY_IDX(node_id, default_dead_time, 0),

/* Define a macro LEG_RISING_DT that retrieves the second element of the 'dead_time'
property (ie. falling dead time) from the Device Tree node with the given 'node_id'. */
#define LEG_FALLING_DT(node_id)	DT_PROP_BY_IDX(node_id, default_dead_time, 1),

/* Define a macro LEG_PHASE that retrieves the phase shift
property from the Device Tree node with the given 'node_id'. */
#define LEG_PHASE(node_id)  DT_PROP(node_id, default_phase_shift),

/* Define a macro LEG_ADC_DECIM that retrieves the adc decimator
property from the Device Tree node with the given 'node_id'. */
#define LEG_ADC_DECIM(node_id)  DT_PROP(node_id, default_adc_decim),

/* Define a macro LEG_OUTPUT1 that retrieves the status of output1
 property from the Device Tree node with the given 'node_id'. */
#define LEG_OUTPUT1(node_id) DT_PROP(node_id, output1_inactive),

/* Define a macro LEG_OUTPUT2 that retrieves the status of output2
 property from the Device Tree node with the given 'node_id'. */
#define LEG_OUTPUT2(node_id) DT_PROP(node_id, output2_inactive),

#define LEG_COUNTER(node_id) +1 // this macro is needed to count the number of leg in the device tree

// the shield node identifier in the device tree
#define POWER_SHIELD_ID           DT_NODELABEL(power_shield)

#ifdef __cplusplus
extern "C" {
#endif

/* Enum to define which DAC to use for current mode */
typedef enum{
    CM_DAC_NONE = 0,
    CM_DAC1 = 25,
    CM_DAC3 = 30,
}cm_dac_t;

/* Define a variable 'timer_frequency' and initialize it with the 'frequency' property
from the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern uint32_t timer_frequency;

/* Define an array 'dt_timing' of type 'hrtim_tu_number_t' and initialize it with an array of 'timing_unit'
properties from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern uint16_t dt_pwm_pin[];

/* Define an array 'dt_adc' of type 'hrtim_adc_trigger_t' and initialize it with an array of
'adc_trigger' property from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern hrtim_adc_trigger_t dt_adc[];

/* Define an array 'dt_adc_decim' of type 'uint32_t' and initialize it with an array of
'adc_decim' property from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern uint32_t dt_adc_decim[];

/* Define an array 'dt_modulation' of type 'hrtim_cnt_t' and initialize it with an array of
'modulation' property from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern hrtim_cnt_t dt_modulation[];

/* Define an array 'dt_rollover' of type 'hrtim_adc_rollover_t' and initialize it with an array of
'roll_over' property from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern hrtim_adc_edgetrigger_t dt_edge_trigger[];

/* Define an array 'dt_rising_deadtime' of type 'uint16_t' and initialize it with an array of
rising dead time values from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern uint16_t dt_rising_deadtime[];

/* Define an array 'dt_falling_deadtime' of type 'uint16_t' and initialize it with an array of
falling dead time values from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern uint16_t dt_falling_deadtime[];

/* Define an array 'dt_phase_shift' of type 'int16_t' and initialize it with an array of
'phase_shift' property from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern int16_t dt_phase_shift[];

/* Define a variable 'dt_leg_count' and initialize it with the
count of children with status 'OKAY' under the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern uint8_t dt_leg_count;

/* Define an array 'dt_output1_inactive' of type 'uint8_t' and initialize it with an array of
'output1_inactive' values from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern uint8_t dt_output1_inactive[];

/* Define an array 'dt_output2_inactive' of type 'uint8_t' and initialize it with an array of
'output2_inactive' values from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern uint8_t dt_output2_inactive[];

/* Define an array 'dt_current_pin' of type 'uint16' and initialize it with an array of
'curent_pin_num' property from the children of the Device Tree node with the ID 'POWER_SHIELD_ID'. */
extern uint16_t dt_current_pin[];

#ifdef __cplusplus
}
#endif

#endif // POWER_H_
