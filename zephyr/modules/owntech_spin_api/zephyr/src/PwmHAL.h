/*
 * Copyright (c) 2022-present LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */

#ifndef PWMHAL_H_
#define PWMHAL_H_

/* Stdlib */
#include <stdint.h>

/* ARM lib */
#include <arm_math.h>

/* OwnTech API */
#include "DataAPI.h"
#include "hrtim_enum.h"

/**
 *   Switch leg operation type.
 */
typedef enum
{
     buck,
     boost
} leg_operation_t;

/**
 *   Inverter leg operation type.
 */
typedef enum
{
     unipolar,
     bipolar
} inverter_modulation_t;

/**
 * @brief  Handles all pwm signals for the spin board
 *
 * @note   Use this element to call functions related to the pwm.
 */
class PwmHAL
{
public:
     /* HRTIM configuration */

     /**
      * @brief This function initializes a timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      */
     void initUnit(hrtim_tu_number_t pwmX);

     /**
      * @brief This function starts both outputs of the selected HRTIM channel
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      */
     void startDualOutput(hrtim_tu_number_t pwmX);

     /**
      * @brief This function stops both outputs of the selected HRTIM channel
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      */
     void stopDualOutput(hrtim_tu_number_t pwmX);

     /**
      * @brief This function starts only one output of the selected HRTIM channel
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @param[in] output output to disable: `TIMING_OUTPUT1`,`TIMING_OUTPUT2`
      */
     void startSingleOutput(hrtim_tu_number_t tu, hrtim_output_number_t output);

     /**
      * @brief This function starts only one output of the selected HRTIM channel
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @param[in] output output to disable: `TIMING_OUTPUT1`,`TIMING_OUTPUT2`
      */
     void stopSingleOutput(hrtim_tu_number_t tu, hrtim_output_number_t output);

     /**
      * @brief This function sets the modulation mode for a given PWM unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @param[in] modulation PWM Modulation: `Lft_aligned` or `UpDwn`
      *
      * @warning this function must be called BEFORE initializing the selected timer
      */
     void setModulation(hrtim_tu_number_t pwmX, hrtim_cnt_t modulation);

     /**
      * @brief This function sets the switch convention for a given PWM unit
      * 		i.e. you decide which one of the output of the timer can be controlled
      * 		with duty cycle.
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @param[in] convention PWM Switch to be driven by the duty cycle:  
      * 
      * - `PWMx1`: `BUCK` convention, switch HIGH 
      * 
      * - `PWMx2`: `BOOST` convention, switch LOW
      *
      * @warning this function must be called before the timer initialization
      */
     void setSwitchConvention(hrtim_tu_number_t pwmX,
                              hrtim_switch_convention_t convention);

     /**
      * @brief   This function initialize the PWM for fixed frequency
      * applications
      *
      * @param[in] fixed_frequency frequency to be fixed in Hz
      *
      * @warning This function must be called BEFORE initializing any timing
      *          unit. 
      *          
      *          The frequency will not vary during the operation of the
      *          power device. 
      * 
      *          Use it for fixed frequencies only.
      */
     void initFixedFrequency(uint32_t fixed_frequency);

     /**
      * @brief This functions initializes the PWM for variable frequency
      * applications.
      *
      * @param[in] initial_frequency The initial value of the frequency in Hz
      * @param[in] minimal_frequency The minimal value of the frequency in Hz
      *
      * @warning This function must be called BEFORE initializing any timing
      *          unit. 
      *          
      *          The user can vary the frequency during the operation of
      *          the power device. 
      * 
      *          This may compromise the resolution of the
      *          PWM. 
      * 
      *          You can check your resolution with the getResolutionPS
      *          function.
      */
     void initVariableFrequency(uint32_t initial_frequency,
                                uint32_t minimal_frequency);

     /**
      * @brief This function sets the dead time for the selected timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * 
      * @param[in] rise_ns    rising edge dead time in ns
      * @param[in] falling_ns	falling edge dead time in ns
      *
      * @warning use this function BEFORE initializing the chosen timer
      */
     void setDeadTime(hrtim_tu_number_t pwmX, uint16_t rise_ns, uint16_t fall_ns);

