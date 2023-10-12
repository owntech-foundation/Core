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
 */

#ifndef HRTIM_H_
#define HRTIM_H_

#include <stdint.h>
#include "arm_math.h"
#include <zephyr/kernel.h>
#include "hrtim_enum.h"

#define TU_DEFAULT_DT (100U)       /* dead-time in ns */
#define TU_DEFAULT_FREQ (200000U)  /* frequency in Hz */
#define TU_DEFAULT_PERIOD (27200U) /* default period for 200kHz in bits */

/**
 * @brief   HRTIM have 5 or 6 timing units
 */
#ifdef HRTIM_MCR_TFCEN
#define HRTIM_STU_NUMOF (6U) /**< number of slave timing units */
#else
#define HRTIM_STU_NUMOF (5U)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Timinig unit configuration - aggregates all the structures.
 */
typedef struct
{
    pwm_conf_t pwm_conf;            /* Pulse Width Modulation Configuration */
    phase_shift_conf_t phase_shift; /* Phase shift configuration */
    gpio_conf_t gpio_conf;          /* Pulse width */
    switch_conv_conf_t switch_conv; /* Switch Convention */
    adc_hrtim_conf_t adc_hrtim;     /* ADC - HRTIM setup*/
    comp_usage_conf_t comp_usage;   /* Usage of comp1 to comp4 */
} timer_hrtim_t;

extern timer_hrtim_t *tu_channel[HRTIM_STU_NUMOF];

/////////////////////////////
////// fUNCTIONS

/**
 * @brief   Gets the APB2 clock
 */
int hrtim_get_apb2_clock();

/**
 * @brief this function initalize all the default parameters for each timing unit structure
 * @warning this function must be called before changing any timing unit parameters (frequency, phase_shift)
 */
void hrtim_init_default_all();

/**
 * @brief   Initializes a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @return    timing unit period
 */
uint16_t hrtim_tu_init(hrtim_tu_number_t tu_number);

/**
 * @brief Returns if the timer was initialized with default value or not
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @return   true or false
 */
hrtim_tu_ON_OFF_t hrtim_get_status(hrtim_tu_number_t tu_number);

/**
 * @brief   Initializes the gpio elements of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 */
void hrtim_tu_gpio_init(hrtim_tu_number_t tu_number);

/**
 * @brief   Disables the output of a given timingg unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 */
void hrtim_out_dis(hrtim_tu_number_t tu_number);

/**
 * @brief   Enables the output of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 */
void hrtim_out_en(hrtim_tu_number_t tu_number);

/**
 * @brief   Enables only one output of a given timing unit
 *
 * @param[in] PWM_OUT        Output pin to be chosen:
 *            @arg @ref PWMA1
 *            @arg @ref PWMA2
 *            @arg @ref PWMB1
 *            @arg @ref PWMB2
 *            @arg @ref PWMC1
 *            @arg @ref PWMC2
 *            @arg @ref PWMD1
 *            @arg @ref PWMD2
 *            @arg @ref PWME1
 *            @arg @ref PWME2
 *            @arg @ref PWMF1
 *            @arg @ref PWMF2
 */
void hrtim_out_en_single(hrtim_output_units_t PWM_OUT);

/**
 * @brief   Disables only one output of a given timing unit
 *
 * @param[in] PWM_OUT        Output pin to be chosen:
 *            @arg @ref PWMA1
 *            @arg @ref PWMA2
 *            @arg @ref PWMB1
 *            @arg @ref PWMB2
 *            @arg @ref PWMC1
 *            @arg @ref PWMC2
 *            @arg @ref PWMD1
 *            @arg @ref PWMD2
 *            @arg @ref PWME1
 *            @arg @ref PWME2
 *            @arg @ref PWMF1
 *            @arg @ref PWMF2
 */
void hrtim_out_dis_single(hrtim_output_units_t PWM_OUT);

/**
 * @brief   Sets the switching convention of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @param[in] modulation        modulation:
 *            @arg @ref Lft_aligned = LL_HRTIM_COUNTING_MODE_UP,
 *            @arg @ref UpDwn = LL_HRTIM_COUNTING_MODE_UP_DOWN
 */
void hrtim_set_modulation(hrtim_tu_number_t tu_number, hrtim_cnt_t modulation);

/**
 * @brief   Gets the switching convention of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @return    modulation:
 *            @arg @ref Lft_aligned,
 *            @arg @ref UpDwn
 */
hrtim_cnt_t hrtim_get_modulation(hrtim_tu_number_t tu_number);

/**
 * @brief   Sets the switching convention of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @param[in] convention        Switching convention:
 *            @arg @ref PWMx1
 *            @arg @ref PWMx2
 */
