/*
 * Copyright (c) 2020-2022 LAAS-CNRS
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
 * @date        2022
 *
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_hrtim

 * @file
 * @brief       Low-level HRTIM driver implementation
 *
 * @author      Hugues Larrive <hugues.larrive@laas.fr>
 * @author      Clément Foucher <clement.foucher@laas.fr>
 * @author      Antoine Boche <antoine.boche@laas.fr>
 * @author      Luiz Villa <luiz.villa@laas.fr>
 * @author      Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#include <stm32_ll_rcc.h>
#include <stm32_ll_bus.h>
#include "stm32g4xx_ll_gpio.h"

#include "hrtim_voltage_mode.h"
#include "assert.h"


//static const struct soc_gpio_pinctrl tim_pinctrl[] = ST_STM32_DT_INST_PINCTRL(0, 0);

static uint8_t _TU_num1(hrtim_tu_t tu) /* Return the number associated to the timing unit */
{
  
    switch(tu){
        case TIMA:
            return 0;
        
        case TIMB:
             return 1;
        
        case TIMC: 
             return 2;
        
        case TIMD:
             return 3;            
        
        case TIME:
             return 4; 
     #if (HRTIM_STU_NUMOF == 6)   
        case TIMF: 
             return 5;
     #endif   
        default:
            return 100;

    }
}

static inline void _clk_init(hrtim_t hrtim)
{
    #if defined(CPU_FAM_STM32F3) || defined(CONFIG_SOC_SERIES_STM32F3X)
    /* 1. selection of the high-speed PLL output */
    RCC->CFGR3 |= hrtim_config[hrtim].rcc_sw_mask;
    #endif
    /* 2. clock enable for the registers mapped on the APB2 bus */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_HRTIM1);

    __DSB(); // from cmsis_armcc.h
    /* Data Synchronization Barrier
     * Acts as a special kind of Data Memory Barrier.
     * It completes when all explicit memory accesses before this
     * instruction complete. */


    /* HRTIM DLL initialization */
    /* DLL calibration: periodic calibration enabled, period set to 14μs */
    LL_HRTIM_ConfigDLLCalibration(HRTIM1, LL_HRTIM_DLLCALIBRATION_MODE_CONTINUOUS, LL_HRTIM_DLLCALIBRATION_RATE_3);

    /* Check DLL end of calibration flag */
    while(LL_HRTIM_IsActiveFlag_DLLRDY(HRTIM1) == RESET)
	{
		// Wait
	}
}

static inline unsigned _msb(unsigned v)
{   
    /* Return the most significant bit */ 

    return 8 * sizeof(v) - __builtin_clz(v) - 1;
}

static inline uint32_t _period_ckpsc(hrtim_t hrtim, uint32_t freq,
                                        uint16_t *per, uint8_t *ckpsc)
{
    #if defined(CONFIG_SOC_SERIES_STM32F3X)
        uint32_t f_hrtim = hrtim_get_apb2_clock() * 2;
    #elif defined(CONFIG_SOC_SERIES_STM32G4X)
         uint32_t f_hrtim = hrtim_get_apb2_clock();
    #else
         #warning "unsupported stm32XX family"
    #endif

    /* t_hrck = f_hrck / freq but f_hrck = (f_hrtim * 32) which is too
     * big for an uint32 so we will firstly divide f_hrtim then also add
     * the modulo to preserve accuracy. */
    uint32_t period = (f_hrtim / freq) * 32 + (f_hrtim % freq) * 32 / freq;

    /* period = t_hrck / 2^ckpsc so bits over 15 position directly give
     * the needed prescaller */
    *ckpsc = (_msb(period) > 15) ? _msb(period) - 15 : 0;
    period /= (1 << *ckpsc);

    /* From the f334 reference manual :
     * The period and compare values must be within a lower and an upper
     * limit related to the high-resolution implementation and listed in
     * Table 82:
     * • The minimum value must be greater than or equal to 3 periods of
     * the f HRTIM clock
     * • The maximum value must be less than or equal to 0xFFFF - 1
     * periods of the f HRTIM clock */
    uint16_t min_period = *ckpsc < 5 ? (96 >> *ckpsc) : 0x3;
    uint16_t max_period = *ckpsc < 4 ? (0xffff - (32 >> *ckpsc)) : 0xfffd;

    /* Adjust prescaler and period if needed */
    if (period > (uint32_t)max_period) {
        *ckpsc = *ckpsc + 1;
        period /= 2;
    }
    /* Verify parameters */
    assert( (*ckpsc <= 7)
            && (hrtim < HRTIM_NUMOF) 
            && (period >= min_period)
            && (period <= max_period));

    /* Note: with period = max_period (48MHz frequency for f344) it
     * theorically not possible to obtain PWM because this limitation
     * also concern compare value, but it is probably possible to
     * workaround this limitation and manage a duty cycle using the
     * dead-time generator which have a 868ps resolution... */

    *per = (uint16_t)period;

    /* Compute and return the effective frequency */
    uint32_t frequency = (  (f_hrtim / period) * 32
                            + (f_hrtim % period) * 32 / period
                         )  / (1 << *ckpsc);
    return frequency;
}

