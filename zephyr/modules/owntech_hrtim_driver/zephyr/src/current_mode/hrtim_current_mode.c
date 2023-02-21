/*
 * Copyright (c) 2020-2023 LAAS-CNRS
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
 * @date 2023
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

// STM32 LL
#include <stm32_ll_rcc.h>
#include <stm32_ll_bus.h>
#include <stm32_ll_hrtim.h>

// header
#include "hrtim_current_mode.h"

static uint8_t _CM_TU_num(hrtim_tu_t tu) /* Return the number associated to the timing unit */
{

    switch (tu)
    {
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

void _CM_init_EEV(void)
{
    // Initialization of external event 4 linked to COMP1	output
    LL_HRTIM_EE_SetSrc(HRTIM1, LL_HRTIM_EVENT_4, LL_HRTIM_EEV4SRC_COMP1_OUT);
    LL_HRTIM_EE_SetPolarity(HRTIM1, LL_HRTIM_EVENT_4, LL_HRTIM_EE_POLARITY_HIGH);
    LL_HRTIM_EE_SetSensitivity(HRTIM1, LL_HRTIM_EVENT_4, LL_HRTIM_EE_SENSITIVITY_LEVEL);
    LL_HRTIM_EE_SetFastMode(HRTIM1, LL_HRTIM_EVENT_4, LL_HRTIM_EE_FASTMODE_DISABLE);

    // Initialization of external event 5 linked to COMP3 output
    LL_HRTIM_EE_SetSrc(HRTIM1, LL_HRTIM_EVENT_5, LL_HRTIM_EEV5SRC_COMP3_OUT);
    LL_HRTIM_EE_SetPolarity(HRTIM1, LL_HRTIM_EVENT_5, LL_HRTIM_EE_POLARITY_HIGH);
    LL_HRTIM_EE_SetSensitivity(HRTIM1, LL_HRTIM_EVENT_5, LL_HRTIM_EE_SENSITIVITY_LEVEL);
    LL_HRTIM_EE_SetFastMode(HRTIM1, LL_HRTIM_EVENT_5, LL_HRTIM_EE_FASTMODE_DISABLE);
}

uint16_t CM_hrtim_init(uint32_t *freq, uint16_t dead_time_ns, uint8_t leg1_upper_switch_convention, uint8_t leg2_upper_switch_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu)
{
    /* Master timer initialization */
    uint16_t period = hrtim_init_master(0, freq);

    /* External event initialization */
    _CM_init_EEV();

    /* Dual DAC trigger initialization */
    CM_DualDAC_init(leg1_tu);
    CM_DualDAC_init(leg2_tu);

    /* Timer initialization for leg 1 */
    hrtim_init_tu(0, leg1_tu, freq, Lft_aligned);
    hrtim_pwm_dt(0, leg1_tu, dead_time_ns, dead_time_ns);        // Set the dead time. Note: this must be done before enable counter
    hrtim_cnt_en(0, leg1_tu);                                    // Enable counter
    hrtim_rst_evt_en(0, leg1_tu, LL_HRTIM_RESETTRIG_MASTER_PER); // We synchronize the leg1 with the master timer, with a reset on period event

    /* Timer initialization for leg 2 */
    hrtim_init_tu(0, leg2_tu, freq, Lft_aligned);
    hrtim_pwm_dt(0, leg2_tu, dead_time_ns, dead_time_ns);        // Set the dead time. Note: this must be done before enable counter
    hrtim_cnt_en(0, leg2_tu);                                    // Enable the counter
    hrtim_rst_evt_en(0, leg2_tu, LL_HRTIM_RESETTRIG_MASTER_PER); // We synchronize the leg2 with the master timer, with a reset on period event

    CM_hrtim_pwm_leg1(leg1_tu, leg1_upper_switch_convention, Lft_aligned); // Set the convention for leg 1
    CM_hrtim_pwm_leg2(leg2_tu, leg2_upper_switch_convention, Lft_aligned); // Set the convention for leg 2

    return period;
}

void CM_hrtim_pwm_set(hrtim_tu_t tu, uint16_t shift)
{
    static uint16_t prev_shift[HRTIM_STU_NUMOF];

    uint8_t tu_n = _CM_TU_num(tu); // timing unit number
    uint32_t duty_cycle_max;       // Duty_cycle max set to 0.9
    uint32_t Sawtooth_step;        // Fix the number of step for the sawtooth

    /* Set the duty_cycle max with comparator 1  */
    duty_cycle_max = LL_HRTIM_TIM_GetPeriod(HRTIM1, tu) * 0.9;
    LL_HRTIM_TIM_SetCompare1(HRTIM1, tu, duty_cycle_max);

    /* Set the number of step with comparator 2 */
    Sawtooth_step = LL_HRTIM_TIM_GetPeriod(HRTIM1, tu) / 100; // divided by 100 to have hundred steps
    LL_HRTIM_TIM_SetCompare2(HRTIM1, tu, Sawtooth_step);

    /* Compare 4 is used to set the PWM */
    LL_HRTIM_TIM_SetCompare4(HRTIM1, tu, 1088);

    if (shift != prev_shift[tu_n])
    {
        prev_shift[tu_n] = shift;

        /* Set reset comparator for phase positioning */
        if (shift)
        {
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu, LL_HRTIM_TIM_GetResetTrig(HRTIM1, tu) & ~LL_HRTIM_RESETTRIG_MASTER_PER);
            switch (tu)
            {
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
        else if (LL_HRTIM_TIM_GetPeriod(HRTIM1, LL_HRTIM_TIMER_MASTER) == LL_HRTIM_TIM_GetPeriod(HRTIM1, tu) && LL_HRTIM_TIM_GetPrescaler(HRTIM1, LL_HRTIM_TIMER_MASTER) == LL_HRTIM_TIM_GetPrescaler(HRTIM1, tu))
        {
            /* shift == 0 and timing unit run at the same frequency as master */
            switch (tu)
            {
            case TIMB:
                LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_B, LL_HRTIM_TIM_GetResetTrig(HRTIM1, LL_HRTIM_TIMER_B) & ~LL_HRTIM_RESETTRIG_MASTER_CMP1);
                break;
            case TIMC:
                LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_C, LL_HRTIM_TIM_GetResetTrig(HRTIM1, LL_HRTIM_TIMER_C) & ~LL_HRTIM_RESETTRIG_MASTER_CMP2);
                break;
            case TIMD:
                LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_D, LL_HRTIM_TIM_GetResetTrig(HRTIM1, LL_HRTIM_TIMER_D) & ~LL_HRTIM_RESETTRIG_MASTER_CMP3);
                break;
            case TIME:
                LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_E, LL_HRTIM_TIM_GetResetTrig(HRTIM1, LL_HRTIM_TIMER_E) & ~LL_HRTIM_RESETTRIG_MASTER_CMP4);
                break;
            #if (HRTIM_STU_NUMOF == 6)
            case TIMF:
                LL_HRTIM_TIM_SetResetTrig(HRTIM1, LL_HRTIM_TIMER_F, LL_HRTIM_TIM_GetResetTrig(HRTIM1, LL_HRTIM_TIMER_F) & ~LL_HRTIM_RESETTRIG_OTHER1_CMP2);
                break;
            #endif
            default:
                break;
            }
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu, LL_HRTIM_RESETTRIG_MASTER_PER);
        }
        else
        {
            /* timing unit do not run at the same frequency as master so
             * phase positioning is not applicable */
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu, LL_HRTIM_TIM_GetResetTrig(HRTIM1, tu) & ~LL_HRTIM_RESETTRIG_MASTER_PER);
        }
    }
}