void hrtim_set_switch_convention(hrtim_tu_number_t tu_number, hrtim_switch_convention_t convention);

/**
 * @brief   Gets the switching convention of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @return    Switching convention of the given time unit
 *            @arg @ref HIGH = 0,
 *            @arg @ref LOW = 1
 */
hrtim_switch_convention_t hrtim_get_switch_convention(hrtim_tu_number_t tu_number);

/**
 * @brief   Activates OUT 1 (switch H) with a given switching convention
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 */
void hrtim_cmpl_pwm_out1(hrtim_tu_number_t tu_number);

/**
 * @brief   Activates OUT 2 (switch L) with a given switching convention
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 */
void hrtim_cmpl_pwm_out2(hrtim_tu_number_t tu_number);

/**
 * @brief   Sets the period of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @param[in] tu_number        Timing unit number:
 */
void hrtim_frequency_set(uint32_t value);

/**
 * @brief   Returns the period of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @return    Period of the timing unit
 */
uint16_t hrtim_period_get(hrtim_tu_number_t tu_number);

/**
 * @brief   Returns the period of a master timer
 *
 * @return    Period of the master timer
 */
uint16_t hrtim_period_Master_get();

/**
 * @brief   Returns the period of a given timing unit in microseconds
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @return    Period of the timing unit in microseconds
 */
uint32_t hrtim_period_get_us(hrtim_tu_number_t tu_number);

/**
 * @brief   Returns the period of the master timer in microseconds
 *
 * @return    Period of the timer master in microseconds
 */
uint32_t hrtim_period_Master_get_us();

/**
 * @brief   Sets one of the four comparators of the HRTIM master timer
 *
 * @param[in] cmp       Master comparators:
 *            @arg @ref CMP1xR = 1,
 *            @arg @ref CMP2xR = 2,
 *            @arg @ref CMP3xR = 3,
 *            @arg @ref CMP4xR = 4
 * @param[in] value        Comparator new value to set:
 */
void hrtim_tu_cmp_set(hrtim_tu_number_t tu_number, hrtim_cmp_t cmp, uint16_t value);

/**
 * @brief   Sets one of the four comparators of the HRTIM master timer
 *
 * @param[in] cmp       Master comparators:
 *            @arg @ref MCMP1R = 1,
 *            @arg @ref MCMP2R = 2
 *            @arg @ref MCMP3R = 3,
 *            @arg @ref MCMP4R = 4
 * @param[in] value        Comparator new value to set:
 */
void hrtim_master_cmp_set(hrtim_cmp_t cmp, uint16_t value);

/**
 * @brief   Sets up a dead time in nano second for given complementary
 *          outputs.
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @param[in] rise_ns        The desired dead time of the rising edge in nano second
 * @param[in] fall_ns        The desired dead time of the falling edge in nano second
 */
void hrtim_dt_set(hrtim_tu_number_t tu_number, uint16_t rise_ns, uint16_t fall_ns);

/**
 * @brief   Updates the duty cycle of a timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @param[in] value        The desired duty cycle value
 */
void hrtim_duty_cycle_set(hrtim_tu_number_t tu_number, uint16_t value);

/**
 * @brief   Shifts the PWM of a timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @param[in] shift        The desired phase shift value
 */
void hrtim_phase_shift_set(hrtim_tu_number_t tu_number, uint16_t shift);

/**
 * @brief   Initialize the dead-time for the PWM
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 */
void hrtim_dt_init(hrtim_tu_number_t tu_number);

/**
 * @brief   Enables a timing unit counter
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 */
void hrtim_cnt_en(hrtim_tu_number_t tu_number);

/**
 * @brief   Disables a timing unit counter
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 */
void hrtim_cnt_dis(hrtim_tu_number_t tu_number);

/**
 * @brief   Enables a timer counter reset event
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @param[in] evt        Reset EVent:
 *            @arg @ref MSTR_PER  = LL_HRTIM_RESETTRIG_MASTER_PER,
 *            @arg @ref MSTR_CMP1 = LL_HRTIM_RESETTRIG_MASTER_CMP1,
 *            @arg @ref MSTR_CMP2 = LL_HRTIM_RESETTRIG_MASTER_CMP2,
 *            @arg @ref MSTR_CMP3 = LL_HRTIM_RESETTRIG_MASTER_CMP3,
 *            @arg @ref MSTR_CMP4 = LL_HRTIM_RESETTRIG_MASTER_CMP4,
 *            @arg @ref PWMA_CMP2 = LL_HRTIM_RESETTRIG_OTHER1_CMP2
 */