uint16_t hrtim_init(hrtim_t hrtim, uint32_t *freq, uint16_t dead_time_ns, uint8_t leg1_upper_switch_convention, uint8_t leg2_upper_switch_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
    /* Master timer initialization */
    uint16_t period = hrtim_init_master(hrtim, freq);

    /* Timer initialization for leg 1 */
    hrtim_init_tu(hrtim, leg1_tu, freq, Lft_aligned);
    hrtim_pwm_dt(hrtim, leg1_tu, dead_time_ns, dead_time_ns); // Set the dead time. Note: this must be done before enable counter
    hrtim_cnt_en(hrtim, leg1_tu); // Enable counter 
    hrtim_rst_evt_en(hrtim, leg1_tu, LL_HRTIM_RESETTRIG_MASTER_PER); // We synchronize the Timer A with the master timer, with a reset on period event

    /* Timer initialization for leg 2 */
    hrtim_init_tu(hrtim, leg2_tu, freq, Lft_aligned);
    hrtim_pwm_dt(hrtim, leg2_tu, dead_time_ns, dead_time_ns);  // Set the dead time. Note: this must be done before enable counter
    hrtim_cnt_en(hrtim, leg2_tu);  // Enable the counter
    hrtim_rst_evt_en(hrtim, leg2_tu, LL_HRTIM_RESETTRIG_MASTER_PER); // We synchronize the Timer B with the master timer, with a reset on period event


    hrtim_cmpl_pwm_out1(hrtim, leg1_tu, leg1_upper_switch_convention, Lft_aligned); // Set the convention for leg 1
    hrtim_cmpl_pwm_out1(hrtim, leg2_tu, leg2_upper_switch_convention, Lft_aligned); // Set the convention for leg 2

    return period;
}

void hrtim_update_dead_time(hrtim_t hrtim, hrtim_tu_t tu, uint16_t rise_ns, uint16_t fall_ns)
{
    hrtim_cnt_dis(hrtim, tu);    //Disable the timing unit counter
    hrtim_pwm_dt(hrtim, tu, rise_ns, fall_ns); // Set the dead time. Note: this must be done before enable counter
    hrtim_cnt_en(hrtim, tu);     // Enable counter 

}