     /**
      * @brief This function sets the duty cycle for the selected timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @param[in] value duty cycle value
      */
     void setDutyCycle(hrtim_tu_number_t pwmX, float32_t duty_cycle);

     /**
      * @brief This function sets the phase shift in respect to timer A
      *        for the selected timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * 
      * @param[in] shift phase shift value in DEGREES (between `-360` and `360`)
      *
      * @warning Use this function AFTER setting the frequency
      *          and initializing the chosen timer
      */
     void setPhaseShift(hrtim_tu_number_t pwmX, int16_t shift);

     /**
      * @brief This function sets a special pwm mode for voltage or current mode
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * 
      * @param[in] mode  PWM mode: `VOLTAGE_MODE` or `CURRENT_MODE`
      *
      * @warning This function must be called BEFORE initializing the selected 
      *          timing unit
      */
     void setMode(hrtim_tu_number_t pwmX, hrtim_pwm_mode_t mode);

     /**
      * @brief This function returns the PWM mode (voltage or current mode)
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * 
      * @return     PWM mode: `VOLTAGE_MODE` or `CURRENT_MODE`
      * @warning This function must be called before initializing a timing unit
      */
     hrtim_pwm_mode_t getMode(hrtim_tu_number_t pwmX);

     /**
      * @brief This function sets external event linked to the timing unit
      *        essential for the current mode
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @param[in] eev   external event trigger:   
      * `EEV1`,`EEV2`,`EEV3`,`EEV4`,`EEV5`,`EEV6`,`EEV7`,`EEV8`,`EEV9`
      * @warning this function must be called before initializing a timing unit
      */
     void setEev(hrtim_tu_number_t pwmX, hrtim_external_trigger_t eev);

     /**
      * @brief This function sets the external event linked to the timing unit
      *        used for the current mode
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @param[in] eev   external event trigger:   
      * `EEV1`,`EEV2`,`EEV3`,`EEV4`,`EEV5`,`EEV6`,`EEV7`,`EEV8`,`EEV9`
      */
     hrtim_external_trigger_t getEev(hrtim_tu_number_t pwmX);

     /**
      * @brief This function returns the modulation type of the selected
      *        timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * 
      * @return The type of modulation `Lft_aligned` or `UpDwn` (center aligned)
      */
     hrtim_cnt_t getModulation(hrtim_tu_number_t pwmX);

     /**
      * @brief This function returns the switching convention of the
      *        selected timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @return     Returns the switch convention : 
      *             
      * - `PWMx1`: `BUCK` convention, switch HIGH 
      * 
      * - `PWMx2`: `BOOST` convention, switch LOW
      */
     hrtim_switch_convention_t getSwitchConvention(hrtim_tu_number_t pwmX);

     /**
      * @brief This function returns the period of the selected timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @return     the period value in number of clock cycles
      */
     uint16_t getPeriod(hrtim_tu_number_t pwmX);

     /**
      * @brief This function returns the maximum period of the selected timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @return     the period value in number of clock cycles
      */
     uint16_t getPeriodMax(hrtim_tu_number_t pwmX);

     /**
      * @brief This function returns the period of the selected timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @return     the period value in number of clock cycles
      */
     uint16_t getPeriodMin(hrtim_tu_number_t pwmX);

     /**
      * @brief This function sets the PostScaler value for the selected timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @param[in] ps_ratio   post scaler ratio
      *
      * @warning    This function must be called after initialiazing a timing unit,
      *             and before enabling the adc trigger
      */
     void setAdcTriggerPostScaler(hrtim_tu_number_t pwmX, uint32_t ps_ratio);

     /**
      * @brief This function sets the adc trigger linked to a timer unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @param[in] adc   ADC number: `ADC_1`,`ADC_2`,`ADC_3`,`ADC_4`,`ADC_5`     
      * @warning Call this function:
      * 
      *          - BEFORE enabling the adc trigger
      *  
      *          - AFTER initializing the selected timer
      */
     void setAdcTrigger(hrtim_tu_number_t pwmX, adc_t adc);