void hrtim_rst_evt_en(hrtim_tu_number_t tu_number, hrtim_reset_trig_t evt);

/**
 * @brief   Disables a timer counter reset event
 *
 * @param[in] tu_number        Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 * @param[in] evt        Reset EVent:
 *            @arg @ref MSTR_PER  = LL_HRTIM_RESETTRIG_MASTER_PER,
 *            @arg @ref MSTR_CMP1 = LL_HRTIM_RESETTRIG_MASTER_CMP1,
 *            @arg @ref MSTR_CMP2 = LL_HRTIM_RESETTRIG_MASTER_CMP2,
 *            @arg @ref MSTR_CMP3 = LL_HRTIM_RESETTRIG_MASTER_CMP3,
 *            @arg @ref MSTR_CMP4 = LL_HRTIM_RESETTRIG_MASTER_CMP4,
 *            @arg @ref PWMA_CMP2 = LL_HRTIM_RESETTRIG_OTHER1_CMP2
 */
void hrtim_rst_evt_dis(hrtim_tu_number_t tu_number, hrtim_reset_trig_t evt);

/**
 * @brief   Sets the HRTIM event postsaler. Postscaler ratio indicates
 *          how many potential events will be ignored between two
 *          events which are effectively generated.
 *
 * @param[in] ps_ratio  Post scaler ratio (0 = no post scaler, default)
 */
void hrtim_adc_trigger_set_postscaler(hrtim_tu_number_t tu_number, uint32_t ps_ratio);

/**
 * @brief   Configures and enables an ADC trigger event.
 *
 * @param[in] tu_number Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 */
void hrtim_adc_trigger_en(hrtim_tu_number_t tu_number);

/**
 * @brief   Disbables a ADCx trigger event
 *
 * @param[in] tu_number Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 */
void hrtim_adc_trigger_dis(hrtim_tu_number_t tu_number);

/**
 * @brief   Configures the adc rollover mode
 *
 * @param[in] tu_number Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 *
 * @param[in] adc_rollover rollover mode
 *            @arg @ref EdgeTrigger_up
 *            @arg @ref EdgeTrigger_down
 *            @arg @ref EdgeTrigger_Both
 */
void hrtim_adc_rollover_set(hrtim_tu_number_t tu_number, hrtim_adc_edgetrigger_t adc_rollover);

/**
 * @brief   Returns the adc rollover mode
 *
 * @param[in] tu_number Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 *
 * @return    adc_rollover rollover mode :
 *            @arg @ref EdgeTrigger_up
 *            @arg @ref EdgeTrigger_down
 *            @arg @ref EdgeTrigger_Both
 */
hrtim_adc_edgetrigger_t hrtim_adc_rollover_get(hrtim_tu_number_t tu_number);

/**
 * @brief Configures interrupt on repetition counter for the chosen timing unit
 * @param tu_src timing unit which will be the source for the ISR:
 *         @arg @ref MSTR
 *         @arg @ref TIMA
 *         @arg @ref TIMB
 *         @arg @ref TIMC
 *         @arg @ref TIMD
 *         @arg @ref TIME
 *         @arg @ref TIMF
 * @param repetition value between 1 and 256 for the repetition counter:
 *        period of the event wrt. periods of the HRTIM.
 *        E.g. when set to 10, one event will be triggered every 10 HRTIM period.
 * @param callback Pointer to a void(void) function that will be called
 *        when the event is triggerred.
 */
void hrtim_PeriodicEvent_configure(hrtim_tu_t tu, uint32_t repetition, hrtim_callback_t callback);

/**
 * @brief Enables interrupt on repetition counter for the chosen timing unit.
 *        The periodic event configuration must have been done previously.
 * @param tu_src timing unit which will be the source for the ISR:
 *         @arg @ref MSTR
 *         @arg @ref TIMA
 *         @arg @ref TIMB
 *         @arg @ref TIMC
 *         @arg @ref TIMD
 *         @arg @ref TIME
 *         @arg @ref TIMF
 */
void hrtim_PeriodicEvent_en(hrtim_tu_t tu);

/**
 * @brief Disables interrupt on repetition counter for the chosen timing unit
 * @param tu_src timing unit which will be the source for the ISR:
 *         @arg @ref MSTR
 *         @arg @ref TIMA
 *         @arg @ref TIMB
 *         @arg @ref TIMC
 *         @arg @ref TIMD
 *         @arg @ref TIME
 *         @arg @ref TIMF
 */
void hrtim_PeriodicEvent_dis(hrtim_tu_t tu);

