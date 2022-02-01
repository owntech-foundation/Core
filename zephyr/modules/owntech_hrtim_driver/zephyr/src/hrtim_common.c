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
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */

#include "voltage_mode/hrtim_voltage_mode.h"
#include "current_mode/hrtim_current_mode.h"
#include "leg.h"


void hrtim_init_current()
{
	hrtim_cm_init();
	hrtim_cm_init_gpio();
	hrtim_cm_enable();
}

void hrtim_init_voltage_buck()
{
	leg_init(true,true);
	hrtim_adc_trigger_en(0, ADC1R, AD13_TAC3);
	hrtim_cmp_set(0, TIMA, CMP3xR, 1);
}

void hrtim_init_voltage_boost()
{
	leg_init(false,false);
	hrtim_adc_trigger_en(0, ADC1R, AD13_TAC3);
	hrtim_cmp_set(0, TIMA, CMP3xR, 1);
}

void hrtim_init_voltage_leg1_buck_leg2_boost()
{
	leg_init(true,false);
	hrtim_adc_trigger_en(0, ADC1R, AD13_TAC3);
	hrtim_cmp_set(0, TIMA, CMP3xR, 1);
}

void hrtim_init_voltage_leg1_boost_leg2_buck()
{
	leg_init(false,true);
	hrtim_adc_trigger_en(0, ADC1R, AD13_TAC3);
	hrtim_cmp_set(0, TIMA, CMP3xR, 1);
}
