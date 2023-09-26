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

/**
 * @date   2023
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

// OwnTech Power API

// Current file header
#include "HardwareConfiguration.h"
#include "hrtim.h" // PWM management layer by inverter leg interface

/**
 * @brief     This function initializes a timing unit
 *
 * @param[in] pwmX    PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 */
void HardwareConfiguration::pwmInit(hrtim_tu_number_t pwmX)
{
	hrtim_tu_init(pwmX);
}

/**
 * @brief     This fonction starts both outputs of the selected HRTIM channel
 *
 * @param[in] pwmX    PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 */
void HardwareConfiguration::pwmStart(hrtim_tu_number_t pwmX)
{
	hrtim_out_en(pwmX);
}

/**
 * @brief     This function stops both outputs of the selected HRTIM channel
 *
 * @param[in] pwmX    PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 */
void HardwareConfiguration::pwmStop(hrtim_tu_number_t pwmX)
{
	hrtim_out_dis(pwmX);
}

/**
 * @brief     This function starts only one output of the selected HRTIM channel
 *
 * @param[in] tu		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME, PWMF
 * @param[in] output	output to disable - TIMING_OUTPUT1, TIMING_OUTPUT2
 */
void HardwareConfiguration::pwmStartSubUnit(hrtim_tu_number_t tu, hrtim_output_number_t output)
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

/**
 * @brief     This function starts only one output of the selected HRTIM channel
 *
 * @param[in] tu		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME, PWMF
 * @param[in] output	output to disable - TIMING_OUTPUT1, TIMING_OUTPUT2
 */
void HardwareConfiguration::pwmStopSubUnit(hrtim_tu_number_t tu, hrtim_output_number_t output)
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

/**
 * @brief     This function sets the modulation mode for a given PWM unit
 *
 * @param[in] pwmX    PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] modulation    PWM Modulation - Lft_aligned or UpDwn
 *
 * @warning this function must be called BEFORE initializing the selected timer
 */
void HardwareConfiguration::pwmSetModulation(hrtim_tu_number_t pwmX, hrtim_cnt_t modulation)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_set_modulation(pwmX, modulation);
}

/**
 * @brief     This function sets the switch convention for a given PWM unit
 * 			  i.e. you decide which one of the output of the timer can be controlled
 * 			  with duty cycle.
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] convention    PWM Switch to be driven by the duty cycle. The other will be complementary - PWMx1 or PWMx2
 *
 * @warning this function must be called before the timer initialization
 */
void HardwareConfiguration::pwmSetSwitchConvention(hrtim_tu_number_t pwmX, hrtim_switch_convention_t convention)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_set_switch_convention(pwmX, convention);
}

/**
 * @brief     This function sets the frequency
 *
 * @param[in] value frequency in Hz
 *
 * @warning this function must be called BEFORE initialiazing any timing unit
 */
void HardwareConfiguration::pwmSetFrequency(uint32_t value)
{
	if (!hrtim_get_status(PWMA))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_frequency_set(value);
}

/**
 * @brief     This function sets the dead time for the selected timing unit
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] rise_ns		rising edge dead time in ns
 * @param[in] falling_ns	falling edge dead time in ns
 *
 * @warning use this function AFTER initializing the chosen timer
 */
void HardwareConfiguration::pwmSetDeadTime(hrtim_tu_number_t pwmX, uint16_t rise_ns, uint16_t fall_ns)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_dt_set(pwmX, rise_ns, fall_ns);
}

/**
 * @brief     This function sets the duty cycle for the selected timing unit
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] value			duty cycle value
 */
void HardwareConfiguration::pwmSetDutyCycle(hrtim_tu_number_t pwmX, float32_t duty_cycle)
{
	uint16_t value = duty_cycle * tu_channel[pwmX]->pwm_conf.period;
	hrtim_duty_cycle_set(pwmX, value);
}

/**
 * @brief     This function sets the phase shift in respect to timer A for the selected timing unit
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] shift			phase shift value ° between -360 and 360
 *
 * @warning use this function AFTER setting the frequency and initializing the chosen timer
 */
void HardwareConfiguration::pwmSetPhaseShift(hrtim_tu_number_t pwmX, int16_t shift)
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

/**
 * @brief     This function returns the modulation type of the selected timing unit
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @return 					Lft_aligned or UpDwn (center aligned)
 */
hrtim_cnt_t HardwareConfiguration::pwmGetModulation(hrtim_tu_number_t pwmX)
{
	return hrtim_get_modulation(pwmX);
}

/**
 * @brief     This function returns the switching convention of the selected timing unit
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @return 					PWMx1 (high side convention) or PWMx2 (low-side convention)
 */