uint16_t hrtim_init_updwn(hrtim_t hrtim, uint32_t *freq, uint16_t dt, uint8_t leg1_upper_switch_convention, uint8_t leg2_upper_switch_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
    /* Master timer and timing unit frequency initialization */
    uint16_t period = hrtim_init_master(hrtim, freq);
    uint32_t freq_tu = (*freq)*2;

    /* Timer initialization for leg 1 */
    hrtim_init_tu(hrtim, leg1_tu, &freq_tu, UpDwn);
    hrtim_pwm_dt(hrtim, leg1_tu, dt, dt); // Set the dead time. Note: this must be done before enable counter
    hrtim_cnt_en(hrtim, leg1_tu); // Enable counter 
    hrtim_rst_evt_en(hrtim, leg1_tu, LL_HRTIM_RESETTRIG_MASTER_PER); // We synchronize the Timer A with the master timer, with a reset on period event

    /* Timer initialization for leg 2 */
    hrtim_init_tu(hrtim, leg2_tu, &freq_tu, UpDwn);
    hrtim_pwm_dt(hrtim, leg2_tu, dt, dt);  // Set the dead time. Note: this must be done before enable counter
    hrtim_cnt_en(hrtim, leg2_tu);  // Enable the counter
    hrtim_rst_evt_en(hrtim, leg2_tu, LL_HRTIM_RESETTRIG_MASTER_PER); // We synchronize the Timer B with the master timer, with a reset on period event


    hrtim_cmpl_pwm_out1(hrtim, leg1_tu, leg1_upper_switch_convention, UpDwn); // Set the convention for leg 1
    hrtim_cmpl_pwm_out1(hrtim, leg2_tu, leg2_upper_switch_convention, UpDwn); // Set the convention for leg 2

    return period/2; // return timing unit period which is half the period of the master timer
}

void hrtim_pwm_set(hrtim_t hrtim, hrtim_tu_t tu, uint16_t value, uint16_t shift)
{
    static uint16_t prev_value[HRTIM_STU_NUMOF];
    static uint16_t prev_shift[HRTIM_STU_NUMOF];

    uint8_t tu_n = _TU_num1(tu); // timing unit number
    
     if (value != prev_value[tu_n]) {
        prev_value[tu_n] = value;

        /* Disable outputs when duty cycle is 0 */
        if (value == 0) {
            hrtim_out_dis(hrtim, tu, OUT1);
            hrtim_out_dis(hrtim, tu, OUT2);
            return;
        }
        /* Set comparator for duty cycle */
        LL_HRTIM_TIM_SetCompare1(HRTIM1, tu, value);

        /* Enable outputs */
        hrtim_out_en(hrtim, tu, OUT1);
        hrtim_out_en(hrtim, tu, OUT2);
    }

    if (shift != prev_shift[tu_n]) {
        prev_shift[tu_n] = shift;

        /* Set reset comparator for phase positioning */
        if (shift) {
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu, LL_HRTIM_TIM_GetResetTrig (HRTIM1, tu) &~ LL_HRTIM_RESETTRIG_MASTER_PER);
            switch (tu) {
                /* Timer A is the phase shift reference so it can't be phase
                 * shifted */
                case TIMB: /* Timer B reset on master cmp1 */
                    LL_HRTIM_TIM_SetCompare1(HRTIM1, LL_HRTIM_TIMER_MASTER, shift);
                    LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_B, LL_HRTIM_RESETTRIG_MASTER_CMP1);
                    break;
                case TIMC: /* Timer C reset on master cmp2 */
                    LL_HRTIM_TIM_SetCompare2(HRTIM1, LL_HRTIM_TIMER_MASTER, shift);
                    LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_C, LL_HRTIM_RESETTRIG_MASTER_CMP2);
                    break;
                case TIMD: /* Timer D reset on master cmp3 */
                    LL_HRTIM_TIM_SetCompare3(HRTIM1, LL_HRTIM_TIMER_MASTER, shift);
                    LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_D, LL_HRTIM_RESETTRIG_MASTER_CMP3);
                    break;
                case TIME: /* Timer E reset on master cmp4 */
                    LL_HRTIM_TIM_SetCompare4(HRTIM1, LL_HRTIM_TIMER_MASTER, shift);
                    LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_E, LL_HRTIM_RESETTRIG_MASTER_CMP4);
                    break;
            #if (HRTIM_STU_NUMOF == 6)
                case TIMF: /* Timer F reset on timerA cmp2 */
                    LL_HRTIM_TIM_SetCompare2(HRTIM1, LL_HRTIM_TIMER_A, shift); 
                    LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu, LL_HRTIM_RESETTRIG_OTHER1_CMP2);
                    break;
            #endif
                default:
                    break;
            }
        }
        else if(LL_HRTIM_TIM_GetPeriod(HRTIM1,LL_HRTIM_TIMER_MASTER) == LL_HRTIM_TIM_GetPeriod(HRTIM1, tu)
        && LL_HRTIM_TIM_GetPrescaler (HRTIM1, LL_HRTIM_TIMER_MASTER) == LL_HRTIM_TIM_GetPrescaler(HRTIM1, tu)){
            /* shift == 0 and timing unit run at the same frequency as master */
            switch (tu) {
                case TIMB:
                    LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_B, LL_HRTIM_TIM_GetResetTrig (HRTIM1, LL_HRTIM_TIMER_B) &~ LL_HRTIM_RESETTRIG_MASTER_CMP1);
                    break;
                case TIMC:
                    LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_C, LL_HRTIM_TIM_GetResetTrig (HRTIM1, LL_HRTIM_TIMER_C) &~ LL_HRTIM_RESETTRIG_MASTER_CMP2);
                    break;
                case TIMD:
                    LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_D, LL_HRTIM_TIM_GetResetTrig (HRTIM1, LL_HRTIM_TIMER_D) &~ LL_HRTIM_RESETTRIG_MASTER_CMP3);
                    break;
                case TIME:
                    LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_E, LL_HRTIM_TIM_GetResetTrig (HRTIM1, LL_HRTIM_TIMER_E) &~ LL_HRTIM_RESETTRIG_MASTER_CMP4);
                    break;
            #if (HRTIM_STU_NUMOF == 6)
                case TIMF:
                    LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_F, LL_HRTIM_TIM_GetResetTrig (HRTIM1, LL_HRTIM_TIMER_F) &~ LL_HRTIM_RESETTRIG_OTHER1_CMP2);
                    break;
            #endif
                default:
                    break;
            }
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu, LL_HRTIM_RESETTRIG_MASTER_PER);
        }
        else {
            /* timing unit do not run at the same frequency as master so
             * phase positioning is not applicable */
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu, LL_HRTIM_TIM_GetResetTrig (HRTIM1, tu) &~ LL_HRTIM_RESETTRIG_MASTER_PER);
         }
   }

}

