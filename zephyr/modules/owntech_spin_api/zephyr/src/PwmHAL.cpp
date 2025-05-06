/*
 * Copyright (c) 2021-present LAAS-CNRS
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
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

/* OwnTech Power API */

/* Current file header */
#include "PwmHAL.h"
/* PWM management layer by inverter leg interface */
#include "hrtim.h"

void PwmHAL::initUnit(hrtim_tu_number_t pwmX)
{
	period[pwmX] = hrtim_tu_init(pwmX);
}

void PwmHAL::startDualOutput(hrtim_tu_number_t pwmX)
{
	hrtim_out_en(pwmX);
}

void PwmHAL::stopDualOutput(hrtim_tu_number_t pwmX)
{
	hrtim_out_dis(pwmX);
}

void PwmHAL::startSingleOutput(hrtim_tu_number_t tu,
							   hrtim_output_number_t output)
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

void PwmHAL::stopSingleOutput(hrtim_tu_number_t tu,
							  hrtim_output_number_t output)
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
	{
		hrtim_init_default_all(); /* Initialize default parameters before */
	}

	hrtim_set_modulation(pwmX, modulation);
}

void PwmHAL::setSwitchConvention(hrtim_tu_number_t pwmX,
                                 hrtim_switch_convention_t convention) 
{
	if (!hrtim_get_status(pwmX))
	{
		hrtim_init_default_all(); /* Initialize default parameters before */
	}

	hrtim_set_switch_convention(pwmX, convention);
}

void PwmHAL::initVariableFrequency(uint32_t initial_frequency,
								   uint32_t minimal_frequency)
{
	if (!hrtim_get_status(PWMA))
		/* Initialize default parameters before */
		hrtim_init_default_all();
	hrtim_frequency_set(initial_frequency, minimal_frequency);
}

void PwmHAL::initFixedFrequency(uint32_t fixed_frequency)
{
	if (!hrtim_get_status(PWMA))
		/* Initialize default parameters before */
		hrtim_init_default_all();
	hrtim_frequency_set(fixed_frequency, fixed_frequency);
}

void PwmHAL::setDeadTime(hrtim_tu_number_t pwmX,
						 uint16_t rise_ns,
						 uint16_t fall_ns)
{
	if (!hrtim_get_status(pwmX))
	{
		hrtim_init_default_all(); /* Initialize default parameters before */
	}

	hrtim_dt_set(pwmX, rise_ns, fall_ns);
}

void PwmHAL::setDutyCycle(hrtim_tu_number_t pwmX, float32_t duty_cycle)
{
	uint16_t period = tu_channel[pwmX]->pwm_conf.period;
	uint16_t value = duty_cycle * period;

	setDutyCycleRaw(pwmX,value);

}

void PwmHAL::setDutyCycleRaw(hrtim_tu_number_t pwmX, uint16_t duty_cycle)
{
    timer_hrtim_t *tu = tu_channel[pwmX]; /* Get pointer to timer unit config */
    uint16_t previous_duty_cycle = tu->pwm_conf.duty_cycle; 
    /* Store previously set duty cycle */

    if (previous_duty_cycle == duty_cycle) {
        return; /* Skip update if duty cycle has not changed */
    }

    uint16_t period = tu->pwm_conf.period;         /* Get PWM period */
    bool swap_state = tu->pwm_conf.duty_swap;      /* Get output swap state */

    /* True if near 100% duty */
    bool over_limit = (duty_cycle >= period - 3); 

    /* Force 0% to avoid glitches near 100% */
    duty_cycle = over_limit ? 0 : duty_cycle; 

    /* Flip outputs only if needed to preserve polarity */
    bool force_swap = swap_state ^ over_limit; 

    /* Write new duty cycle to HRTIM registers */
    hrtim_duty_cycle_set(pwmX, duty_cycle); 

    if (force_swap) {
        hrtim_output_hot_swap(pwmX); 
        /* Flip the outputs (high ↔ low) */
    }
}



