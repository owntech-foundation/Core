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
 * @brief   HRTIM has 5 or 6 timing units
 */
#ifdef HRTIM_MCR_TFCEN
#define HRTIM_STU_NUMOF (6U) /* number of slave timing units */
#else
#define HRTIM_STU_NUMOF (5U)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Timing unit configuration that aggregates all the structures: 
 *
 *  - `pwm_conf` = Pulse Width Modulation Configuration
 *
 *  - `phase_shift` = Phase shift configuration           
 *
 *  - `gpio_conf` = Configuration of the gpio                         
 *
 *  - `switch_conv` = Switch Convention                   
 *
 *  - `adc_hrtim` = ADC - HRTIM setup                   
 *
 *  - `comp_usage` = Usage of comp1 to comp4             
 */
typedef struct
{
    pwm_conf_t pwm_conf;            
    phase_shift_conf_t phase_shift;            
    gpio_conf_t gpio_conf;          
    switch_conv_conf_t switch_conv; 
    adc_hrtim_conf_t adc_hrtim;     
    comp_usage_conf_t comp_usage;   
} timer_hrtim_t;

extern timer_hrtim_t *tu_channel[HRTIM_STU_NUMOF];

/* Public Functions */

/**
 * @brief Get the current APB2 clock frequency used by HRTIM.
 *
 * This function retrieves the prescaler value configured for the APB2 bus
 * and returns the effective clock frequency driving the HRTIM peripheral.
 *
 * - Reads the APB2 prescaler from RCC configuration.
 *
 * - Calculates the actual APB2 clock by dividing the system clock by the
 *   prescaler value.
 *
 * @return The APB2 clock frequency in Hz.
 *
 */
int hrtim_get_apb2_clock();

/**
 * @brief This function initialize all the default parameters
 *        for each timing unit structure
 * @warning This function must be called before
 *          changing any timing unit parameters (frequency, phase_shift)
 */
void hrtim_init_default_all();

/**
 * @brief   Initializes a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @return Timing unit period in clock pulses
 */
uint16_t hrtim_tu_init(hrtim_tu_number_t tu_number);

/**
 * @brief Returns if the timer was initialized with default value or not
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @return   `true` or `false`
 */
hrtim_tu_ON_OFF_t hrtim_get_status(hrtim_tu_number_t tu_number);

/**
 * @brief   Initializes the gpio elements of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_tu_gpio_init(hrtim_tu_number_t tu_number);

/**
 * @brief   Disables the output of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_out_dis(hrtim_tu_number_t tu_number);

/**
 * @brief   Enables the output of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_out_en(hrtim_tu_number_t tu_number);

/**
 * @brief   Enables only one output of a given timing unit
 *
 * @param[in] PWM_OUT        Output pin to be chosen:
 *                     `PWMA1`,`PWMA2`,`PWMB1`,`PWMB2`,`PWMC1`,`PWMC2`,
 *                     `PWMD1`,`PWMD2`,`PWME1`,`PWME2`,`PWMF1`,`PWMF2`
 */
void hrtim_out_en_single(hrtim_output_units_t PWM_OUT);

/**
 * @brief   Disables only one output of a given timing unit
 *
 * @param[in] PWM_OUT  Output pin to be chosen: 
 *                     `PWMA1`,`PWMA2`,`PWMB1`,`PWMB2`,`PWMC1`,`PWMC2`,
 *                     `PWMD1`,`PWMD2`,`PWME1`,`PWME2`,`PWMF1`,`PWMF2`
 */
void hrtim_out_dis_single(hrtim_output_units_t PWM_OUT);

/**
 * @brief   Sets the switching convention of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param[in] modulation        modulation: `Lft_aligned`, `UpDwn`
 */
void hrtim_set_modulation(hrtim_tu_number_t tu_number, hrtim_cnt_t modulation);

/**
 * @brief   Gets the switching convention of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @return    Modulation type: `Lft_aligned`, `UpDwn`
 */
hrtim_cnt_t hrtim_get_modulation(hrtim_tu_number_t tu_number);