uint16_t hrtim_init_master(hrtim_t hrtim, uint32_t *freq)
{
    uint16_t period;
    uint8_t ckpsc;

    /* HRTIM clock initialization */
    _clk_init(hrtim);

    /* At start-up, it is mandatory to initialize first the prescaler
     * bitfields before writing the compare and period registers. */
    *freq = _period_ckpsc(hrtim, *freq, &period, &ckpsc);
    /* master timer prescaler init */
    LL_HRTIM_TIM_SetPrescaler(HRTIM1, LL_HRTIM_TIMER_MASTER, ckpsc);

    /* Master timer initialization */
    /* continuous mode, preload enabled on repetition event */

    LL_HRTIM_TIM_SetCounterMode(HRTIM1,LL_HRTIM_TIMER_MASTER, LL_HRTIM_MODE_CONTINUOUS);                                
    LL_HRTIM_TIM_EnablePreload(HRTIM1, LL_HRTIM_TIMER_MASTER);
    LL_HRTIM_TIM_SetUpdateTrig(HRTIM1, LL_HRTIM_TIMER_MASTER, LL_HRTIM_UPDATETRIG_REPETITION);

    /* Enable counter */
    LL_HRTIM_TIM_CounterEnable(HRTIM1, LL_HRTIM_TIMER_MASTER);

    /* Configure the PWM frequency by setting the period registers */
    LL_HRTIM_TIM_SetPeriod(HRTIM1, LL_HRTIM_TIMER_MASTER, period);

    return period;
}

