/*
 * Copyright (c) 2021-2023 LAAS-CNRS
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

/*
 * @date   2023
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 * @author Thomas Walter <thomas.walter@laas.fr>
 */

#include <stm32_ll_hrtim.h>

#include "voltage_mode/hrtim_voltage_mode.h"
#include "current_mode/hrtim_current_mode.h"
#include "leg.h"


// HRTIM interrupts
static const uint8_t HRTIM_IRQ_NUMBER = 67;
static const uint8_t HRTIM_IRQ_PRIO   = 0;
static const uint8_t HRTIM_IRQ_FLAGS  = 0;
static hrtim_callback_t user_callback = NULL;


void _hrtim_init_events(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	if(leg1_tu == TIMA && leg2_tu == TIMB){
		hrtim_adc_trigger_en(1, 1, LL_HRTIM_ADCTRIG_SRC13_TIMACMP3);
		hrtim_adc_trigger_en(3, 2, LL_HRTIM_ADCTRIG_SRC13_TIMBCMP3);
	}else if(leg1_tu == TIMA && leg2_tu == TIMC){
		hrtim_adc_trigger_en(3, 1, LL_HRTIM_ADCTRIG_SRC13_TIMACMP3);
		hrtim_adc_trigger_en(1, 3, LL_HRTIM_ADCTRIG_SRC13_TIMCCMP3);
	}

	hrtim_update_adc_trig_interleaved(1, leg1_tu, leg2_tu);
}

void _hrtim_init_events_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	if(leg1_tu == TIMA && leg2_tu == TIMB){
		// setting the adc roll-over mode on period event
		LL_HRTIM_TIM_SetADCRollOverMode(HRTIM1, LL_HRTIM_TIMER_A, LL_HRTIM_ROLLOVER_MODE_PER);
		LL_HRTIM_TIM_SetADCRollOverMode(HRTIM1, LL_HRTIM_TIMER_B, LL_HRTIM_ROLLOVER_MODE_PER);

		// setting adc trigger
		hrtim_adc_trigger_en(1, 1, LL_HRTIM_ADCTRIG_SRC13_TIMACMP3);
		hrtim_adc_trigger_en(3, 2, LL_HRTIM_ADCTRIG_SRC13_TIMBCMP4);
	}else if(leg1_tu == TIMA && leg2_tu == TIMC){
		// setting the adc roll-over mode on period event
		LL_HRTIM_TIM_SetADCRollOverMode(HRTIM1, LL_HRTIM_TIMER_A, LL_HRTIM_ROLLOVER_MODE_PER);
		LL_HRTIM_TIM_SetADCRollOverMode(HRTIM1, LL_HRTIM_TIMER_C, LL_HRTIM_ROLLOVER_MODE_PER);

		// setting adc trigger
		hrtim_adc_trigger_en(3, 1, LL_HRTIM_ADCTRIG_SRC13_TIMACMP3);
		hrtim_adc_trigger_en(1, 3, LL_HRTIM_ADCTRIG_SRC13_TIMCCMP4);
	}
}

void _hrtim_callback()
{
	LL_HRTIM_ClearFlag_REP(HRTIM1,LL_HRTIM_TIMER_MASTER);
	if (user_callback != NULL)
	{
		user_callback();
	}
}

void hrtim_update_adc_trig_interleaved(uint16_t new_trig, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	if(leg1_tu == TIMA && leg2_tu == TIMB){
		hrtim_cmp_set(0, TIMA, CMP3xR, new_trig);
		hrtim_cmp_set(0, TIMB, CMP3xR, new_trig);
	}else if(leg1_tu == TIMA && leg2_tu == TIMC){
		hrtim_cmp_set(0, TIMA, CMP3xR, new_trig);
		hrtim_cmp_set(0, TIMC, CMP3xR, new_trig);
	}
}

void hrtim_PeriodicEvent_configure(hrtim_tu_t tu_src, uint32_t repetition, hrtim_callback_t callback)
{
	/* Memorize user callback */
	user_callback = callback;

	/* Set repetition counter to repetition-1 so that an event
	 * is triggered every "repetition" number of periods.
	 */
	LL_HRTIM_TIM_SetRepetition(HRTIM1, tu_src, repetition-1);
}

void hrtim_PeriodicEvent_en(hrtim_tu_t tu_src)
{
	LL_HRTIM_EnableIT_REP(HRTIM1, tu_src);         /* Enabling the interrupt on repetition counter event*/

	IRQ_CONNECT(HRTIM_IRQ_NUMBER, HRTIM_IRQ_PRIO, _hrtim_callback, NULL, HRTIM_IRQ_FLAGS);
	irq_enable(HRTIM_IRQ_NUMBER);
}

void hrtim_PeriodicEvent_dis(hrtim_tu_t tu_src)
{
	irq_disable(HRTIM_IRQ_NUMBER);
	LL_HRTIM_DisableIT_REP(HRTIM1, tu_src);        /* Disabling the interrupt on repetition counter event*/
}

void hrtim_PeriodicEvent_SetRep(hrtim_tu_t tu_src, uint32_t repetition)
{
	/* Set repetition counter to repetition-1 so that an event
	 * is triggered every "repetition" number of periods.
	 */
	LL_HRTIM_TIM_SetRepetition(HRTIM1, tu_src, repetition-1);
}

uint32_t hrtim_PeriodicEvent_GetRep(hrtim_tu_t tu_src)
{
	return LL_HRTIM_TIM_GetRepetition(HRTIM1, tu_src)+1;
}

void hrtim_init_current(bool leg1_convention, bool leg2_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	leg_init_CM(leg1_convention, leg2_convention, leg1_tu,  leg2_tu);
	_hrtim_init_events(leg1_tu, leg2_tu);
}

void hrtim_init_voltage_buck(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	leg_init(true,true, leg1_tu, leg2_tu);
	_hrtim_init_events(leg1_tu, leg2_tu);
}

void hrtim_init_voltage_buck_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	leg_init_center_aligned(true,true, leg1_tu, leg2_tu);
	_hrtim_init_events_center_aligned(leg1_tu, leg2_tu);
}

void hrtim_init_voltage_boost(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	leg_init(false,false, leg1_tu, leg2_tu);
	_hrtim_init_events(leg1_tu, leg2_tu);
}

void hrtim_init_voltage_boost_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	leg_init_center_aligned(false,false, leg1_tu, leg2_tu);
	_hrtim_init_events_center_aligned(leg1_tu, leg2_tu);
}

void hrtim_init_voltage_leg1_buck_leg2_boost(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	leg_init(true,false, leg1_tu, leg2_tu);
	_hrtim_init_events(leg1_tu, leg2_tu);
}

void hrtim_init_voltage_leg1_buck_leg2_boost_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	leg_init_center_aligned(true,false, leg1_tu, leg2_tu);
	_hrtim_init_events_center_aligned(leg1_tu, leg2_tu);
}

void hrtim_init_voltage_leg1_boost_leg2_buck(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	leg_init(false,true, leg1_tu, leg2_tu);
	_hrtim_init_events(leg1_tu, leg2_tu);
}

void hrtim_init_voltage_leg1_boost_leg2_buck_center_aligned(hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
	leg_init_center_aligned(false,true, leg1_tu, leg2_tu);
	_hrtim_init_events_center_aligned(leg1_tu, leg2_tu);
}