/**
 * @brief   Gets the time resolution for a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @return    resolution in picoseconds
 */
uint32_t hrtim_get_resolution_ps(hrtim_tu_number_t tu_number);

/**
 * @brief   Gets the maximum period of the timing unit in number of clock cycles
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @return    period in number of clock cycles
 */
uint16_t hrtim_get_max_period(hrtim_tu_number_t tu_number);

/**
 * @brief   Gets the minimum period of the timing unit in number of clock cycles
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @return    period in number of clock cycles
 */
uint16_t hrtim_get_min_period(hrtim_tu_number_t tu_number);


/**
 * @brief   Gets the minimum frequency of the timing unit in Hertz
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @return    frequency in Hertz
 */
uint32_t hrtim_get_max_frequency(hrtim_tu_number_t tu_number);

/**
 * @brief   Gets the minimum frequency of the timing unit in Hertz
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @return    frequency in Hertz
 */
uint32_t hrtim_get_min_frequency(hrtim_tu_number_t tu_number);



/**
 * @brief   Sets the switching convention of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param[in] convention        Switching convention: `PWMx1`, `PWMx2`
 */
void hrtim_set_switch_convention(hrtim_tu_number_t tu_number,
                                 hrtim_switch_convention_t convention);

/**
 * @brief   Gets the switching convention of a given timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @return    Switching convention of the given time unit: `PWMx1=0`, `PWMx2=1`
 */
hrtim_switch_convention_t hrtim_get_switch_convention(
    hrtim_tu_number_t tu_number
);

/**
 * @brief   Activates OUT 1 and 2 (switch H and L) with 
 *          a given switching convention
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_cmpl_pwm_out(hrtim_tu_number_t tu_number);


/**
 * @brief   Sets the frequency of a given timing unit in Hz
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param[in] tu_number        Timing unit number:
 */
void hrtim_frequency_set(uint32_t frequency_set, uint32_t frequency_min);

/**
 * @brief   Returns the period of a given timing unit in number of clock cycles
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @return    Period of the timing unit
 */
uint16_t hrtim_period_get(hrtim_tu_number_t tu_number);

/**
 * @brief   Returns the period of a master timer in number of clock cycles
 *
 * @return    Period of the master timer
 */
uint16_t hrtim_period_Master_get();

/**
 * @brief   Returns the period of a given timing unit in microseconds
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
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
 * @param[in] tu_number Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param[in] cmp       Master comparators: `CMP1xR`,`CMP2xR`,`CMP3xR`,`CMP4xR`
 * @param[in] value        Comparator new value to set
 */
void hrtim_tu_cmp_set(hrtim_tu_number_t tu_number,
                      hrtim_cmp_t cmp,
                      uint16_t value);

/**
 * @brief   Sets one of the four comparators of the HRTIM master timer
 *
 * @param[in] cmp       Master comparators: `MCMP1R`,`MCMP2R`,`MCMP3R`,`MCMP4R`
 * @param[in] value        Comparator new value to set:
 */
void hrtim_master_cmp_set(hrtim_cmp_t cmp, uint16_t value);

/**
 * @brief   Sets up a dead time in nano second for given complementary
 *          outputs.
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param[in] rise_ns  The desired dead time of the rising edge in nano second
 * @param[in] fall_ns  The desired dead time of the falling edge in nano second
 */
void hrtim_dt_set(hrtim_tu_number_t tu_number,
                  uint16_t rise_ns,
                  uint16_t fall_ns);

/**
 * @brief   Updates the duty cycle of a timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param[in] value        The desired duty cycle value
 */
void hrtim_duty_cycle_set(hrtim_tu_number_t tu_number, uint16_t value);

/**
 * @brief   Shifts the PWM of a timing unit
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param[in] shift        The desired phase shift value
 */
void hrtim_phase_shift_set(hrtim_tu_number_t tu_number, uint16_t shift);

/**
 * @brief   Initialize the dead-time for the PWM
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_dt_init(hrtim_tu_number_t tu_number);

/**
 * @brief   Initialize burst mode
 *          This mode permits to skip one or multiple PWM periods by
 *          idling the output on a low state. It is used in light load
 *          conditions to minimize switching losses.
 */