void CM_hrtim_pwm_leg1(hrtim_tu_t tu, bool leg_upper_switch_convention, hrtim_cnt_t cnt_mode)
{
    // Configuration for the upper switch convention in buck mode
    // For a selected timing unit, leg1 will set on CMP4 and reset on CMP1 (max duty_cyle) or EEV4 (COMP1 output)
    if (leg_upper_switch_convention == true)
    {
        switch (tu)
        {
        case TIMA:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        case TIMB:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        case TIMC:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        case TIMD:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        case TIME:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        case TIMF:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        default:
            break;
        }
    }

    // Configuration for the upper switch convention in boost mode
    // For a selected timing unit, leg1 will reset on CMP4 and set on CMP1 (max duty_cyle) or EEV4 (COMP1 output)
    else if (leg_upper_switch_convention == false)
    {
        switch (tu)
        {
        case TIMA:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        case TIMB:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        case TIMC:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        case TIMD:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        case TIME:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        case TIMF:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_4);
            break;

        default:
            break;
        }
    }
}

void CM_hrtim_pwm_leg2(hrtim_tu_t tu, bool leg_upper_switch_convention, hrtim_cnt_t cnt_mode)
{
    // Configuration for the upper switch convention in buck mode
    // For a selected timing unit, leg1 will set on CMP4 and reset on CMP1 (max duty_cyle) or EEV5 (COMP3 output)
    if (leg_upper_switch_convention == true)
    {
        switch (tu)
        {
        case TIMA:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        case TIMB:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        case TIMC:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        case TIMD:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        case TIME:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        case TIMF:
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        default:
            break;
        }
    }

    // Configuration for the upper switch convention in boost mode
    // For a selected timing unit, leg1 will reset on CMP4 and set on CMP1 (max duty_cyle) or EEV5 (COMP3 output)
    else if (leg_upper_switch_convention == false)
    {
        switch (tu)
        {
        case TIMA:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TA1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        case TIMB:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TB1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        case TIMC:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TC1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        case TIMD:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TD1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        case TIME:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TE1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        case TIMF:
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTSET_TIMCMP4);
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, LL_HRTIM_OUTPUT_TF1, LL_HRTIM_OUTPUTRESET_TIMCMP1 | LL_HRTIM_OUTPUTRESET_EEV_5);
            break;

        default:
            break;
        }
    }
}

