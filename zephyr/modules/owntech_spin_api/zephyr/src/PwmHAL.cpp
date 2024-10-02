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
 * SPDX-License-Identifier: LGPL-2.1
 */

/**
 * @date   2023
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

// OwnTech Power API

// Current file header
#include "PwmHAL.h"
#include "hrtim.h" // PWM management layer by inverter leg interface

void PwmHAL::initUnit(hrtim_tu_number_t pwmX)
{
	hrtim_tu_init(pwmX);
}

void PwmHAL::startDualOutput(hrtim_tu_number_t pwmX)
{
	hrtim_out_en(pwmX);
}

void PwmHAL::stopDualOutput(hrtim_tu_number_t pwmX)
{
	hrtim_out_dis(pwmX);
}

void PwmHAL::startSingleOutput(hrtim_tu_number_t tu, hrtim_output_number_t output)
{
	if (output == TIMING_OUTPUT1)
	{
		switch (tu)
		{
		case PWMA:
			hrtim_out_en_single(PWMA1);
			break;
		case PWMB:
			hrtim_out_en_single(PWMB1);
			break;
		case PWMC:
			hrtim_out_en_single(PWMC1);
			break;
		case PWMD:
			hrtim_out_en_single(PWMD1);
			break;
		case PWME:
			hrtim_out_en_single(PWME1);
			break;
		case PWMF:
			hrtim_out_en_single(PWMF1);
			break;
		default:
			break;
		}
	}
	else if (output == TIMING_OUTPUT2)
	{
		switch (tu)
		{
		case PWMA:
			hrtim_out_en_single(PWMA2);
			break;
		case PWMB:
			hrtim_out_en_single(PWMB2);
			break;
		case PWMC:
			hrtim_out_en_single(PWMC2);
			break;
		case PWMD:
			hrtim_out_en_single(PWMD2);
			break;
		case PWME:
			hrtim_out_en_single(PWME2);
			break;
		case PWMF:
			hrtim_out_en_single(PWMF2);
			break;
		default:
			break;
		}
	}
}

void PwmHAL::stopSingleOutput(hrtim_tu_number_t tu, hrtim_output_number_t output)
{
	if (output == TIMING_OUTPUT1)
	{
		switch (tu)
		{
		case PWMA:
			hrtim_out_dis_single(PWMA1);
			break;
		case PWMB:
			hrtim_out_dis_single(PWMB1);
			break;
		case PWMC:
			hrtim_out_dis_single(PWMC1);
			break;
		case PWMD:
			hrtim_out_dis_single(PWMD1);
			break;
		case PWME:
			hrtim_out_dis_single(PWME1);
			break;
		case PWMF:
			hrtim_out_dis_single(PWMF1);
			break;
		default:
			break;
		}
	}
	else if (output == TIMING_OUTPUT2)
	{
		switch (tu)
		{
		case PWMA:
			hrtim_out_dis_single(PWMA2);
			break;
		case PWMB:
			hrtim_out_dis_single(PWMB2);
			break;
		case PWMC:
			hrtim_out_dis_single(PWMC2);
			break;
		case PWMD:
			hrtim_out_dis_single(PWMD2);
			break;
		case PWME:
			hrtim_out_dis_single(PWME2);
			break;
		case PWMF:
			hrtim_out_dis_single(PWMF2);
			break;
		default:
			break;
		}
	}
}

void PwmHAL::setModulation(hrtim_tu_number_t pwmX, hrtim_cnt_t modulation)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_set_modulation(pwmX, modulation);
}

void PwmHAL::setSwitchConvention(hrtim_tu_number_t pwmX, hrtim_switch_convention_t convention)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_set_switch_convention(pwmX, convention);
}

void PwmHAL::initFrequency(uint32_t init_frequency, uint32_t minimal_frequency)
{
	if (!hrtim_get_status(PWMA))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_frequency_set(init_frequency, minimal_frequency);
}

void PwmHAL::initFrequency(uint32_t init_frequency)
{
	if (!hrtim_get_status(PWMA))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_frequency_set(init_frequency, init_frequency);
}

void PwmHAL::setDeadTime(hrtim_tu_number_t pwmX, uint16_t rise_ns, uint16_t fall_ns)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_dt_set(pwmX, rise_ns, fall_ns);
}

void PwmHAL::setDutyCycle(hrtim_tu_number_t pwmX, float32_t duty_cycle)
{
	uint16_t value = duty_cycle * tu_channel[pwmX]->pwm_conf.period;
	hrtim_duty_cycle_set(pwmX, value);
}

void PwmHAL::setPhaseShift(hrtim_tu_number_t pwmX, int16_t shift)
{
	int16_t phase_shift_degree = shift % 360; // modulo
	if (phase_shift_degree < 0)
		phase_shift_degree += 360; // case of negative phase
	uint16_t period;
	if (pwmX != PWMB)
	{
		period = hrtim_period_get(pwmX);
		if (hrtim_get_modulation(pwmX) == UpDwn)
			period = 2 * period;
	}
	else if (pwmX == PWMB)
	{
		period = hrtim_period_get(PWMA);
	}
	uint16_t phase_shift_int = (period * phase_shift_degree) / 360;
	hrtim_phase_shift_set(pwmX, phase_shift_int);
}

hrtim_cnt_t PwmHAL::getModulation(hrtim_tu_number_t pwmX)
{
	return hrtim_get_modulation(pwmX);
}

hrtim_switch_convention_t PwmHAL::getSwitchConvention(hrtim_tu_number_t pwmX)
{
	return hrtim_get_switch_convention(pwmX);
}

uint16_t PwmHAL::getPeriod(hrtim_tu_number_t pwmX)
{
	return hrtim_period_get(pwmX);
}

void PwmHAL::setAdcTriggerPostScaler(hrtim_tu_number_t pwmX, uint32_t ps_ratio)
{
	hrtim_adc_trigger_set_postscaler(pwmX, ps_ratio);
}

void PwmHAL::enableAdcTrigger(hrtim_tu_number_t pwmX)
{
	uint16_t initial_trigger_value; // initial trigger value when you enable the adc trigger
	hrtim_adc_trigger_en(pwmX);
	if (hrtim_get_modulation(pwmX) == UpDwn)
	{
		initial_trigger_value = 0.06 * hrtim_period_get(pwmX);
		hrtim_tu_cmp_set(pwmX, CMP3xR, initial_trigger_value);
	}
	else
	{
		initial_trigger_value = 100;
		hrtim_tu_cmp_set(pwmX, CMP3xR, initial_trigger_value);
	}
}

void PwmHAL::setAdcTriggerInstant(hrtim_tu_number_t pwmX, float32_t trig_val)
{
	uint16_t triger_value_int = trig_val * hrtim_period_get(pwmX);
	hrtim_tu_cmp_set(pwmX, CMP3xR, triger_value_int);
}

void PwmHAL::disableAdcTrigger(hrtim_tu_number_t tu_number)
{
	hrtim_adc_trigger_dis(tu_number);
}

void PwmHAL::configurePeriodEvnt(hrtim_tu_t PWM_tu, uint32_t repetition, hrtim_callback_t callback)
{
	hrtim_PeriodicEvent_configure(PWM_tu, repetition, callback);
}

void PwmHAL::enablePeriodEvnt(hrtim_tu_t PWM_tu)
{
	hrtim_PeriodicEvent_en(PWM_tu);
}

void PwmHAL::disablePeriodEvnt(hrtim_tu_t PWM_tu)
{
	hrtim_PeriodicEvent_dis(PWM_tu);
}

void PwmHAL::setPeriodEvntRep(hrtim_tu_t PWM_tu, uint32_t repetition)
{
	hrtim_PeriodicEvent_SetRep(PWM_tu, repetition);
}

uint32_t PwmHAL::getPeriodEvntRep(hrtim_tu_t PWM_tu)
{
	return hrtim_PeriodicEvent_GetRep(PWM_tu);
}

uint32_t PwmHAL::getPeriodUs(hrtim_tu_number_t pwmX)
{
	return hrtim_period_get_us(pwmX);
}

void PwmHAL::setAdcEdgeTrigger(hrtim_tu_number_t pwmX, hrtim_adc_edgetrigger_t adc_edge_trigger)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_adc_rollover_set(pwmX, adc_edge_trigger);
}

hrtim_adc_edgetrigger_t PwmHAL::getAdcEdgeTrigger(hrtim_tu_number_t pwmX)
{
	return hrtim_adc_rollover_get(pwmX);
}

void PwmHAL::setAdcTrigger(hrtim_tu_number_t pwmX, hrtim_adc_trigger_t adc_trig)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_adc_triger_set(pwmX, adc_trig);
}

hrtim_adc_trigger_t PwmHAL::getAdcTrigger(hrtim_tu_number_t pwmX, hrtim_adc_trigger_t adc_trig)
{
	return hrtim_adc_triger_get(pwmX);
}

void PwmHAL::setMode(hrtim_tu_number_t pwmX, hrtim_pwm_mode_t mode)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_pwm_mode_set(pwmX, mode);
}

hrtim_pwm_mode_t PwmHAL::getMode(hrtim_tu_number_t pwmX)
{
	return hrtim_pwm_mode_get(pwmX);
}

void PwmHAL::setEev(hrtim_tu_number_t pwmX, hrtim_external_trigger_t eev)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_eev_set(pwmX, eev);
}

hrtim_external_trigger_t PwmHAL::getEev(hrtim_tu_number_t pwmX)
{
	return hrtim_eev_get(pwmX);
}

void PwmHAL::setAdcDecimation(hrtim_tu_number_t pwmX, uint32_t decimation)
{
	if (decimation > 32)
		decimation = 32;
	else if (decimation < 1)
		decimation = 1;
	hrtim_adc_trigger_set_postscaler(pwmX, decimation - 1);
}

void PwmHAL::setFrequency(uint32_t frequency_update)
{
	hrtim_change_frequency(frequency_update);
}

void PwmHAL::initBurstMode()
{
	hrtim_burst_mode_init();
}

void PwmHAL::setBurstMode(int bm_cmp, int bm_per)
{
	hrtim_burst_set(bm_cmp, bm_per);
}

void PwmHAL::startBurstMode()
{
	hrtim_burst_start();
}

void PwmHAL::stopBurstMode()
{
	hrtim_burst_stop();
}

void PwmHAL::deInitBurstMode()
{
	hrtim_burst_dis();
}