     /**
      * @brief This function returns the adc trigger linked to a timer unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * @return ADC number: `ADC_1`,`ADC_2`,`ADC_3`,`ADC_4`,`ADC_5` or 
      *
      * `UNKNOWN_ADC` if the ADC trigger was not configured
      *                  for this timing unit.
      */
     adc_t getAdcTrigger(hrtim_tu_number_t pwmX);

     /**
      * @brief This function enables the adc trigger for the selected timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      *
      * @warning  Call this function only AFTER setting the adc trigger
      *           and initializing the chosen timer
      */
     void enableAdcTrigger(hrtim_tu_number_t tu_number);

     /**
      * @brief This function disables the adc trigger for the selected timing unit
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      */
     void disableAdcTrigger(hrtim_tu_number_t tu_number);

     /**
      * @brief This function sets the comparator value at which the ADC is trigered
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * 
      * @param[in] trig_val   a float value between 0 and 1
      */
     void setAdcTriggerInstant(hrtim_tu_number_t pwmX, float32_t trig_val);

     /**
      * @brief This function sets the adc trig rollover mode for the selected timer
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * 
      * @param[in] adc_edge_trigger  Rollover mode: 
      *            `EdgeTrigger_up`, `EdgeTrigger_down`, `EdgeTrigger_Both`
      *
      * @warning This function must be called BEFORE initialiazing
      *          the selected timing unit
      */
     void setAdcEdgeTrigger(hrtim_tu_number_t pwmX,
                            hrtim_adc_edgetrigger_t adc_edge_trigger);

     /**
      * @brief This function returns the adc trigger rollover mode for
      *        the selected timer
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * 
      * @return     Rollover mode: 
      *            `EdgeTrigger_up`, `EdgeTrigger_down`, `EdgeTrigger_Both`
      */
     hrtim_adc_edgetrigger_t getAdcEdgeTrigger(hrtim_tu_number_t pwmX);

     /**
      * @brief This function sets the number of event which will be ignored
      *        between two events.ie. you divide the number of trigger
      *        in a fixed period. 
      * 
      *        For example if : 
      *        
      *        - `decimation = 1`, nothing changes but with
      *  
      *        - `decimation = 2`, you have twice less adc trigger
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      * 
      * @param[in] decimation decimation/post-scaler: a number between 1 and 32
      *
      * @warning this function must be called AFTER initialiazing
      *          the selected timing unit
      */
     void setAdcDecimation(hrtim_tu_number_t pwmX, uint32_t decimation);

     /**
      * @brief This function disables the interrupt on repetition counter
      *
      * @param[in] PWM_tu PWM Unit:`TIMA`,`TIMB`,`TIMC`,`TIMD`,`TIME`,`TIMF`
      */
     void disablePeriodEvnt(hrtim_tu_t PWM_tu);

     /**
      * @brief This function sets the repetition counter to ISR period
      *
      * @param[in] PWM_tu PWM Unit:`TIMA`,`TIMB`,`TIMC`,`TIMD`,`TIME`,`TIMF`
      * 
      * @param[in] repetition Number of repetition before the interruption
      *                       on repetition counter event
      */
     void setPeriodEvntRep(hrtim_tu_t PWM_tu, uint32_t repetition);

     /**
      * @brief This function returns the repetition counter value
      *
      * @param[in] PWM_tu PWM Unit:`TIMA`,`TIMB`,`TIMC`,`TIMD`,`TIME`,`TIMF`
      * 
      * @return     repetition counter value
      */
     uint32_t getPeriodEvntRep(hrtim_tu_t PWM_tu);