void CM_DualDAC_init(hrtim_tu_t tu)
{
    switch (tu)
    {
    case TIMA:
        LL_HRTIM_TIM_SetDualDacResetTrigger(HRTIM1, LL_HRTIM_TIMER_A, LL_HRTIM_DCDR_COUNTER);
        LL_HRTIM_TIM_SetDualDacStepTrigger(HRTIM1, LL_HRTIM_TIMER_A, LL_HRTIM_DCDS_CMP2);
        LL_HRTIM_TIM_EnableDualDacTrigger(HRTIM1, LL_HRTIM_TIMER_A);
        break;

    case TIMB:
        LL_HRTIM_TIM_SetDualDacResetTrigger(HRTIM1, LL_HRTIM_TIMER_B, LL_HRTIM_DCDR_COUNTER);
        LL_HRTIM_TIM_SetDualDacStepTrigger(HRTIM1, LL_HRTIM_TIMER_B, LL_HRTIM_DCDS_CMP2);
        LL_HRTIM_TIM_EnableDualDacTrigger(HRTIM1, LL_HRTIM_TIMER_B);
        break;

    case TIMC:
        LL_HRTIM_TIM_SetDualDacResetTrigger(HRTIM1, LL_HRTIM_TIMER_C, LL_HRTIM_DCDR_COUNTER);
        LL_HRTIM_TIM_SetDualDacStepTrigger(HRTIM1, LL_HRTIM_TIMER_C, LL_HRTIM_DCDS_CMP2);
        LL_HRTIM_TIM_EnableDualDacTrigger(HRTIM1, LL_HRTIM_TIMER_C);
        break;

    case TIMD:
        LL_HRTIM_TIM_SetDualDacResetTrigger(HRTIM1, LL_HRTIM_TIMER_D, LL_HRTIM_DCDR_COUNTER);
        LL_HRTIM_TIM_SetDualDacStepTrigger(HRTIM1, LL_HRTIM_TIMER_D, LL_HRTIM_DCDS_CMP2);
        LL_HRTIM_TIM_EnableDualDacTrigger(HRTIM1, LL_HRTIM_TIMER_D);
        break;

    case TIME:
        LL_HRTIM_TIM_SetDualDacResetTrigger(HRTIM1, LL_HRTIM_TIMER_E, LL_HRTIM_DCDR_COUNTER);
        LL_HRTIM_TIM_SetDualDacStepTrigger(HRTIM1, LL_HRTIM_TIMER_E, LL_HRTIM_DCDS_CMP2);
        LL_HRTIM_TIM_EnableDualDacTrigger(HRTIM1, LL_HRTIM_TIMER_E);
        break;

    case TIMF:
        LL_HRTIM_TIM_SetDualDacResetTrigger(HRTIM1, LL_HRTIM_TIMER_F, LL_HRTIM_DCDR_COUNTER);
        LL_HRTIM_TIM_SetDualDacStepTrigger(HRTIM1, LL_HRTIM_TIMER_F, LL_HRTIM_DCDS_CMP2);
        LL_HRTIM_TIM_EnableDualDacTrigger(HRTIM1, LL_HRTIM_TIMER_F);
        break;

    default:
        break;
    }
}