void PwmHAL::setPhaseShift(hrtim_tu_number_t pwmX, int16_t shift)
{
	/* modulo */
	int16_t phase_shift_degree = shift % 360;
	if (phase_shift_degree < 0)
		/* case of negative phase */
		phase_shift_degree += 360;
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

hrtim_cnt_t PwmHAL::getModulation(hrtim_tu_number_t pwmX) {
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

uint16_t PwmHAL::getPeriodMax(hrtim_tu_number_t pwmX)
{
	return hrtim_get_max_period(pwmX);
}

uint16_t PwmHAL::getPeriodMin(hrtim_tu_number_t pwmX)
{
	return hrtim_get_min_period(pwmX);
}

void PwmHAL::setAdcTriggerPostScaler(hrtim_tu_number_t pwmX, uint32_t ps_ratio)
{
	
	if (!hrtim_get_status(pwmX))
	{
		hrtim_init_default_all(); /* Initialize default parameters before */
	}

	hrtim_adc_trigger_set_postscaler(pwmX, ps_ratio);
}

void PwmHAL::enableAdcTrigger(hrtim_tu_number_t pwmX)
{
	/* Initial trigger value when you enable the adc trigger */
	uint16_t initial_trigger_value;
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
	if (!hrtim_get_status(pwmX))
	{
		hrtim_init_default_all(); /* Initialize default parameters before */
	}

	uint16_t trigger_value_int = trig_val * hrtim_period_get(pwmX);
	hrtim_tu_cmp_set(pwmX, CMP3xR, trigger_value_int);
}

void PwmHAL::disableAdcTrigger(hrtim_tu_number_t tu_number)
{
	hrtim_adc_trigger_dis(tu_number);
}

void PwmHAL::configurePeriodEvnt(hrtim_tu_t PWM_tu,
								 uint32_t repetition,
								 hrtim_callback_t callback)
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

void PwmHAL::setAdcEdgeTrigger(hrtim_tu_number_t pwmX,
							   hrtim_adc_edgetrigger_t adc_edge_trigger)
{
	if (!hrtim_get_status(pwmX))
	{
		hrtim_init_default_all(); /* Initialize default parameters before */
	}

	hrtim_adc_rollover_set(pwmX, adc_edge_trigger);
}

hrtim_adc_edgetrigger_t PwmHAL::getAdcEdgeTrigger(hrtim_tu_number_t pwmX)
{
	return hrtim_adc_rollover_get(pwmX);
}

void PwmHAL::setAdcTrigger(hrtim_tu_number_t pwmX, adc_t adc)
{
	// Get trigger depending on ADC number,
	// and make sure the ADC is correct.
	hrtim_adc_trigger_t adc_trig;
	switch(adc)
	{
		case ADC_1:
			adc_trig = ADCTRIG_1;
			break;
		case ADC_2:
			adc_trig = ADCTRIG_3;
			break;
		case ADC_3:
			adc_trig = ADCTRIG_5;
			break;
		case ADC_4:
			adc_trig = ADCTRIG_7;
			break;
		case ADC_5:
			adc_trig = ADCTRIG_9;
			break;
		case UNKNOWN_ADC:
		case DEFAULT_ADC:
		default:
			return;
	}

	if (!hrtim_get_status(pwmX))
	{
		hrtim_init_default_all(); /* Initialize default parameters before */
	}

	hrtim_adc_trigger_set(pwmX, adc_trig);
}

adc_t PwmHAL::getAdcTrigger(hrtim_tu_number_t pwmX)
{
	hrtim_adc_trigger_t adc_trig = hrtim_adc_trigger_get(pwmX);

	switch(adc_trig)
	{
		case ADCTRIG_1:
			return ADC_1;
			break;
		case ADCTRIG_3:
			return ADC_2;
			break;
		case ADCTRIG_5:
			return ADC_3;
			break;
		case ADCTRIG_7:
			return ADC_4;
			break;
		case ADCTRIG_9:
			return ADC_5;
			break;
		default:
			return UNKNOWN_ADC;
	}
}

void PwmHAL::setMode(hrtim_tu_number_t pwmX, hrtim_pwm_mode_t mode)
{
	if (!hrtim_get_status(pwmX))
	{
		hrtim_init_default_all(); /* Initialize default parameters before */
	}

	hrtim_pwm_mode_set(pwmX, mode);
}

hrtim_pwm_mode_t PwmHAL::getMode(hrtim_tu_number_t pwmX)
{
	return hrtim_pwm_mode_get(pwmX);
}

void PwmHAL::setEev(hrtim_tu_number_t pwmX, hrtim_external_trigger_t eev)
{
	if (!hrtim_get_status(pwmX))
	{
		hrtim_init_default_all(); /* Initialize default parameters before */
	}

	hrtim_eev_set(pwmX, eev);
}

hrtim_external_trigger_t PwmHAL::getEev(hrtim_tu_number_t pwmX)
{
	return hrtim_eev_get(pwmX);
}

void PwmHAL::setAdcDecimation(hrtim_tu_number_t pwmX, uint32_t decimation)
{
	if (!hrtim_get_status(pwmX))
	{
		hrtim_init_default_all(); /* Initialize default parameters before */
	}


	if (decimation > 32)
		decimation = 32;
	else if (decimation < 1)
		decimation = 1;
	hrtim_adc_trigger_set_postscaler(pwmX, decimation - 1);
}

void PwmHAL::setFrequency(uint32_t frequency_update)
{
	if (!hrtim_get_status(PWMA))
	/* Initialize default parameters before */
		hrtim_init_default_all();

	hrtim_change_frequency(frequency_update);
}

uint32_t PwmHAL::getFrequencyMax(hrtim_tu_number_t pwmX)
{
	return hrtim_get_max_frequency(pwmX);
}

uint32_t PwmHAL::getFrequencyMin(hrtim_tu_number_t pwmX)
{
	return hrtim_get_min_frequency(pwmX);
}

uint32_t PwmHAL::getResolutionPs(hrtim_tu_number_t pwmX)
{
	return hrtim_get_resolution_ps(pwmX);
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