uint16_t hrtim_init_tu(hrtim_t hrtim, hrtim_tu_t tu, uint32_t *freq, hrtim_cnt_t cnt_mode)
{
    uint16_t period;
    uint8_t ckpsc;
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Outputs initialization */

    hrtim_out_dis(hrtim, tu, OUT1);
    hrtim_out_dis(hrtim, tu, OUT2);
    
    //stm32_dt_pinctrl_configure(tim_pinctrl, ARRAY_SIZE(tim_pinctrl), HRTIM1_BASE);
    if(tu==TIMA){
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
            /**HRTIM1 GPIO Configuration
            PA8     ------> HRTIM1_CHA1
            PA9     ------> HRTIM1_CHA2
            */
        GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_13;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_13;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    }else if (tu == TIMB){
        
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
        /**HRTIM1 GPIO Configuration
        PA10     ------> HRTIM1_CHB1
        PA11     ------> HRTIM1_CHB2
        */
        GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_13;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_13;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    } else {
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
        /* USER CODE END HRTIM1_Init 2 */
            /**HRTIM1 GPIO Configuration
            PB12     ------> HRTIM1_CHC1
            PB13     ------> HRTIM1_CHC2
            */
        GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_13;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_13;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }


    
    /* At start-up, it is mandatory to initialize first the prescaler
     * bitfields before writing the compare and period registers. */
   
    *freq = _period_ckpsc(hrtim, *freq, &period, &ckpsc);
        LL_HRTIM_TIM_SetPrescaler(HRTIM1, tu, ckpsc);   

    /* timer initialization */
    /* continuous mode, preload enabled on repetition event */
    
    LL_HRTIM_TIM_SetCounterMode(HRTIM1, tu, LL_HRTIM_MODE_CONTINUOUS);                                
    LL_HRTIM_TIM_EnablePreload(HRTIM1, tu);
    LL_HRTIM_TIM_SetUpdateTrig(HRTIM1, tu, LL_HRTIM_UPDATETRIG_REPETITION);
    
    /* Set the counting mode, left aligned or up-down (center aligned) */ 

    if(cnt_mode == UpDwn){LL_HRTIM_TIM_SetCountingMode(HRTIM1, tu, LL_HRTIM_COUNTING_MODE_UP_DOWN);}

    /* Configure the PWM frequency by setting the period registers */
    LL_HRTIM_TIM_SetPeriod(HRTIM1, tu, period);
    return period;
}

void hrtim_set_cb_set(hrtim_t hrtim, uint32_t out,
                        uint32_t cb)
{
    LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, out, cb);  
}

void hrtim_set_cb_unset(hrtim_t hrtim, uint32_t out,
                        uint32_t cb)
{
    LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, out,  LL_HRTIM_OUT_GetOutputSetSrc(HRTIM1, out) & ~cb ); 
}

void hrtim_rst_cb_set(hrtim_t hrtim, uint32_t out,
                        uint32_t cb)
{
    LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, out, cb);
}

void hrtim_rst_cb_unset(hrtim_t hrtim, uint32_t out,
                            uint32_t cb)
{
    LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, out,  LL_HRTIM_OUT_GetOutputResetSrc(HRTIM1,out) & ~cb ); 
}


void hrtim_cmpl_pwm_out1(hrtim_t hrtim, hrtim_tu_t tu, bool leg_upper_switch_convention, hrtim_cnt_t cnt_mode)
{
    // Configuration for the upper switch convention in buck mode
    if (leg_upper_switch_convention == true)
    { switch(tu){

        case TIMA: 
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTSET_TIMPER);}
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        case TIMB: 
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTSET_TIMPER);}
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        case TIMC:
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTSET_TIMPER);} 
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        case TIMD: 
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTSET_TIMPER);} 
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        case TIME:
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTSET_TIMPER);}  
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        case TIMF: 
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTSET_TIMPER);} 
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        default:
        break;

    }
    }

    // Configuration for the upper switch convention in boost mode
    else if (leg_upper_switch_convention == false)
    {
        switch(tu){
        
        case TIMA: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break; 

        case TIMB: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break; 

        case TIMC: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break; 

         case TIMD: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break; 

        case TIME: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break; 

        case TIMF: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break; 

        default:
        break; 

        }
    }
}