     /**
      * @brief This function configures the interrupt on repetition counter
      *
      * @param[in] PWM_tu PWM Unit:`TIMA`,`TIMB`,`TIMC`,`TIMD`,`TIME`,`TIMF`
      * 
      * @param[in] repetition 	Number of repetition before the interruption
      *                            on repetition counter event
      * @param[in] callback		Function to call each interupt
      */
     void configurePeriodEvnt(hrtim_tu_t PWM_tu,
                              uint32_t repetition,
                              hrtim_callback_t callback);

     /**
      * @brief This function enables the interrupt on repetition counter
      *
      * @param[in] PWM_tu PWM Unit:`TIMA`,`TIMB`,`TIMC`,`TIMD`,`TIME`,`TIMF`
      */
     void enablePeriodEvnt(hrtim_tu_t PWM_tu);

     /**
      * @brief This function returns the period in µs of the selected timer
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      */
     uint32_t getPeriodUs(hrtim_tu_number_t pwmX);

     /**
      * @brief Change the frequency/period after it has been initialized.
      * @param[in] frequency_update The new frequency in Hz
      * @warning The new frequency can't be inferior to the the one set
      *          in the initialization step.
      * 
      *          Use it AFTER the initialization of the timing unit.
      */
     void setFrequency(uint32_t frequency_update);

     /**
      * @brief     	          This function returns the minimum frequency
      *                       of the selected timer in Hz
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      */
     uint32_t getFrequencyMin(hrtim_tu_number_t pwmX);

     /**
      * @brief     This function returns the maximum frequency
      *            of the selected timer in Hz
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      */
     uint32_t getFrequencyMax(hrtim_tu_number_t pwmX);

     /**
      * @brief  This function returns the resolution of the timing unit in 
      *         picoseconds
      *
      * @param[in] pwmX  PWM Unit: `PWMA`,`PWMB`,`PWMC`,`PWMD`,`PWME`,`PWMF`
      *
      * @note                 The resolution of the PWM depends on the prescaler
      *                       that is automatically calculated when the master
      *                       unit is initialized.
      * 
      *                       For an HRTIM frequency of =170MHz:
      * 
      * - `PRSCL = 0`: `fHRTIM x 32 = 4.608 GHz` - `Res:00184 ps - Min PWM f: 83.00 kHz`
      * 
      * - `PRSCL = 1`: `fHRTIM x 16 = 2.304 GHz` - `Res:00368 ps - Min PWM f: 41.50 kHz`
      * 
      * - `PRSCL = 2`: `fHRTIM x 08 = 1.152 GHz` - `Res:00735 ps - Min PWM f: 20.80 kHz`
      * 
      * - `PRSCL = 3`: `fHRTIM x 04 = 0.576 GHz` - `Res:01470 ps - Min PWM f: 10.40 kHz`
      * 
      * - `PRSCL = 4`: `fHRTIM x 02 = 0.288 GHz` - `Res:02940 ps - Min PWM f: 05.20 kHz`
      * 
      * - `PRSCL = 5`: `fHRTIM x 01 = 0.144 GHz` - `Res:05880 ps - Min PWM f: 02.60 kHz`
      * 
      * - `PRSCL = 6`: `fHRTIM / 02 = 0.072 GHz` - `Res:11760 ps - Min PWM f: 01.30 kHz`
      * 
      * - `PRSCL = 7`: `fHRTIM / 04 = 0.036 GHz` - `Res:23530 ps - Min PWM f: 00.65 kHz`
      *
      */
     uint32_t getResolutionPs(hrtim_tu_number_t pwmX);

     /**
      * @brief   This function initialize burst mode.
      *
      */
     void initBurstMode();

     /**
      * @brief   This function sets burst mode parameters
      *
      * @param[in]  bm_cmp Number of period of PWM off
      * @param[in]  bm_per Total number of PWM period
      */
     void setBurstMode(int bm_cmp, int bm_per);

     /**
      * @brief   This function starts burst mode
      *
      */
     void startBurstMode();

     /**
      * @brief   This function stops burst mode
      *
      */
     void stopBurstMode();

     /**
      * @brief   This function deinit burst mode.
      *
      */
     void deInitBurstMode();
};

#endif /* PWMHAL_H_ */