hrtim_switch_convention_t HardwareConfiguration::pwmGetSwitchConvention(hrtim_tu_number_t pwmX)
{
	return hrtim_get_switch_convention(pwmX);
}

/**
 * @brief     This function returns the period of the selected timing unit
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @return 					the period value in uint16
 */
uint16_t HardwareConfiguration::pwmGetPeriod(hrtim_tu_number_t pwmX)
{
	return hrtim_period_get(pwmX);
}

/**
 * @brief     This function sets the PostScaler value for the selected timing unit
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] ps_ratio		post scaler ratio
 *
 * @warning   this function must be called after initialiazing a timing unit, and before
 * 			   enabling the adc trigger
 */
void HardwareConfiguration::pwmSetAdcTriggerPostScaler(hrtim_tu_number_t pwmX, uint32_t ps_ratio)
{
	hrtim_adc_trigger_set_postscaler(pwmX, ps_ratio);
}

/**
 * @brief     This function enables the adc trigger for the selected timing unit
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 *
 * @warning  call this function only AFTER setting the adc trigger and initializing the chosen timer
 */
void HardwareConfiguration::pwmAdcTriggerEnable(hrtim_tu_number_t pwmX)
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

/**
 * @brief     This function sets the comparator value at which the ADC is trigered
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] trig_val    	a value between 0 and 1
 */
void HardwareConfiguration::pwmSetAdcTriggerInstant(hrtim_tu_number_t pwmX, float32_t trig_val)
{
	uint16_t triger_value_int = trig_val * hrtim_period_get(pwmX);
	hrtim_tu_cmp_set(pwmX, CMP3xR, triger_value_int);
}

/**
 * @brief     This function disables the adc trigger for the selected timing unit
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 */
void HardwareConfiguration::pwmAdcTriggerDisable(hrtim_tu_number_t tu_number)
{
	hrtim_adc_trigger_dis(tu_number);
}

/**
 * @brief     This function configures the interrupt on repetition counter
 *
 * @param[in] PWM_tu    	PWM Unit - TIMA, TIMB, TIMC, TIMD, TIME or TIMF
 * @param[in] repetition 	number of repetition before the interruption on repetition counter event
 * @param[in] callback		function to call each interupt
 */
void HardwareConfiguration::pwmPeriodEvntConf(hrtim_tu_t PWM_tu, uint32_t repetition, hrtim_callback_t callback)
{
	hrtim_PeriodicEvent_configure(PWM_tu, repetition, callback);
}

/**
 * @brief     This function enables the interrupt on repetition counter
 *
 * @param[in] PWM_tu    	PWM Unit - TIMA, TIMB, TIMC, TIMD, TIME or TIMF
 */
void HardwareConfiguration::pwmPeriodEvntEnable(hrtim_tu_t PWM_tu)
{
	hrtim_PeriodicEvent_en(PWM_tu);
}

/**
 * @brief     This function disables the interrupt on repetition counter
 *
 * @param[in] PWM_tu    	PWM Unit - TIMA, TIMB, TIMC, TIMD, TIME or TIMF
 */
void HardwareConfiguration::pwmPeriodEvntDisable(hrtim_tu_t PWM_tu)
{
	hrtim_PeriodicEvent_dis(PWM_tu);
}

/**
 * @brief     This function sets the repetition counter to ISR period
 *
 * @param[in] PWM_tu    	PWM Unit - TIMA, TIMB, TIMC, TIMD, TIME or TIMF
 * @param[in] repetition 	number of repetition before the interruption on repetition counter event
 */
void HardwareConfiguration::pwmSetPeriodEvntRep(hrtim_tu_t PWM_tu, uint32_t repetition)
{
	hrtim_PeriodicEvent_SetRep(PWM_tu, repetition);
}

/**
 * @brief     This function returns the repetition counter value
 *
 * @param[in] PWM_tu    	PWM Unit - TIMA, TIMB, TIMC, TIMD, TIME or TIMF
 * @return 					repetition counter value
 */
uint32_t HardwareConfiguration::pwmGetPeriodEvntRep(hrtim_tu_t PWM_tu)
{
	return hrtim_PeriodicEvent_GetRep(PWM_tu);
}

/**
 * @brief     				This function returns the period in µs of the selected timer
 *
 * @param[in] pwmX    	PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 */
uint32_t HardwareConfiguration::pwmGetPeriodUs(hrtim_tu_number_t pwmX)
{
	return hrtim_period_get_us(pwmX);
}

/**
 * @brief     				This function sets the adc trig rollover mode for the selected timer
 *
 * @param[in] pwmX    			PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] adc_edge_trigger  Rollover mode - EdgeTrigger_up, EdgeTrigger_down, EdgeTrigger_Both
 *
 * @warning this function must be called BEFORE initialiazing the selected timing unit
 */