void hrtim_burst_mode_init(void);

/**
 * @brief   Set burst mode parameters
 *
 * @param[in] bm_cmp        number of periods idle:
 * @param[in] bm_per        total number of period during a cycle (idle and active)
 */
void hrtim_burst_set(int bm_cmp, int bm_per);

/**
 * @brief   Starts burst mode. Burst mode won't stop until hrtim_burst_stop is
 * called
 */
void hrtim_burst_start(void);

/**
 * @brief   Stops burst mode
 */
void hrtim_burst_stop(void);

/**
 * @brief   Disable burst mode
 */
void hrtim_burst_dis(void);

/**
 * @brief   Enables a timing unit counter
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_cnt_en(hrtim_tu_number_t tu_number);

/**
 * @brief   Disables a timing unit counter
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_cnt_dis(hrtim_tu_number_t tu_number);

/**
 * @brief   Enables a timer counter reset event
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param[in] evt        Reset EVent:
 * - `MSTR_PER`,`MSTR_CMP1`,`MSTR_CMP2`,`MSTR_CMP3`,`MSTR_CMP4`,`PWMA_CMP2` 
 */
void hrtim_rst_evt_en(hrtim_tu_number_t tu_number, hrtim_reset_trig_t evt);

/**
 * @brief   Disables a timer counter reset event
 *
 * @param[in] tu_number        Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param[in] evt        Reset EVent:
 * - `MSTR_PER`,`MSTR_CMP1`,`MSTR_CMP2`,`MSTR_CMP3`,`MSTR_CMP4`,`PWMA_CMP2` 
 */
void hrtim_rst_evt_dis(hrtim_tu_number_t tu_number, hrtim_reset_trig_t evt);

/**
 * @brief   Sets the HRTIM event postsaler. Postscaler ratio indicates
 *          how many potential events will be ignored between two
 *          events which are effectively generated.
 *
 * @param[in] ps_ratio  Post scaler ratio (0 = no post scaler, default)
 */
void hrtim_adc_trigger_set_postscaler(hrtim_tu_number_t tu_number,
                                      uint32_t ps_ratio);

/**
 * @brief   Configures and enables an ADC trigger event.
 *
 * @param[in] tu_number Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_adc_trigger_en(hrtim_tu_number_t tu_number);

/**
 * @brief   Disbables a ADCx trigger event
 *
 * @param[in] tu_number Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_adc_trigger_dis(hrtim_tu_number_t tu_number);

/**
 * @brief   Configures the adc rollover mode
 *
 * @param tu_number Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 *
 * @param adc_rollover rollover mode:
 *            `EdgeTrigger_up`,`EdgeTrigger_down`,`EdgeTrigger_Both`
 */
void hrtim_adc_rollover_set(hrtim_tu_number_t tu_number,
                            hrtim_adc_edgetrigger_t adc_rollover);

/**
 * @brief   Returns the adc rollover mode
 *
 * @param[in] tu_number Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 *
 * @return    adc_rollover rollover mode :
 *            `EdgeTrigger_up`,`EdgeTrigger_down`,`EdgeTrigger_Both`
 */
hrtim_adc_edgetrigger_t hrtim_adc_rollover_get(hrtim_tu_number_t tu_number);

/**
 * @brief Configures interrupt on repetition counter for the chosen timing unit
 * @param tu_src timing unit which will be the source for the ISR:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param repetition value between 1 and 256 for the repetition counter:
 *        period of the event wrt. periods of the HRTIM.
 *        E.g. when set to 10, one event will be triggered every 10 HRTIM period.
 * @param callback Pointer to a void(void) function that will be called
 *        when the event is triggerred.
 */
void hrtim_PeriodicEvent_configure(hrtim_tu_t tu,
                                   uint32_t repetition,
                                   hrtim_callback_t callback);