void hrtim_cmpl_pwm_out2(hrtim_t hrtim, hrtim_tu_t tu, bool leg_lower_switch_convention, hrtim_cnt_t cnt_mode)
{
    // Configuration for the lower switch convention in buck mode
    if (leg_lower_switch_convention == true)
    {   
        switch(tu){

        case TIMA: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA2, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA2, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break; 

        case TIMB: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB2, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB2, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break;

        case TIMC: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC2, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC2, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break;

        case TIMD: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD2, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD2, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break;

        case TIME: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE2, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE2, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break;

        case TIMF: 
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF2, LL_HRTIM_OUTPUTSET_TIMCMP1);
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF2, LL_HRTIM_OUTPUTRESET_TIMPER);} 
        break;

        default: 
        break;

        }
    }

    // Configuration for the lower switch convention in boost mode
    else if (leg_lower_switch_convention == false)
    {   
        switch(tu){

        case TIMA: 
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1,  LL_HRTIM_OUTPUT_TA2, LL_HRTIM_OUTPUTSET_TIMPER);} 
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA2, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break; 

        case TIMB: 
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB2, LL_HRTIM_OUTPUTSET_TIMPER);} 
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB2, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        case TIMC: 
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC2, LL_HRTIM_OUTPUTSET_TIMPER);} 
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC2, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        case TIMD: 
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD2, LL_HRTIM_OUTPUTSET_TIMPER);} 
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD2, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        case TIME: 
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE2, LL_HRTIM_OUTPUTSET_TIMPER);} 
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE2, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        case TIMF: 
        if(cnt_mode == Lft_aligned){LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF2, LL_HRTIM_OUTPUTSET_TIMPER);} 
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF2, LL_HRTIM_OUTPUTRESET_TIMCMP1);
        break;

        default: 
        break;
        
        }
    }
}


void hrtim_period_set(hrtim_t hrtim, hrtim_tu_t tu, uint16_t value)
{
    LL_HRTIM_TIM_SetPeriod(HRTIM1, tu, value);
}

void hrtim_cmp_set(hrtim_t hrtim, hrtim_tu_t tu, hrtim_cmp_t cmp,
                    uint16_t value)
{
    if (tu == MSTR) {
        switch (cmp) {
            case MCMP1R:
                LL_HRTIM_TIM_SetCompare1(HRTIM1,LL_HRTIM_TIMER_MASTER,value);
                break;
            case MCMP2R:
                LL_HRTIM_TIM_SetCompare2(HRTIM1,LL_HRTIM_TIMER_MASTER,value);
                break;
            case MCMP3R:
                LL_HRTIM_TIM_SetCompare3(HRTIM1,LL_HRTIM_TIMER_MASTER,value);
                break;
            case MCMP4R:
                LL_HRTIM_TIM_SetCompare4(HRTIM1, LL_HRTIM_TIMER_MASTER, value);
                break;
            default:
                break;
        }
    }
    else {
        switch (cmp) {
            case CMP1xR:
                LL_HRTIM_TIM_SetCompare1(HRTIM1, tu, value);
                break;
            case CMP2xR:
                LL_HRTIM_TIM_SetCompare2(HRTIM1, tu, value);
                break;
            case CMP3xR:
                LL_HRTIM_TIM_SetCompare3(HRTIM1, tu, value);
                break;
            case CMP4xR:
                LL_HRTIM_TIM_SetCompare4(HRTIM1, tu, value);
                break;
            default:
                break;
        }
    }
}

void hrtim_cnt_en(hrtim_t hrtim, hrtim_tu_t tu)
{
    LL_HRTIM_TIM_CounterEnable(HRTIM1, tu);
}

void hrtim_cnt_dis(hrtim_t hrtim, hrtim_tu_t tu)
{
    LL_HRTIM_TIM_CounterDisable(HRTIM1, tu);
}

void hrtim_rst_evt_en(hrtim_t hrtim, hrtim_tu_t tu, uint32_t evt)
{
    LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu, evt);
}

void hrtim_rst_evt_dis(hrtim_t hrtim, hrtim_tu_t tu, uint32_t evt)
{
    LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu, LL_HRTIM_TIM_GetResetTrig(HRTIM1, tu) &~ evt); 
}