void HardwareConfiguration::pwmSetAdcEdgeTrigger(hrtim_tu_number_t pwmX, hrtim_adc_edgetrigger_t adc_edge_trigger)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_adc_rollover_set(pwmX, adc_edge_trigger);
}

/**
 * @brief     				This function returns the adc trigger rollover mode for the selected timer
 *
 * @param[in] pwmX    		PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @return    			    Rollover mode - EdgeTrigger_up, EdgeTrigger_down, EdgeTrigger_Both
 */
hrtim_adc_edgetrigger_t HardwareConfiguration::pwmGetAdcEdgeTrigger(hrtim_tu_number_t pwmX)
{
	return hrtim_adc_rollover_get(pwmX);
}

/**
 * @brief     				This function sets the adc trigger linked to a timer unit
 *
 * @param[in] pwmX	    	PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] adc_trig    	adc trigger - ADCTRIG_1, ADCTRIG_2, ADCTRIG_3 et ADCTRIG_4
 *
 * @warning Call this function BEFORE enabling the adc trigger and AFTER initializing the selected timer
 */
void HardwareConfiguration::pwmSetAdcTrig(hrtim_tu_number_t pwmX, hrtim_adc_trigger_t adc_trig)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_adc_triger_set(pwmX, adc_trig);
}

/**
 * @brief     				This function returns the adc trigger linked to a timer unit
 *
 * @param[in] pwmX	    	PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @return    				adc trigger - ADCTRIG_1, ADCTRIG_2, ADCTRIG_3 et ADCTRIG_4
 */
hrtim_adc_trigger_t HardwareConfiguration::pwmGetAdcTrig(hrtim_tu_number_t pwmX, hrtim_adc_trigger_t adc_trig)
{
	return hrtim_adc_triger_get(pwmX);
}

/**
 * @brief     				This function sets a special pwm mode for voltage or current mode
 *
 * @param[in] pwmX	    	PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] mode    		PWM mode - VOLTAGE_MODE or CURRENT_MODE
 *
 * @warning this function must be called BEFORE initialiazing the selected timing unit
 */
void HardwareConfiguration::pwmSetMode(hrtim_tu_number_t pwmX, hrtim_pwm_mode_t mode)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_pwm_mode_set(pwmX, mode);
}

/**
 * @brief     				This function returns the PWM mode (voltage or current mode)
 *
 * @param[in] pwmX	    	PWM Unit - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @return           		PWM mode - VOLTAGE_MODE or CURRENT_MODE
 *
 * @warning this function must be called before initialiazing a timing unit
 */
hrtim_pwm_mode_t HardwareConfiguration::pwmGetMode(hrtim_tu_number_t pwmX)
{
	return hrtim_pwm_mode_get(pwmX);
}

/**
 * @brief     				This function sets external event linked to the timing unit essential for the current mode
 *
 * @param[in] pwmX	    	PWM Unit 					 - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] eev    		external event trigger		 - EEV1,EEV2, EEV3, EEV3, EEV4, EEV5, EEV6, EEV7, EEV8, EEV9
 *
 * @warning this function must be called before initialiazing a timing unit
 */
void HardwareConfiguration::pwmSetEev(hrtim_tu_number_t pwmX, hrtim_external_trigger_t eev)
{
	if (!hrtim_get_status(pwmX))
		hrtim_init_default_all(); // initialize default parameters before
	hrtim_eev_set(pwmX, eev);
}

/**
 * @brief     				This function sets the external event linked to the timing unit used for the current mode
 *
 * @param[in] pwmX	    	PWM Unit 					 - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @return  		   		external event trigger		 - EEV1,EEV2, EEV3, EEV3, EEV4, EEV5, EEV6, EEV7, EEV8, EEV9
 */
hrtim_external_trigger_t HardwareConfiguration::pwmGetEev(hrtim_tu_number_t pwmX)
{
	return hrtim_eev_get(pwmX);
}

/**
 * @brief     				This function sets the number of event which will be ignored between two events.
 * 							ie. you divide the number of trigger in a fixed period. For example if decimation = 1,
 * 							nothing changes but with decimation = 2 you have twice less adc trigger.
 *
 * @param[in] pwmX	    	PWM Unit 					 - PWMA, PWMB, PWMC, PWMD, PWME or PWMF
 * @param[in] decimation  	decimation/post-scaler		 - a number between 1 and 32
 *
 * @warning this function must be called AFTER initialiazing the selected timing unit
 */
void HardwareConfiguration::pwmSetAdcDecimation(hrtim_tu_number_t pwmX, uint32_t decimation)
{
	if (decimation > 32)
		decimation = 32;
	else if (decimation < 1)
		decimation = 1;
	hrtim_adc_trigger_set_postscaler(pwmX, decimation - 1);
}