/**
 * @brief Changes the repetition counter value to control the ISR interrupt
 * @param tu_src timing unit which will be the source for the ISR:
 *         @arg @ref MSTR
 *         @arg @ref TIMA
 *         @arg @ref TIMB
 *         @arg @ref TIMC
 *         @arg @ref TIMD
 *         @arg @ref TIME
 *         @arg @ref TIMF
 * @param repetion value between 1 and 256 for the repetition counter:
 * period of the event wrt. periods of the HRTIM.
 * E.g. when set to 10, one event will be triggered every 10 HRTIM period.
 */
void hrtim_PeriodicEvent_SetRep(hrtim_tu_t tu, uint32_t repetition);

/**
 * @brief Gets the current value of the repetition counter.
 * @param tu_src timing unit which will be the source for the ISR
 *         @arg @ref MSTR
 *         @arg @ref TIMA
 *         @arg @ref TIMB
 *         @arg @ref TIMC
 *         @arg @ref TIMD
 *         @arg @ref TIME
 *         @arg @ref TIMF
 * @return Value between 1 and 256 for the repetition counter:
 * period of the event wrt. periods of the HRTIM.
 */
uint32_t hrtim_PeriodicEvent_GetRep(hrtim_tu_t tu);

/**
 * @brief   Initializes dual DAC reset and trigger. The selected timing unit CMP2
 *          will trigger the step (Decrement/Increment of sawtooth) and the reset
 *          (return to initial value).
 *
 * @param[in] tu  timing unit
 */
void DualDAC_init(hrtim_tu_number_t tu);

/**
 * @brief Sets the pwm mode : voltage or current mode
 *
 * @param[in] tu_number Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 *
 * @param[in] mode  pwm mode:
 *            @arg @ref VOLTAGE_MODE
 *            @arg @ref CURRENT_MODE
 */
void hrtim_pwm_mode_set(hrtim_tu_number_t tu_number, hrtim_pwm_mode_t mode);

/**
 * @brief  Returns timing unit pwm mode
 * @return CURRENT_MODE or VOLTAGE_MODE
 */
hrtim_pwm_mode_t hrtim_pwm_mode_get(hrtim_tu_number_t tu_number);

/**
 * @brief Sets the adc trigger number for a timing unit
 *
 * @param[in] tu_number Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 *
 * @param[in] adc_trig  pwm mode:
 *            @arg @ref ADCTRIG_1
 *            @arg @ref ADCTRIG_2
 *            @arg @ref ADCTRIG_3
 *            @arg @ref ADCTRIG_4
 */
void hrtim_adc_triger_set(hrtim_tu_number_t tu_number, hrtim_adc_trigger_t adc_trig);

/**
 * @brief Returns the adc trigger
 * @return
 *            @arg @ref ADCTRIG_1 = ADC trigger 1,
 *            @arg @ref ADCTRIG_2 = ADC trigger 2,
 *            @arg @ref ADCTRIG_3 = ADC trigger 3,
 *            @arg @ref ADCTRIG_4 = ADC trigger 4
 */
hrtim_adc_trigger_t hrtim_adc_triger_get(hrtim_tu_number_t tu_number);

/**
 * @brief Sets the external event used in current mode for a timing unit
 *
 * @param[in] tu_number Timing unit number:
 *            @arg @ref PWMA
 *            @arg @ref PWMB
 *            @arg @ref PWMC
 *            @arg @ref PWMD
 *            @arg @ref PWME
 *            @arg @ref PWMF
 *
 * @param[in] adc_trig  pwm mode:
 *            @arg @ref EEV1
 *            @arg @ref EEV2
 *            @arg @ref EEV3
 *            @arg @ref EEV4
 *            @arg @ref EEV5
 *            @arg @ref EEV6
 *            @arg @ref EEV7
 *            @arg @ref EEV8
 *            @arg @ref EEV9
 */
void hrtim_eev_set(hrtim_tu_number_t tu_number, hrtim_external_trigger_t eev);

/**
 * @brief Returns the external event trigger used in current mode
 * @return
 *            @arg @ref EEV1 = external event 1,
 *            @arg @ref EEV2 = external event 2,
 *            @arg @ref EEV3 = external event 3,
 *            @arg @ref EEV4 = external event 4,
 *            @arg @ref EEV5 = external event 5,
 *            @arg @ref EEV6 = external event 6,
 *            @arg @ref EEV7 = external event 7,
 *            @arg @ref EEV8 = external event 8,
 *            @arg @ref EEV9 = external event 9
 */
hrtim_external_trigger_t hrtim_eev_get(hrtim_tu_number_t tu_number);

#ifdef __cplusplus
}
#endif

#endif // HRTIM_H_