void hrtim_out_en(hrtim_t hrtim, hrtim_tu_t tu, hrtim_out_t out)
{   
    if(out == OUT1){

    switch(tu){ 
    
    case TIMA:
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TA1);
    break; 

    case TIMB: 
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TB1);
    break; 
    
    case TIMC: 
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TC1);
    break;

    case TIMD: 
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TD1);
    break;

    case TIME: 
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TE1);
    break;

    case TIMF: 
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TF1);
    break;

    default:
    break; 

            } 
    
    }else{

        switch(tu){ 
    
    case TIMA:
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TA2);
    break; 

    case TIMB: 
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TB2);
    break; 
    
    case TIMC: 
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TC2);
    break;

    case TIMD: 
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TD2);
    break;

    case TIME: 
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TE2);
    break;

    case TIMF: 
    LL_HRTIM_EnableOutput(HRTIM1, LL_HRTIM_OUTPUT_TF2);
    break;

    default:
    break; 


             }
   
    }

}


void hrtim_out_dis(hrtim_t hrtim, hrtim_tu_t tu, hrtim_out_t out)
{
    if(out == OUT1){

    switch(tu){ 
    
    case TIMA:
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TA1);
    break; 

    case TIMB: 
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TB1);
    break; 
    
    case TIMC: 
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TC1);
    break;

    case TIMD: 
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TD1);
    break;

    case TIME: 
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TE1);
    break;

    case TIMF: 
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TF1);
    break;

    default:
    break; 

            } 
    
    }else{

        switch(tu){ 
    
    case TIMA:
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TA2);
    break; 

    case TIMB: 
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TB2);
    break; 
    
    case TIMC: 
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TC2);
    break;

    case TIMD: 
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TD2);
    break;

    case TIME: 
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TE2);
    break;

    case TIMF: 
    LL_HRTIM_DisableOutput(HRTIM1, LL_HRTIM_OUTPUT_TF2);
    break;

    default:
    break; 


             }
   
    }
}


/* Note : The dead time is configured centered by default,
 * there are no options available to modify this, so the dead time
 * must be taken into account when calculating the PWM duty cycle */
void hrtim_pwm_dt(hrtim_t hrtim, hrtim_tu_t tu, uint16_t rise_ns, uint16_t fall_ns)
{
    uint32_t rise_ps = rise_ns * 1000;
    uint32_t fall_ps = fall_ns * 1000;
    /* t_dtg = (2^dtpsc) * (t_hrtim / 8)
     *       = (2^dtpsc) / (f_hrtim * 8) */
    #if defined(CONFIG_SOC_SERIES_STM32F3X)
         uint32_t f_hrtim = hrtim_get_apb2_clock() * 2;
    #elif defined(CONFIG_SOC_SERIES_STM32G4X)
         uint32_t f_hrtim = hrtim_get_apb2_clock();
    #else
        #warning "unsupported stm32XX family"
    #endif

    uint8_t dtpsc = 0; // Deadtime clock prescaler set at xx
    uint32_t t_dtg_ps = (1 << dtpsc) * 1000000 / ((f_hrtim * 8) / 1000000); // intermediate gain for dead time calculation
    uint16_t rise_dt = rise_ps / t_dtg_ps; // calculate the register value based on desired deadtime in picoseconds
    
    while (rise_dt > 511 && dtpsc < 7) {
        dtpsc++;
        t_dtg_ps = (1 << dtpsc) * 1000000 / ((f_hrtim * 8) / 1000000);
        rise_dt = rise_ps / t_dtg_ps;
    }
    if (rise_dt > 511) {
        rise_dt = 511;
    }

    uint16_t fall_dt = fall_ps / t_dtg_ps; // calculate the register value based on desired deadtime in picoseconds
    
    while (fall_dt > 511 && dtpsc < 7) {
        dtpsc++;
        t_dtg_ps = (1 << dtpsc) * 1000000 / ((f_hrtim * 8) / 1000000);
        fall_dt = fall_ps / t_dtg_ps;
    }
    if (fall_dt > 511) {
        fall_dt = 511;
    }


    LL_HRTIM_DT_SetPrescaler(HRTIM1, tu, dtpsc ); // Deadtime clock prescaler

    LL_HRTIM_DT_SetFallingValue(HRTIM1, tu, fall_dt); // Deadtime falling edge value

    LL_HRTIM_DT_SetRisingValue(HRTIM1, tu, rise_dt); // Deadtime rising edge value

    // LL_HRTIM_DT_SetFallingSign(hrtim, tu, LL_HRTIM_DT_FALLING_NEGATIVE);  // Change the behavior of the deadtime insertion by overlapping the signals (negative falling edge)

    // LL_HRTIM_DT_SetRisingSign(hrtim, tu, LL_HRTIM_DT_RISING_POSITIVE); // change the behavior of the deadtime insertion by overlapping the signals (positive falling edge)

    LL_HRTIM_TIM_EnableDeadTime(HRTIM1, tu); /* Note: This
    * parameter cannot be changed once the timer is operating (TxEN bit
    * set) or if its outputs are enabled and set/reset by another timer. */
}

