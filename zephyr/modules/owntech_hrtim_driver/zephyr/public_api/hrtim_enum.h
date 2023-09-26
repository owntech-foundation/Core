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
 * @date   2023
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef HRTIM_ENUM_H_
#define HRTIM_ENUM_H_

#include <stm32_ll_hrtim.h>
#include <stm32g4xx_ll_gpio.h>
#include <stm32_ll_bus.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *  @brief callback function
     */
    typedef void (*hrtim_callback_t)();

    static const uint8_t HRTIM_CHANNELS = 6;

    /////////////////////////////
    ////// ENUM

    /**
     * @brief   HRTIM timing units definition
     */
    typedef enum
    {
        TIMA = LL_HRTIM_TIMER_A,
        TIMB = LL_HRTIM_TIMER_B,
        TIMC = LL_HRTIM_TIMER_C,
        TIMD = LL_HRTIM_TIMER_D,
        TIME = LL_HRTIM_TIMER_E,
        TIMF = LL_HRTIM_TIMER_F,
        MSTR = LL_HRTIM_TIMER_MASTER
    } hrtim_tu_t;

    /**
     * @brief   HRTIM timing units number definition
     */
    typedef enum
    {
        PWMA = 0,
        PWMB = 1,
        PWMC = 2,
        PWMD = 3,
        PWME = 4,
        PWMF = 5
    } hrtim_tu_number_t;

    /**
     * @brief   HRTIM gpio clock units definition
     */
    typedef enum
    {
        CLK_GPIOA = LL_AHB2_GRP1_PERIPH_GPIOA,
        CLK_GPIOB = LL_AHB2_GRP1_PERIPH_GPIOB,
        CLK_GPIOC = LL_AHB2_GRP1_PERIPH_GPIOC
    } hrtim_gpio_clock_number_t;

    /**
     * @brief   HRTIM reset trig source definitions
     */
    typedef enum
    {
        MSTR_PER = LL_HRTIM_RESETTRIG_MASTER_PER,
        MSTR_CMP1 = LL_HRTIM_RESETTRIG_MASTER_CMP1,
        MSTR_CMP2 = LL_HRTIM_RESETTRIG_MASTER_CMP2,
        MSTR_CMP3 = LL_HRTIM_RESETTRIG_MASTER_CMP3,
        MSTR_CMP4 = LL_HRTIM_RESETTRIG_MASTER_CMP4,
        PWMA_CMP2 = LL_HRTIM_RESETTRIG_OTHER1_CMP2
    } hrtim_reset_trig_t;

    /**
     * @brief Special PWM mode for current mode
     */
    typedef enum
    {
        VOLTAGE_MODE = 0,
        CURRENT_MODE = 1
    } hrtim_pwm_mode_t;

    /**
     * @brief   HRTIM output set units definition
     */
    typedef enum
    {
        SET_NONE = LL_HRTIM_OUTPUTSET_NONE,
        SET_CMP1 = LL_HRTIM_OUTPUTSET_TIMCMP1,
        SET_CMP2 = LL_HRTIM_OUTPUTSET_TIMCMP2,
        SET_CMP3 = LL_HRTIM_OUTPUTSET_TIMCMP3,
        SET_CMP4 = LL_HRTIM_OUTPUTSET_TIMCMP4,
        SET_PER = LL_HRTIM_OUTPUTSET_TIMPER
    } hrtim_output_set_t;

    /**
     * @brief   HRTIM output reset units definition
     */
    typedef enum
    {
        RST_NONE = LL_HRTIM_OUTPUTRESET_NONE,
        RST_CMP1 = LL_HRTIM_OUTPUTRESET_TIMCMP1,
        RST_CMP2 = LL_HRTIM_OUTPUTRESET_TIMCMP2,
        RST_CMP3 = LL_HRTIM_OUTPUTRESET_TIMCMP3,
        RST_CMP4 = LL_HRTIM_OUTPUTRESET_TIMCMP4,
        RST_PER = LL_HRTIM_OUTPUTRESET_TIMPER
    } hrtim_output_reset_t;

    /**
     * @brief   HRTIM output units definition
     */
    typedef enum
    {
        PWMA1 = LL_HRTIM_OUTPUT_TA1,
        PWMA2 = LL_HRTIM_OUTPUT_TA2,
        PWMB1 = LL_HRTIM_OUTPUT_TB1,
        PWMB2 = LL_HRTIM_OUTPUT_TB2,
        PWMC1 = LL_HRTIM_OUTPUT_TC1,
        PWMC2 = LL_HRTIM_OUTPUT_TC2,
        PWMD1 = LL_HRTIM_OUTPUT_TD1,
        PWMD2 = LL_HRTIM_OUTPUT_TD2,
        PWME1 = LL_HRTIM_OUTPUT_TE1,
        PWME2 = LL_HRTIM_OUTPUT_TE2,
        PWMF1 = LL_HRTIM_OUTPUT_TF1,
        PWMF2 = LL_HRTIM_OUTPUT_TF2
    } hrtim_output_units_t;

    typedef enum
    {
        TIMING_OUTPUT1,
        TIMING_OUTPUT2,
    }hrtim_output_number_t;

    /**
     * @brief   HRTIM ADC trigger
     */
    typedef enum
    {
        ADCTRIG_1 = LL_HRTIM_ADCTRIG_1,
        ADCTRIG_2 = LL_HRTIM_ADCTRIG_2,
        ADCTRIG_3 = LL_HRTIM_ADCTRIG_3,
        ADCTRIG_4 = LL_HRTIM_ADCTRIG_4,
        ADCTRIG_NONE,
    } hrtim_adc_trigger_t;

    /**
     * @brief   HRTIM ADC event update
     */
    typedef enum
    {
        PWMA_UPDT = LL_HRTIM_ADCTRIG_UPDATE_TIMER_A,
        PWMB_UPDT = LL_HRTIM_ADCTRIG_UPDATE_TIMER_B,
        PWMC_UPDT = LL_HRTIM_ADCTRIG_UPDATE_TIMER_C,
        PWMD_UPDT = LL_HRTIM_ADCTRIG_UPDATE_TIMER_D,
        PWME_UPDT = LL_HRTIM_ADCTRIG_UPDATE_TIMER_E,
        PWMF_UPDT = LL_HRTIM_ADCTRIG_UPDATE_TIMER_F
    } hrtim_adc_event_t;

    /**
     * @brief   HRTIM ADC Event Number and its associated source
     *          There are a huge number of possibilities, for now this code explores only a few
     * @warning prioritize cmp3, cmp4 and cmp2 might be used for current mode, and cmp1 for duty cycle
     */
    typedef enum
    {
        TIMA_CMP3 = LL_HRTIM_ADCTRIG_SRC13_TIMACMP3,
        TIMB_CMP3 = LL_HRTIM_ADCTRIG_SRC13_TIMBCMP3,
        TIMC_CMP3 = LL_HRTIM_ADCTRIG_SRC13_TIMCCMP3,
        TIMD_CMP3 = LL_HRTIM_ADCTRIG_SRC13_TIMDCMP3,
        TIME_CMP3 = LL_HRTIM_ADCTRIG_SRC13_TIMECMP3,
        TIMF_CMP3 = LL_HRTIM_ADCTRIG_SRC13_TIMFCMP3
    } hrtim_adc_source_t;

    /**
     * @brief   HRTIM comparators definition
     */
    typedef enum
    {
        CMP1xR = 1,
        CMP2xR = 2,
        CMP3xR = 3,
        CMP4xR = 4,
        MCMP1R = 5,
        MCMP2R = 6,
        MCMP3R = 7,
        MCMP4R = 8,
        MCMPER = 10
    } hrtim_cmp_t;

    /**
     * @brief timing unit output 1 or 2
     */
    typedef enum
    {
        OUT1 = 1,
        OUT2 = 2
    } hrtim_out_t;

    /**
     * @brief   HRTIM TU switch convention
     *  PWMx1 : control high-side mosfet
     *  PWMx2 : control low-side mosfet
     */
    typedef enum
    {
        PWMx1 = 0,
        PWMx2 = 1
    } hrtim_switch_convention_t;

    /**
     * comparator usage for a timing unit
     */
    typedef enum
    {
        USED = true,
        FREE = false
    } hrtim_comp_usage_t;

    /**
     * @brief describe if a timing unit has been initialized
     */
    typedef enum
    {
        UNIT_ON = true,
        UNIT_OFF = false
    } hrtim_tu_ON_OFF_t;

    /**
     * @brief   HRTIM ADC trigger registers definition
     */
    typedef enum
    {
        ADC1R = 1,
        ADC2R = 2,
        ADC3R = 3,
        ADC4R = 4
    } hrtim_adc_t;

    typedef enum
    {
        EdgeTrigger_up = LL_HRTIM_ROLLOVER_MODE_PER,
        EdgeTrigger_down = LL_HRTIM_ROLLOVER_MODE_RST,
        EdgeTrigger_Both = LL_HRTIM_ROLLOVER_MODE_BOTH
    } hrtim_adc_edgetrigger_t;

    /**
     * @brief External eventcoming from comparator used for current mode
     */
    typedef enum
    {
        EEV1 = LL_HRTIM_OUTPUTRESET_EEV_1,
        EEV2 = LL_HRTIM_OUTPUTRESET_EEV_2,
        EEV3 = LL_HRTIM_OUTPUTRESET_EEV_3,
        EEV4 = LL_HRTIM_OUTPUTRESET_EEV_4,
        EEV5 = LL_HRTIM_OUTPUTRESET_EEV_5,
        EEV6 = LL_HRTIM_OUTPUTRESET_EEV_6,
        EEV7 = LL_HRTIM_OUTPUTRESET_EEV_7,
        EEV8 = LL_HRTIM_OUTPUTRESET_EEV_8,
        EEV9 = LL_HRTIM_OUTPUTRESET_EEV_9
    } hrtim_external_trigger_t;

    /**
     * @brief  HRTIM counting mode setting
     */
    typedef enum
    {
        Lft_aligned = LL_HRTIM_COUNTING_MODE_UP,
        UpDwn = LL_HRTIM_COUNTING_MODE_UP_DOWN // also known as center aligned

    } hrtim_cnt_t;

    /////////////////////////////
    ////// STRUCT

    /**
     * @brief   Structure containing all the data regarding the pwm of a given timing unit
     */
    typedef struct
    {
        hrtim_tu_t pwm_tu;                         /* Timing Unit associated with the PWM */
        uint16_t rise_dead_time;                   /* Rising Edge Dead time */
        uint16_t fall_dead_time;                   /* Falling Edge Dead time */
        uint16_t duty_cycle;                       /* Current value of its duty cycle */
        uint16_t period;                           /* Period used by the unit */
        uint32_t frequency;                        /* Frequency used by the unit */
        hrtim_cnt_t modulation;                    /* Type of modulation used for this unit */
        hrtim_tu_ON_OFF_t unit_on;                 /* State of the time unit (ON/OFF) */
        uint8_t ckpsc;                             /* Holds the clock pre-scaler of the timing unit */
        hrtim_pwm_mode_t pwm_mode;                 /* pwm mode for voltage mode or current mode */
        hrtim_external_trigger_t external_trigger; /* external trigger event for current mode */
    } pwm_conf_t;

    /**
     * @brief   Structure containing all the data regarding phase shifting for a given timing unit
     */
    typedef struct
    {
        uint16_t value;                /* Value of the phase shift */
        hrtim_tu_t compare_tu;         /* Compare timing unit used to make the phase shift */
        hrtim_reset_trig_t reset_trig; /* Pulse width */
    } phase_shift_conf_t;

    /**
     * @brief   Structure containing all the information of the gpio linked to a given timing unit
     */
    typedef struct
    {
        GPIO_TypeDef *unit;                    /* The GPIO structure to which the TU pins belong */
        LL_GPIO_InitTypeDef switch_H;          /* Details of the high-side switch gpio */
        hrtim_output_units_t OUT_H;            /* High-side switch hrtim output unit */
        LL_GPIO_InitTypeDef switch_L;          /* Details of the low-side switch */
        hrtim_output_units_t OUT_L;            /* Low-side switch hrtim output unit */
        hrtim_gpio_clock_number_t tu_gpio_CLK; /* Gpio clock unit */
    } gpio_conf_t;

    /**
     * @brief   Structure describing the switching convention of a given timing unit
     */
    typedef struct
    {
        hrtim_switch_convention_t convention; /* High-side or Low-side switch convention */
        uint32_t set_H[2];                    /* Set event used to the High-side switch on the high-side convention */
        uint32_t reset_H[2];                  /* Set event used to the Low-side switch on the high-side convention */
        uint32_t set_L[2];                    /* Set event used to the High-side switch on the high-side convention */
        uint32_t reset_L[2];                  /* Set event used to the Low-side switch on the high-side convention */
    } switch_conv_conf_t;

    /**
     * @brief   Structure containing information to setup adc events, adc source links and adc triggers
     */
    typedef struct
    {
        hrtim_adc_event_t adc_event;       /* ADC event linked to this PWM time unit */
        hrtim_adc_source_t adc_source;     /* ADC time unit linked to this event */
        hrtim_adc_trigger_t adc_trigger;   /* ADC trigger between source and event */
        hrtim_adc_edgetrigger_t adc_rollover; /* ADC rollover only relevant in center aligned */
    } adc_hrtim_conf_t;

    /**
     * @brief   Structure containing the status of the usage of comparators and their values
     */
    typedef struct
    {
        hrtim_comp_usage_t cmp1; /* Sets if the COMP 1 of the tu is being used */
        uint16_t cmp1_value;     /* Sets if the COMP 1 of the tu is being used */
        hrtim_comp_usage_t cmp2; /* Sets if the COMP 2 of the tu is being used */
        uint16_t cmp2_value;     /* Sets if the COMP 1 of the tu is being used */
        hrtim_comp_usage_t cmp3; /* Sets if the COMP 3 of the tu is being used */
        uint16_t cmp3_value;     /* Sets if the COMP 1 of the tu is being used */
        hrtim_comp_usage_t cmp4; /* Sets if the COMP 4 of the tu is being used */
        uint16_t cmp4_value;     /* Sets if the COMP 1 of the tu is being used */
    } comp_usage_conf_t;

#ifdef __cplusplus
}
#endif

#endif // COMPARATOR_DRIVER_H_