/**
 * @brief Enables interrupt on repetition counter for the chosen timing unit.
 *        The periodic event configuration must have been done previously.
 * @param tu_src timing unit which will be the source for the ISR:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_PeriodicEvent_en(hrtim_tu_t tu);

/**
 * @brief Disables interrupt on repetition counter for the chosen timing unit
 * @param tu_src timing unit which will be the source for the ISR:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 */
void hrtim_PeriodicEvent_dis(hrtim_tu_t tu);

/**
 * @brief Changes the repetition counter value to control the ISR interrupt
 * @param tu_src timing unit which will be the source for the ISR:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param repetion value between 1 and 256 for the repetition counter:
 * period of the event write periods of the HRTIM.
 * E.g. when set to 10, one event will be triggered every 10 HRTIM period.
 */
void hrtim_PeriodicEvent_SetRep(hrtim_tu_t tu, uint32_t repetition);

/**
 * @brief Gets the current value of the repetition counter.
 * @param tu timing unit which will be the source for the ISR
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * 
 * @return Value between 1 and 256 for the repetition counter:
 * period of the event wrt. periods of the HRTIM.
 */
uint32_t hrtim_PeriodicEvent_GetRep(hrtim_tu_t tu);

/**
 * @brief   Initializes dual DAC reset and trigger. The selected timing unit CMP2
 *          will trigger the step (Decrement/Increment of sawtooth) and the reset
 *          (return to initial value).
 *
 * @param[in] tu  Timing unit number: 
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * 
 */
void DualDAC_init(hrtim_tu_number_t tu);

/**
 * @brief Sets the pwm mode : voltage or current mode
 *
 * @param tu_number Timing unit number: 
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 *
 * @param[in] mode  pwm mode: `VOLTAGE_MODE`, `CURRENT_MODE`
 */
void hrtim_pwm_mode_set(hrtim_tu_number_t tu_number, hrtim_pwm_mode_t mode);

/**
 * @brief  Returns timing unit pwm mode
 * @return `CURRENT_MODE` or `VOLTAGE_MODE`
 */
hrtim_pwm_mode_t hrtim_pwm_mode_get(hrtim_tu_number_t tu_number);

/**
 * @brief Sets the adc trigger number for a timing unit
 *
 * @param[in] tu_number Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 * @param[in] adc_trig  pwm mode:
 *                      `ADCTRIG_1`, `ADCTRIG_2`, `ADCTRIG_3`, `ADCTRIG_4`
 */
void hrtim_adc_trigger_set(hrtim_tu_number_t tu_number,
                           hrtim_adc_trigger_t adc_trig);

/**
 * @brief Returns the adc trigger
 * @return `ADCTRIG_1`, `ADCTRIG_2`, `ADCTRIG_3`, `ADCTRIG_4`
 */
hrtim_adc_trigger_t hrtim_adc_trigger_get(hrtim_tu_number_t tu_number);

/**
 * @brief Sets the external event used in current mode for a timing unit
 *
 * @param[in] tu_number Timing unit number:
 *                  `MSTR`, `TIMA`, `TIMB`, `TIMC`, `TIMD`, `TIME`, `TIMF`
 *
 * @param[in] adc_trig  pwm mode:
 * `EEV1`,`EEV2`,`EEV3`,`EEV4`,`EEV5`,`EEV6`,`EEV7`,`EEV8`,`EEV9`
 */
void hrtim_eev_set(hrtim_tu_number_t tu_number, hrtim_external_trigger_t eev);

/**
 * @brief Returns the external event trigger used in current mode
 * 
 * @return
 * `EEV1`,`EEV2`,`EEV3`,`EEV4`,`EEV5`,`EEV6`,`EEV7`,`EEV8`,`EEV9`
 */
hrtim_external_trigger_t hrtim_eev_get(hrtim_tu_number_t tu_number);

/**
 * @brief Change the frequency/period after it has been initialized.
 * @param[in] new_frequency The new frequency in Hz
 * @warning The new frequency can't be inferior to the the one set
 *          in the initialization step.
 */
void hrtim_change_frequency(uint32_t new_frequency);

#ifdef __cplusplus
}
#endif

#endif /* HRTIM_H_ */