void hrtim_adc_trigger_set_postscaler(hrtim_t hrtim, uint32_t ps_ratio)
{
    LL_HRTIM_SetADCPostScaler(HRTIM1, LL_HRTIM_ADCTRIG_1,ps_ratio);
}

void hrtim_adc_trigger_en(uint32_t event_number, uint32_t source_timer, uint32_t event)
{
	uint32_t adcTrig = LL_HRTIM_ADCTRIG_1;
	uint32_t update = LL_HRTIM_ADCTRIG_UPDATE_MASTER;

	switch (event_number)
	{
		case 1:
			adcTrig = LL_HRTIM_ADCTRIG_1;
			break;
		case 2:
			adcTrig = LL_HRTIM_ADCTRIG_2;
			break;
		case 3:
			adcTrig = LL_HRTIM_ADCTRIG_3;
			break;
		case 4:
			adcTrig = LL_HRTIM_ADCTRIG_4;
			break;
	}

	switch (source_timer)
	{
		case 1:
			update = LL_HRTIM_ADCTRIG_UPDATE_TIMER_A;
			break;
		case 2:
			update = LL_HRTIM_ADCTRIG_UPDATE_TIMER_B;
			break;
		case 3:
			update = LL_HRTIM_ADCTRIG_UPDATE_TIMER_C;
			break;
		case 4:
			update = LL_HRTIM_ADCTRIG_UPDATE_TIMER_D;
			break;
	}

	LL_HRTIM_SetADCTrigSrc(HRTIM1, adcTrig, event);
	LL_HRTIM_SetADCTrigUpdate(HRTIM1, adcTrig, update);
}

void hrtim_adc_trigger_dis(hrtim_t hrtim, hrtim_adc_t adc, uint32_t evt)
{
    switch (adc) {
        
        case ADC1R: LL_HRTIM_SetADCTrigSrc(HRTIM1, LL_HRTIM_ADCTRIG_1,LL_HRTIM_GetADCTrigSrc(HRTIM1,LL_HRTIM_ADCTRIG_1)&~ evt);
        case ADC2R: LL_HRTIM_SetADCTrigSrc(HRTIM1, LL_HRTIM_ADCTRIG_2, LL_HRTIM_GetADCTrigSrc(HRTIM1,LL_HRTIM_ADCTRIG_2)&~ evt);
        case ADC3R: LL_HRTIM_SetADCTrigSrc(HRTIM1, LL_HRTIM_ADCTRIG_3,LL_HRTIM_GetADCTrigSrc(HRTIM1,LL_HRTIM_ADCTRIG_3)&~ evt);
        case ADC4R: LL_HRTIM_SetADCTrigSrc(HRTIM1, LL_HRTIM_ADCTRIG_4, LL_HRTIM_GetADCTrigSrc(HRTIM1,LL_HRTIM_ADCTRIG_4)&~ evt);
    }
}


int hrtim_get_apb2_clock()
{
    int prescaler = 1;

    switch(LL_RCC_GetAPB2Prescaler())
    {
        case RCC_CFGR_PPRE2_DIV1:
            prescaler=1;
            break;
        case RCC_CFGR_PPRE2_DIV2:
            prescaler=2;
            break;
        case RCC_CFGR_PPRE2_DIV4:
            prescaler=4;
            break;
        case RCC_CFGR_PPRE2_DIV8:
            prescaler=8;
            break;
        case RCC_CFGR_PPRE2_DIV16:
            prescaler=16;
            break;
    }

    return CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC / prescaler;
}

