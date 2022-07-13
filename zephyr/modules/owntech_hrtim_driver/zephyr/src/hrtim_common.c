/*
 * Copyright (c) 2021-2022 LAAS-CNRS
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
 * @date   2022
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#include <stm32_ll_hrtim.h>

#include "voltage_mode/hrtim_voltage_mode.h"
#include "current_mode/hrtim_current_mode.h"
#include "leg.h"


void _hrtim_init_events()
{
	hrtim_adc_trigger_en(1, 1, LL_HRTIM_ADCTRIG_SRC13_TIMACMP3);
	hrtim_adc_trigger_en(3, 2, LL_HRTIM_ADCTRIG_SRC13_TIMBCMP4);
	hrtim_update_adc_trig_interleaved(1);
}

void _hrtim_init_events_center_aligned()
{
	
	// setting the adc roll-over mode on period event
	LL_HRTIM_TIM_SetADCRollOverMode(HRTIM1, LL_HRTIM_TIMER_A, LL_HRTIM_ROLLOVER_MODE_PER);
	LL_HRTIM_TIM_SetADCRollOverMode(HRTIM1, LL_HRTIM_TIMER_B, LL_HRTIM_ROLLOVER_MODE_PER);
	
	// setting adc trigger
	hrtim_adc_trigger_en(1, 1, LL_HRTIM_ADCTRIG_SRC13_TIMAPER);
	hrtim_adc_trigger_en(3, 2, LL_HRTIM_ADCTRIG_SRC13_TIMBPER);
}

void hrtim_update_adc_trig_interleaved(uint16_t new_trig)
{
	hrtim_cmp_set(0, TIMA, CMP3xR, new_trig);
	hrtim_cmp_set(0, TIMB, CMP4xR, new_trig);
}


void hrtim_init_current()
{
	hrtim_cm_init();
	hrtim_cm_init_gpio();
	hrtim_cm_enable();
}

void hrtim_init_voltage_buck()
{
	leg_init(true,true);
	_hrtim_init_events();
}

void hrtim_init_voltage_buck_center_aligned()
{
	leg_init_center_aligned(true,true);
	_hrtim_init_events_center_aligned();
}

void hrtim_init_voltage_boost()
{
	leg_init(false,false);
	_hrtim_init_events();
}

void hrtim_init_voltage_boost_center_aligned()
{
	leg_init_center_aligned(false,false);
	_hrtim_init_events_center_aligned();
}

void hrtim_init_voltage_leg1_buck_leg2_boost()
{
	leg_init(true,false);
	_hrtim_init_events();
}

void hrtim_init_voltage_leg1_buck_leg2_boost_center_aligned()
{
	leg_init_center_aligned(true,false);
	_hrtim_init_events_center_aligned();
}

void hrtim_init_voltage_leg1_boost_leg2_buck()
{
	leg_init(false,true);
	_hrtim_init_events();
}

void hrtim_init_voltage_leg1_boost_leg2_buck_center_aligned()
{
	leg_init_center_aligned(false,true);
	_hrtim_init_events_center_aligned();
}
