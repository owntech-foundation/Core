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
 * @file
 * @brief   PWM management layer by inverter leg
 * @date    2023
 * @author  Hugues Larrive <hugues.larrive@laas.fr>
 * @author  Antoine Boche <antoine.boche@laas.fr>
 * @author  Luiz Villa <luiz.villa@laas.fr>
 * @author  Clément Foucher <clement.foucher@laas.fr>
 * @author  Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

/* include */
#include <stm32_ll_rcc.h>
#include "assert.h"
#include "hrtim.h"

/* variables for ISR */
static const uint8_t HRTIM_IRQ_NUMBER = 67;
static const uint8_t HRTIM_IRQ_PRIO = 0;
static const uint8_t HRTIM_IRQ_FLAGS = 0;
static float32_t HRTIM_CLK_RESOLUTION = 184e-6;

/* User callback for ISR */
static hrtim_callback_t user_callback = NULL;

/* Default values to initialize all the timer */
static hrtim_tu_t list_tu[HRTIM_STU_NUMOF] = {TIMA, TIMB, TIMC, TIMD, TIME, TIMF};                                                               // listing all timing unit
static hrtim_tu_t phase_shift_compare_units[HRTIM_STU_NUMOF] = {MSTR, MSTR, MSTR, MSTR, MSTR, TIMA};                                             // All timing units phase shift are referenced to the master, with the exception of TIMF
static hrtim_reset_trig_t phase_shift_reset_trig[HRTIM_STU_NUMOF] = {MSTR_PER, PWMA_CMP2, MSTR_CMP2, MSTR_CMP3, MSTR_CMP4, MSTR_CMP1};           // phase shift trigger source
static hrtim_output_units_t tu_output_high[HRTIM_STU_NUMOF] = {PWMA1, PWMB1, PWMC1, PWMD1, PWME1, PWMF1};                                        // PWM output high side
static hrtim_output_units_t tu_output_low[HRTIM_STU_NUMOF] = {PWMA2, PWMB2, PWMC2, PWMD2, PWME2, PWMF2};                                         // PWM output low side
static GPIO_TypeDef *unit[HRTIM_STU_NUMOF] = {GPIOA, GPIOA, GPIOB, GPIOB, GPIOC, GPIOC};                                                         // The GPIO structure to which the TU pins belong
static hrtim_gpio_clock_number_t clk_gpio[HRTIM_STU_NUMOF] = {CLK_GPIOA, CLK_GPIOA, CLK_GPIOB, CLK_GPIOB, CLK_GPIOC, CLK_GPIOC};                 // gpio clock
static uint32_t switch_H_pin[HRTIM_STU_NUMOF] = {LL_GPIO_PIN_8, LL_GPIO_PIN_10, LL_GPIO_PIN_12, LL_GPIO_PIN_14, LL_GPIO_PIN_8, LL_GPIO_PIN_6};   // gpio pin high side
static uint32_t switch_L_pin[HRTIM_STU_NUMOF] = {LL_GPIO_PIN_9, LL_GPIO_PIN_11, LL_GPIO_PIN_13, LL_GPIO_PIN_15, LL_GPIO_PIN_9, LL_GPIO_PIN_7};   // gpio pin low side
static uint32_t alternate_function[HRTIM_STU_NUMOF] = {LL_GPIO_AF_13, LL_GPIO_AF_13, LL_GPIO_AF_13, LL_GPIO_AF_13, LL_GPIO_AF_3, LL_GPIO_AF_13}; // alternate functions                                                                                           /*low convention*/
static hrtim_adc_trigger_t tu_adc_trigger[HRTIM_STU_NUMOF] = {ADCTRIG_3, ADCTRIG_2, ADCTRIG_1, ADCTRIG_2, ADCTRIG_2, ADCTRIG_2};                 // adc trigger used
static hrtim_adc_event_t tu_adc_events[HRTIM_STU_NUMOF] = {PWMA_UPDT, PWMB_UPDT, PWMC_UPDT, PWMD_UPDT, PWME_UPDT, PWMF_UPDT};                    // ADC trigger update source
static hrtim_adc_source_t tu_adc_source[HRTIM_STU_NUMOF] = {TIMA_CMP3, TIMB_CMP3, TIMC_CMP3, TIMD_CMP3, TIME_CMP3, TIMF_CMP3};                   // ADC trigger source
static hrtim_external_trigger_t tu_external_trig[HRTIM_STU_NUMOF] = {EEV4, EEV1, EEV5, EEV1, EEV1, EEV1};

/**
 *  PWMx1 : setting high-side switch convention
 *  PWMx2 : setting low-side switch conventioon
 */
static hrtim_switch_convention_t conv_PWMx1 = PWMx1;
static hrtim_switch_convention_t conv_PWMx2 = PWMx2;

/* master timer structure initialization */
timer_hrtim_t timerMaster = {
    .pwm_conf.frequency = TU_DEFAULT_FREQ,
    .pwm_conf.unit_on = UNIT_OFF,

    .comp_usage.cmp1 = FREE,
    .comp_usage.cmp1_value = 0,
    .comp_usage.cmp2 = FREE,
    .comp_usage.cmp2_value = 0,
    .comp_usage.cmp3 = FREE,
    .comp_usage.cmp3_value = 0,
    .comp_usage.cmp4 = FREE,
    .comp_usage.cmp4_value = 0,

    .phase_shift.compare_tu = MSTR,
    .phase_shift.reset_trig = MSTR_PER,
};

timer_hrtim_t timerA;
timer_hrtim_t timerB;
timer_hrtim_t timerC;
timer_hrtim_t timerD;
timer_hrtim_t timerE;
timer_hrtim_t timerF;

timer_hrtim_t *tu_channel[HRTIM_STU_NUMOF] = {&timerA, &timerB, &timerC, &timerD, &timerE, &timerF};

/////////////////////////////
////// Private functions

static inline void _clk_init()
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
    while (LL_HRTIM_IsActiveFlag_DLLRDY(HRTIM1) == RESET)
    {
        // Wait
    }
}

static inline unsigned _msb(unsigned v)
{
    /* Return the most significant bit */

    return 8 * sizeof(v) - __builtin_clz(v) - 1;
}

static inline uint32_t _period_ckpsc(uint32_t freq, timer_hrtim_t *tu)
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
    tu->pwm_conf.ckpsc = (_msb(period) > 15) ? _msb(period) - 15 : 0;
    period /= (1 << tu->pwm_conf.ckpsc);

    /* From the f334 reference manual :
     * The period and compare values must be within a lower and an upper
     * limit related to the high-resolution implementation and listed in
     * Table 82:
     * • The minimum value must be greater than or equal to 3 periods of
     * the f HRTIM clock
     * • The maximum value must be less than or equal to 0xFFFF - 1
     * periods of the f HRTIM clock */
    uint16_t min_period = tu->pwm_conf.ckpsc < 5 ? (96 >> tu->pwm_conf.ckpsc) : 0x3;
    uint16_t max_period = tu->pwm_conf.ckpsc < 4 ? (0xffff - (32 >> tu->pwm_conf.ckpsc)) : 0xfffd;

    /* Adjust prescaler and period if needed */
    if (period > (uint32_t)max_period)
    {
        tu->pwm_conf.ckpsc = tu->pwm_conf.ckpsc + 1;
        period /= 2;
    }
    /* Verify parameters */
    assert((tu->pwm_conf.ckpsc <= 7) && (period >= min_period) && (period <= max_period));

    /* Note: with period = max_period (48MHz frequency for f344) it
     * theorically not possible to obtain PWM because this limitation
     * also concern compare value, but it is probably possible to
     * workaround this limitation and manage a duty cycle using the
     * dead-time generator which have a 868ps resolution... */

    tu->pwm_conf.period = (uint16_t)period;

    /* Compute and return the effective frequency */
    uint32_t frequency = ((f_hrtim / period) * 32 + (f_hrtim % period) * 32 / period) / (1 << tu->pwm_conf.ckpsc);
    return frequency;
}

/* callback for interruption on repetition counter */
void _hrtim_callback()
{
    if (LL_HRTIM_GetSyncInSrc(HRTIM1) == LL_HRTIM_SYNCIN_SRC_NONE)
        LL_HRTIM_ClearFlag_REP(HRTIM1, LL_HRTIM_TIMER_MASTER);

    if (LL_HRTIM_GetSyncInSrc(HRTIM1) == LL_HRTIM_SYNCIN_SRC_EXTERNAL_EVENT)
        LL_HRTIM_ClearFlag_SYNC(HRTIM1);

    if (LL_HRTIM_GetSyncOutConfig(HRTIM1) == LL_HRTIM_SYNCOUT_POSITIVE_PULSE)
    {
        /* In case of master communication mode, the master will send a synchronization
        Pulse every control period allowing the slave to synchronize its control task execution
        to master control task execution */
        LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_ALTERNATE);

        k_busy_wait(1);

        LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
    }

    if (user_callback != NULL)
    {
        user_callback();
    }
}

void _CM_init_EEV(void)
{
    // Initialization of external event 4 linked to COMP1 output
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

/* hrtim master initialization */
void _init_master()
{
    /* HRTIM clock initialization */
    _clk_init();

    /* At start-up, it is mandatory to initialize first the prescaler
     * bitfields before writing the compare and period registers. */
    timerMaster.pwm_conf.frequency = _period_ckpsc(timerMaster.pwm_conf.frequency, &timerMaster);

    /* master timer prescaler init */
    LL_HRTIM_TIM_SetPrescaler(HRTIM1, LL_HRTIM_TIMER_MASTER, timerMaster.pwm_conf.ckpsc);

    /* Master timer initialization */
    /* continuous mode, preload enabled on repetition event */
    LL_HRTIM_TIM_SetCounterMode(HRTIM1, LL_HRTIM_TIMER_MASTER, LL_HRTIM_MODE_CONTINUOUS);
    LL_HRTIM_TIM_EnablePreload(HRTIM1, LL_HRTIM_TIMER_MASTER);
    LL_HRTIM_TIM_SetUpdateTrig(HRTIM1, LL_HRTIM_TIMER_MASTER, LL_HRTIM_UPDATETRIG_REPETITION);

    /* Configure the PWM frequency by setting the period registers */
    LL_HRTIM_TIM_SetPeriod(HRTIM1, LL_HRTIM_TIMER_MASTER, timerMaster.pwm_conf.period);

    /* Enable counter */
    LL_HRTIM_TIM_CounterEnable(HRTIM1, LL_HRTIM_TIMER_MASTER);

    timerMaster.pwm_conf.unit_on = UNIT_ON;
}

/////////////////////////////
////// Public functions

/* return bus clock prescalerl */
int hrtim_get_apb2_clock()
{
    int prescaler = 1;

    switch (LL_RCC_GetAPB2Prescaler())
    {
    case RCC_CFGR_PPRE2_DIV1:
        prescaler = 1;
        break;
    case RCC_CFGR_PPRE2_DIV2:
        prescaler = 2;
        break;
    case RCC_CFGR_PPRE2_DIV4:
        prescaler = 4;
        break;
    case RCC_CFGR_PPRE2_DIV8:
        prescaler = 8;
        break;
    case RCC_CFGR_PPRE2_DIV16:
        prescaler = 16;
        break;
    }

    return CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC / prescaler;
}

void hrtim_init_default_all()
{
    for (uint8_t tu_count = 0; tu_count < HRTIM_CHANNELS; tu_count++)
    {
        if (tu_channel[tu_count]->pwm_conf.unit_on == false)
        {
            // sets up the pwm variables of the timing units
            tu_channel[tu_count]->pwm_conf.pwm_tu = list_tu[tu_count];
            tu_channel[tu_count]->pwm_conf.duty_cycle = 0;
            tu_channel[tu_count]->pwm_conf.frequency = timerMaster.pwm_conf.frequency;
            tu_channel[tu_count]->pwm_conf.fall_dead_time = TU_DEFAULT_DT;
            tu_channel[tu_count]->pwm_conf.period = TU_DEFAULT_PERIOD;
            tu_channel[tu_count]->pwm_conf.rise_dead_time = TU_DEFAULT_DT;
            tu_channel[tu_count]->pwm_conf.ckpsc = 0;
            tu_channel[tu_count]->pwm_conf.pwm_mode = VOLTAGE_MODE;
            tu_channel[tu_count]->pwm_conf.external_trigger = tu_external_trig[tu_count];

            // sets up the phase shift variables of the timing units
            tu_channel[tu_count]->phase_shift.value = 0;                                        // Phase shift configuration
            tu_channel[tu_count]->phase_shift.compare_tu = phase_shift_compare_units[tu_count]; // Phase shift configuration
            tu_channel[tu_count]->phase_shift.reset_trig = phase_shift_reset_trig[tu_count];    // Phase shift configuration

            // sets up the gpio variables of the timing units
            tu_channel[tu_count]->gpio_conf.OUT_H = tu_output_high[tu_count];
            tu_channel[tu_count]->gpio_conf.switch_H.Pin = switch_H_pin[tu_count];
            tu_channel[tu_count]->gpio_conf.switch_H.Mode = LL_GPIO_MODE_ALTERNATE;
            tu_channel[tu_count]->gpio_conf.switch_H.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
            tu_channel[tu_count]->gpio_conf.switch_H.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
            tu_channel[tu_count]->gpio_conf.switch_H.Pull = LL_GPIO_PULL_NO;
            tu_channel[tu_count]->gpio_conf.switch_H.Alternate = alternate_function[tu_count];

            tu_channel[tu_count]->gpio_conf.OUT_L = tu_output_low[tu_count];
            tu_channel[tu_count]->gpio_conf.switch_L.Pin = switch_L_pin[tu_count];
            tu_channel[tu_count]->gpio_conf.switch_L.Mode = LL_GPIO_MODE_ALTERNATE;
            tu_channel[tu_count]->gpio_conf.switch_L.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
            tu_channel[tu_count]->gpio_conf.switch_L.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
            tu_channel[tu_count]->gpio_conf.switch_L.Pull = LL_GPIO_PULL_NO;
            tu_channel[tu_count]->gpio_conf.switch_L.Alternate = alternate_function[tu_count];

            tu_channel[tu_count]->gpio_conf.tu_gpio_CLK = clk_gpio[tu_count];
            tu_channel[tu_count]->gpio_conf.unit = unit[tu_count];

            // sets up the switching convention variables of the timing units

            tu_channel[tu_count]->switch_conv.set_H[conv_PWMx1] = SET_PER;
            tu_channel[tu_count]->switch_conv.reset_H[conv_PWMx1] = RST_CMP1;
            tu_channel[tu_count]->switch_conv.set_L[conv_PWMx1] = SET_CMP1;
            tu_channel[tu_count]->switch_conv.reset_L[conv_PWMx1] = RST_PER;

            tu_channel[tu_count]->switch_conv.set_H[conv_PWMx2] = SET_CMP1;
            tu_channel[tu_count]->switch_conv.reset_H[conv_PWMx2] = RST_PER;
            tu_channel[tu_count]->switch_conv.set_L[conv_PWMx2] = SET_PER;
            tu_channel[tu_count]->switch_conv.reset_L[conv_PWMx2] = RST_CMP1;

            tu_channel[tu_count]->switch_conv.convention = conv_PWMx1;

            tu_channel[tu_count]->comp_usage.cmp1 = FREE;
            tu_channel[tu_count]->comp_usage.cmp1_value = 0;
            tu_channel[tu_count]->comp_usage.cmp2 = FREE;
            tu_channel[tu_count]->comp_usage.cmp2_value = 0;
            tu_channel[tu_count]->comp_usage.cmp3 = FREE;
            tu_channel[tu_count]->comp_usage.cmp3_value = 0;
            tu_channel[tu_count]->comp_usage.cmp4 = FREE;
            tu_channel[tu_count]->comp_usage.cmp4_value = 0;

            // sets up the adc events variables of the timing units
            tu_channel[tu_count]->adc_hrtim.adc_event = tu_adc_events[tu_count];
            tu_channel[tu_count]->adc_hrtim.adc_source = tu_adc_source[tu_count];
            tu_channel[tu_count]->adc_hrtim.adc_trigger = tu_adc_trigger[tu_count];
            tu_channel[tu_count]->adc_hrtim.adc_rollover = EdgeTrigger_up;

            tu_channel[tu_count]->pwm_conf.unit_on = true; // timer initialised
        }
    }
}

uint16_t hrtim_tu_init(hrtim_tu_number_t tu_number)
{
    if (tu_channel[tu_number]->pwm_conf.unit_on == false)
        hrtim_init_default_all(); // default initialization for all timing unit

    if (timerMaster.pwm_conf.unit_on == UNIT_OFF)
        _init_master(); // if master timer not initialized, we initialize it

    if (tu_channel[tu_number]->pwm_conf.pwm_mode == CURRENT_MODE)
        tu_channel[tu_number]->pwm_conf.modulation = Lft_aligned; // If we are in current mode, only lft_aligned is supported

    // setting the adc roll-over mode on period event in center-aligned modulation
    if (tu_channel[tu_number]->pwm_conf.modulation == UpDwn)
        LL_HRTIM_TIM_SetADCRollOverMode(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, tu_channel[tu_number]->adc_hrtim.adc_rollover);

    /* Timer initialization for leg 1 */
    hrtim_tu_gpio_init(tu_number);                                              // initialize timing unit
    hrtim_dt_init(tu_number);                                                   // Set the dead time. Note: this must be done before enable counter
    hrtim_cnt_en(tu_number);                                                    // Enable counter
    hrtim_rst_evt_en(tu_number, tu_channel[tu_number]->phase_shift.reset_trig); // Set the timer reset trigger event
    hrtim_phase_shift_set(tu_number, 0);                                        // We initialize the phase shift to 0, all the timer will reset on master timer reset event

    if (tu_channel[tu_number]->pwm_conf.modulation == UpDwn && tu_channel[tu_number]->pwm_conf.pwm_mode == VOLTAGE_MODE)
    {
        /* IF we are in voltage mode AND Center aligned */

        tu_channel[tu_number]->switch_conv.set_H[conv_PWMx1] = SET_NONE;
        tu_channel[tu_number]->switch_conv.reset_H[conv_PWMx1] = RST_CMP1;
        tu_channel[tu_number]->switch_conv.set_L[conv_PWMx1] = SET_CMP1;
        tu_channel[tu_number]->switch_conv.reset_L[conv_PWMx1] = RST_NONE;

        tu_channel[tu_number]->switch_conv.set_H[conv_PWMx2] = SET_CMP1;
        tu_channel[tu_number]->switch_conv.reset_H[conv_PWMx2] = RST_NONE;
        tu_channel[tu_number]->switch_conv.set_L[conv_PWMx2] = SET_NONE;
        tu_channel[tu_number]->switch_conv.reset_L[conv_PWMx2] = RST_CMP1;
    }
    else if (tu_channel[tu_number]->pwm_conf.pwm_mode == CURRENT_MODE)
    {
        /* IF we are in current mode */

        /* External event initialization */
        _CM_init_EEV();

        /* Dual DAC trigger initialization */
        DualDAC_init(tu_number);

        /* Set the duty_cycle max with comparator 1  */
        uint32_t duty_cycle_max = tu_channel[tu_number]->pwm_conf.period * 0.9;
        LL_HRTIM_TIM_SetCompare1(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, duty_cycle_max);
        tu_channel[tu_number]->comp_usage.cmp1 = USED;

        /* Set the number of step with comparator 2 */
        uint32_t Sawtooth_step = tu_channel[tu_number]->pwm_conf.period / 100; // divided by 100 to have hundred steps
        LL_HRTIM_TIM_SetCompare2(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, Sawtooth_step);
        tu_channel[tu_number]->comp_usage.cmp2 = USED;

        /* Compare 4 is used to set the PWM. A small delay is required from
        the beginning of the switching period as if the comparator trip event
        must be cleared before the output can be set HIGH by one of the set sources.
        a raw value of 1088 represents 200ns when resolution is 184ps.
        see AN5497. */
        LL_HRTIM_TIM_SetCompare4(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, 1088);
        tu_channel[tu_number]->comp_usage.cmp4 = USED;

        tu_channel[tu_number]->switch_conv.set_H[conv_PWMx1] = SET_CMP4;
        tu_channel[tu_number]->switch_conv.reset_H[conv_PWMx1] = (RST_CMP1 | tu_channel[tu_number]->pwm_conf.external_trigger);
        tu_channel[tu_number]->switch_conv.set_L[conv_PWMx1] = (SET_CMP1 | tu_channel[tu_number]->pwm_conf.external_trigger);
        tu_channel[tu_number]->switch_conv.reset_L[conv_PWMx1] = RST_CMP4;

        tu_channel[tu_number]->switch_conv.set_H[conv_PWMx2] = (SET_CMP1 | tu_channel[tu_number]->pwm_conf.external_trigger);
        tu_channel[tu_number]->switch_conv.reset_H[conv_PWMx2] = RST_CMP4;
        tu_channel[tu_number]->switch_conv.set_L[conv_PWMx2] = SET_CMP4;
        tu_channel[tu_number]->switch_conv.reset_L[conv_PWMx2] = (RST_CMP1 | tu_channel[tu_number]->pwm_conf.external_trigger);
    }

    hrtim_cmpl_pwm_out1(tu_number);

    return tu_channel[tu_number]->pwm_conf.period; // returns the period of the timing unit
}

hrtim_tu_ON_OFF_t hrtim_get_status(hrtim_tu_number_t tu_number)
{
    return tu_channel[tu_number]->pwm_conf.unit_on;
}

void hrtim_tu_gpio_init(hrtim_tu_number_t tu_number)
{
    uint32_t freq_mult = 1;

    /* Disable output first for initialization */
    LL_HRTIM_DisableOutput(HRTIM1, tu_channel[tu_number]->gpio_conf.OUT_H);
    LL_HRTIM_DisableOutput(HRTIM1, tu_channel[tu_number]->gpio_conf.OUT_L);

    /* GPIO initialization */
    LL_AHB2_GRP1_EnableClock(tu_channel[tu_number]->gpio_conf.tu_gpio_CLK);
    LL_GPIO_Init(tu_channel[tu_number]->gpio_conf.unit, &(tu_channel[tu_number]->gpio_conf.switch_H));
    LL_GPIO_Init(tu_channel[tu_number]->gpio_conf.unit, &(tu_channel[tu_number]->gpio_conf.switch_L));

    /* At start-up, it is mandatory to initialize first the prescaler
     * bitfields before writing the compare and period registers. */
    if (tu_channel[tu_number]->pwm_conf.modulation == UpDwn && tu_channel[tu_number]->pwm_conf.pwm_mode == VOLTAGE_MODE)
        freq_mult = 2;
    tu_channel[tu_number]->pwm_conf.frequency = _period_ckpsc(freq_mult * tu_channel[tu_number]->pwm_conf.frequency, tu_channel[tu_number]);
    LL_HRTIM_TIM_SetPrescaler(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, tu_channel[tu_number]->pwm_conf.ckpsc);

    /* timer initialization */
    /* continuous mode, preload enabled on repetition event */

    LL_HRTIM_TIM_SetCounterMode(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, LL_HRTIM_MODE_CONTINUOUS);
    LL_HRTIM_TIM_EnablePreload(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu);
    LL_HRTIM_TIM_SetUpdateTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, LL_HRTIM_UPDATETRIG_REPETITION);

    /* Set the counting mode, left aligned or up-down (center aligned) */
    LL_HRTIM_TIM_SetCountingMode(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, tu_channel[tu_number]->pwm_conf.modulation);

    /* Configure the PWM frequency by setting the period registers */
    LL_HRTIM_TIM_SetPeriod(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, tu_channel[tu_number]->pwm_conf.period);
}

void hrtim_out_dis(hrtim_tu_number_t tu_number)
{
    LL_HRTIM_DisableOutput(HRTIM1, tu_channel[tu_number]->gpio_conf.OUT_H);
    LL_HRTIM_DisableOutput(HRTIM1, tu_channel[tu_number]->gpio_conf.OUT_L);
}

void hrtim_out_en(hrtim_tu_number_t tu_number)
{
    LL_HRTIM_EnableOutput(HRTIM1, tu_channel[tu_number]->gpio_conf.OUT_H);
    LL_HRTIM_EnableOutput(HRTIM1, tu_channel[tu_number]->gpio_conf.OUT_L);
}

void hrtim_out_dis_single(hrtim_output_units_t PWM_OUT)
{
    LL_HRTIM_DisableOutput(HRTIM1, PWM_OUT);
}

void hrtim_out_en_single(hrtim_output_units_t PWM_OUT)
{
    LL_HRTIM_EnableOutput(HRTIM1, PWM_OUT);
}

void hrtim_set_modulation(hrtim_tu_number_t tu_number, hrtim_cnt_t modulation)
{
    tu_channel[tu_number]->pwm_conf.modulation = modulation;
}

hrtim_cnt_t hrtim_get_modulation(hrtim_tu_number_t tu_number)
{
    return tu_channel[tu_number]->pwm_conf.modulation;
}

void hrtim_set_switch_convention(hrtim_tu_number_t tu_number, hrtim_switch_convention_t convention)
{
    tu_channel[tu_number]->switch_conv.convention = convention;
}

hrtim_switch_convention_t hrtim_get_switch_convention(hrtim_tu_number_t tu_number)
{
    return tu_channel[tu_number]->switch_conv.convention;
}

void hrtim_cmpl_pwm_out1(hrtim_tu_number_t tu_number)
{
    LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, tu_channel[tu_number]->gpio_conf.OUT_H, tu_channel[tu_number]->switch_conv.set_H[tu_channel[tu_number]->switch_conv.convention]);
    LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, tu_channel[tu_number]->gpio_conf.OUT_H, tu_channel[tu_number]->switch_conv.reset_H[tu_channel[tu_number]->switch_conv.convention]);
}

void hrtim_cmpl_pwm_out2(hrtim_tu_number_t tu_number)
{
    LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, tu_channel[tu_number]->gpio_conf.OUT_L, tu_channel[tu_number]->switch_conv.set_L[tu_channel[tu_number]->switch_conv.convention]);
    LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, tu_channel[tu_number]->gpio_conf.OUT_L, tu_channel[tu_number]->switch_conv.reset_L[tu_channel[tu_number]->switch_conv.convention]);
}

void hrtim_frequency_set(uint32_t value)
{
    timerMaster.pwm_conf.frequency = value;
    tu_channel[PWMA]->pwm_conf.frequency = value;
    tu_channel[PWMB]->pwm_conf.frequency = value;
    tu_channel[PWMC]->pwm_conf.frequency = value;
    tu_channel[PWMD]->pwm_conf.frequency = value;
    tu_channel[PWME]->pwm_conf.frequency = value;
    tu_channel[PWMF]->pwm_conf.frequency = value;
}

inline uint16_t hrtim_period_Master_get()
{
    return timerMaster.pwm_conf.period;
}

inline uint16_t hrtim_period_get(hrtim_tu_number_t tu_number)
{
    return tu_channel[tu_number]->pwm_conf.period; // returns the value of the period
}

uint32_t hrtim_period_Master_get_us()
{
    return timerMaster.pwm_conf.period * HRTIM_CLK_RESOLUTION;
}

uint32_t hrtim_period_get_us(hrtim_tu_number_t tu_number)
{
    uint32_t mult = 1;
    if (tu_channel[tu_number]->pwm_conf.modulation == UpDwn)
        mult = 2;
    return tu_channel[tu_number]->pwm_conf.period * HRTIM_CLK_RESOLUTION * mult;
}

/* CMP1, CMP2 and CMP3 must not be changed in current mode since they are used */
void hrtim_tu_cmp_set(hrtim_tu_number_t tu_number, hrtim_cmp_t cmp, uint16_t value)
{
    switch (cmp)
    {
    case CMP1xR:
        if (tu_channel[tu_number]->pwm_conf.pwm_mode != CURRENT_MODE)
        {
            LL_HRTIM_TIM_SetCompare1(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, value);
            tu_channel[tu_number]->comp_usage.cmp1 = USED;
            tu_channel[tu_number]->comp_usage.cmp1_value = value;
        }
        break;
    case CMP2xR:
        if (tu_channel[tu_number]->pwm_conf.pwm_mode != CURRENT_MODE)
        {
            LL_HRTIM_TIM_SetCompare2(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, value);
            tu_channel[tu_number]->comp_usage.cmp2 = USED;
            tu_channel[tu_number]->comp_usage.cmp2_value = value;
        }
        break;
    case CMP3xR:
        LL_HRTIM_TIM_SetCompare3(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, value);
        tu_channel[tu_number]->comp_usage.cmp3 = USED;
        tu_channel[tu_number]->comp_usage.cmp3_value = value;
        break;
    case CMP4xR:
        if (tu_channel[tu_number]->pwm_conf.pwm_mode != CURRENT_MODE)
        {
            LL_HRTIM_TIM_SetCompare4(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, value);
            tu_channel[tu_number]->comp_usage.cmp4 = USED;
            tu_channel[tu_number]->comp_usage.cmp4_value = value;
        }
        break;
    default:
        break;
    }
}

void hrtim_master_cmp_set(hrtim_cmp_t cmp, uint16_t value)
{
    switch (cmp)
    {
    case MCMP1R:
        LL_HRTIM_TIM_SetCompare1(HRTIM1, LL_HRTIM_TIMER_MASTER, value);
        timerMaster.comp_usage.cmp1 = USED;
        timerMaster.comp_usage.cmp1_value = value;
        break;
    case MCMP2R:
        LL_HRTIM_TIM_SetCompare2(HRTIM1, LL_HRTIM_TIMER_MASTER, value);
        timerMaster.comp_usage.cmp2 = USED;
        timerMaster.comp_usage.cmp2_value = value;
        break;
    case MCMP3R:
        LL_HRTIM_TIM_SetCompare3(HRTIM1, LL_HRTIM_TIMER_MASTER, value);
        timerMaster.comp_usage.cmp3 = USED;
        timerMaster.comp_usage.cmp3_value = value;
        break;
    case MCMP4R:
        LL_HRTIM_TIM_SetCompare4(HRTIM1, LL_HRTIM_TIMER_MASTER, value);
        timerMaster.comp_usage.cmp4 = USED;
        timerMaster.comp_usage.cmp4_value = value;
        break;
    default:
        break;
    }
}

void hrtim_pwm_mode_set(hrtim_tu_number_t tu_number, hrtim_pwm_mode_t mode)
{
    tu_channel[tu_number]->pwm_conf.pwm_mode = mode;
}

hrtim_pwm_mode_t hrtim_pwm_mode_get(hrtim_tu_number_t tu_number)
{
    return tu_channel[tu_number]->pwm_conf.pwm_mode;
}

void hrtim_eev_set(hrtim_tu_number_t tu_number, hrtim_external_trigger_t eev)
{
    tu_channel[tu_number]->pwm_conf.external_trigger = eev;
}

hrtim_external_trigger_t hrtim_eev_get(hrtim_tu_number_t tu_number)
{
    return tu_channel[tu_number]->pwm_conf.external_trigger;
}

void hrtim_dt_set(hrtim_tu_number_t tu_number, uint16_t rise_ns, uint16_t fall_ns)
{
    tu_channel[tu_number]->pwm_conf.rise_dead_time = rise_ns; // updates the rise dead time on the structure
    tu_channel[tu_number]->pwm_conf.fall_dead_time = fall_ns; // updates the fall dead time on the structure
    hrtim_cnt_dis(tu_number);                                 // Disable the timing unit counter
    hrtim_dt_init(tu_number);                                 // Set the dead time. Note: this must be done before enable counter
    hrtim_cnt_en(tu_number);                                  // Enable counter
}

/* Duty_cycle should not be set if we are in current mode */
void hrtim_duty_cycle_set(hrtim_tu_number_t tu_number, uint16_t value)
{
    if (value != tu_channel[tu_number]->pwm_conf.duty_cycle && tu_channel[tu_number]->pwm_conf.pwm_mode != CURRENT_MODE)
    {
        tu_channel[tu_number]->pwm_conf.duty_cycle = value;

        /* Set comparator for duty cycle */
        hrtim_tu_cmp_set(tu_number, CMP1xR, value);
    }
}

void hrtim_phase_shift_set(hrtim_tu_number_t tu_number, uint16_t shift)
{
    tu_channel[tu_number]->phase_shift.value = shift;

    /* Set reset comparator for phase positioning */
    if (shift)
    {
        LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, LL_HRTIM_TIM_GetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu) & ~timerMaster.phase_shift.reset_trig);
        switch (tu_channel[tu_number]->pwm_conf.pwm_tu)
        {
        /* Timer A is the phase shift reference so it can't be phase
         * shifted */
        case TIMB: /* Timer B reset on timerA comp 2 */
            if (tu_channel[PWMA]->comp_usage.cmp2 != USED && tu_channel[PWMA]->pwm_conf.modulation != UpDwn)
            {
                LL_HRTIM_TIM_SetCompare2(HRTIM1, LL_HRTIM_TIMER_A, shift); // if timA comp2 is used (e.g. in current mode) and timA is not left-aligned we can't phaseshift timB
                LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, tu_channel[tu_number]->phase_shift.reset_trig);
            }
            else
                LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, timerMaster.phase_shift.reset_trig); // the reset of timerB is on master PER event
            break;
        case TIMC: /* Timer C reset on master cmp2 */
            LL_HRTIM_TIM_SetCompare2(HRTIM1, LL_HRTIM_TIMER_MASTER, shift);
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, tu_channel[tu_number]->phase_shift.reset_trig);
            break;
        case TIMD: /* Timer D reset on master cmp3 */
            LL_HRTIM_TIM_SetCompare3(HRTIM1, LL_HRTIM_TIMER_MASTER, shift);
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, tu_channel[tu_number]->phase_shift.reset_trig);
            break;
        case TIME: /* Timer E reset on master cmp4 */
            LL_HRTIM_TIM_SetCompare4(HRTIM1, LL_HRTIM_TIMER_MASTER, shift);
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, tu_channel[tu_number]->phase_shift.reset_trig);
            break;
        case TIMF: /* Timer F reset on master cmp1 */
            LL_HRTIM_TIM_SetCompare1(HRTIM1, LL_HRTIM_TIMER_MASTER, shift);
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, tu_channel[tu_number]->phase_shift.reset_trig);
            break;
        default:
            break;
        }
    }
    else if ((timerMaster.pwm_conf.period == tu_channel[tu_number]->pwm_conf.period && timerMaster.pwm_conf.ckpsc == tu_channel[tu_number]->pwm_conf.ckpsc) || (timerMaster.pwm_conf.period == 2 * (tu_channel[tu_number]->pwm_conf.period) && timerMaster.pwm_conf.ckpsc == tu_channel[tu_number]->pwm_conf.ckpsc))
    {
        /* shift == 0 and timing unit run at the same frequency as master */
        if (tu_channel[tu_number]->pwm_conf.pwm_tu != TIMA)
        {
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, LL_HRTIM_TIM_GetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu) & ~tu_channel[tu_number]->phase_shift.reset_trig);
            LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, timerMaster.phase_shift.reset_trig);
        }
    }
    else
    {
        /* timing unit do not run at the same frequency as master so
         * phase positioning is not applicable */
        LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, LL_HRTIM_TIM_GetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu) & ~timerMaster.phase_shift.reset_trig);
    }
}

/* Note : The dead time is configured centered by default,
 * there are no options available to modify this, so the dead time
 * must be taken into account when calculating the PWM duty cycle */
void hrtim_dt_init(hrtim_tu_number_t tu_number)
{
    uint32_t rise_ps = tu_channel[tu_number]->pwm_conf.rise_dead_time * 1000;
    uint32_t fall_ps = tu_channel[tu_number]->pwm_conf.fall_dead_time * 1000;
/* t_dtg = (2^dtpsc) * (t_hrtim / 8)
 *       = (2^dtpsc) / (f_hrtim * 8) */
#if defined(CONFIG_SOC_SERIES_STM32F3X)
    uint32_t f_hrtim = hrtim_get_apb2_clock() * 2;
#elif defined(CONFIG_SOC_SERIES_STM32G4X)
    uint32_t f_hrtim = hrtim_get_apb2_clock();
#else
#warning "unsupported stm32XX family"
#endif

    uint8_t dtpsc = 0;                                                      // Deadtime clock prescaler set at xx
    uint32_t t_dtg_ps = (1 << dtpsc) * 1000000 / ((f_hrtim * 8) / 1000000); // intermediate gain for dead time calculation
    uint16_t rise_dt = rise_ps / t_dtg_ps;                                  // calculate the register value based on desired deadtime in picoseconds

    while (rise_dt > 511 && dtpsc < 7)
    {
        dtpsc++;
        t_dtg_ps = (1 << dtpsc) * 1000000 / ((f_hrtim * 8) / 1000000);
        rise_dt = rise_ps / t_dtg_ps;
    }
    if (rise_dt > 511)
    {
        rise_dt = 511;
    }

    uint16_t fall_dt = fall_ps / t_dtg_ps; // calculate the register value based on desired deadtime in picoseconds

    while (fall_dt > 511 && dtpsc < 7)
    {
        dtpsc++;
        t_dtg_ps = (1 << dtpsc) * 1000000 / ((f_hrtim * 8) / 1000000);
        fall_dt = fall_ps / t_dtg_ps;
    }
    if (fall_dt > 511)
    {
        fall_dt = 511;
    }

    LL_HRTIM_DT_SetPrescaler(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, dtpsc); // Deadtime clock prescaler

    LL_HRTIM_DT_SetFallingValue(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, fall_dt); // Deadtime falling edge value

    LL_HRTIM_DT_SetRisingValue(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, rise_dt); // Deadtime rising edge value

    // LL_HRTIM_DT_SetFallingSign(hrtim, tu, LL_HRTIM_DT_FALLING_NEGATIVE);  // Change the behavior of the deadtime insertion by overlapping the signals (negative falling edge)

    // LL_HRTIM_DT_SetRisingSign(hrtim, tu, LL_HRTIM_DT_RISING_POSITIVE); // change the behavior of the deadtime insertion by overlapping the signals (positive falling edge)

    LL_HRTIM_TIM_EnableDeadTime(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu); /* Note: This
                                                                                  * parameter cannot be changed once the timer is operating (TxEN bit
                                                                                  * set) or if its outputs are enabled and set/reset by another timer. */
}

void hrtim_cnt_en(hrtim_tu_number_t tu_number)
{
    LL_HRTIM_TIM_CounterEnable(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu);
}

void hrtim_cnt_dis(hrtim_tu_number_t tu_number)
{
    LL_HRTIM_TIM_CounterDisable(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu);
}

void hrtim_rst_evt_en(hrtim_tu_number_t tu_number, hrtim_reset_trig_t evt)
{
    LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, evt);
}

void hrtim_rst_evt_dis(hrtim_tu_number_t tu_number, hrtim_reset_trig_t evt)
{
    LL_HRTIM_TIM_SetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, LL_HRTIM_TIM_GetResetTrig(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu) & ~evt);
}

void hrtim_adc_trigger_set_postscaler(hrtim_tu_number_t tu_number, uint32_t ps_ratio)
{
    LL_HRTIM_SetADCPostScaler(HRTIM1, tu_channel[tu_number]->adc_hrtim.adc_trigger, ps_ratio);
}

void hrtim_adc_trigger_en(hrtim_tu_number_t tu_number)
{
    if (tu_channel[tu_number]->adc_hrtim.adc_trigger != ADCTRIG_NONE)
    {
        LL_HRTIM_SetADCTrigSrc(HRTIM1, tu_channel[tu_number]->adc_hrtim.adc_trigger, tu_channel[tu_number]->adc_hrtim.adc_source);
        LL_HRTIM_SetADCTrigUpdate(HRTIM1, tu_channel[tu_number]->adc_hrtim.adc_trigger, tu_channel[tu_number]->adc_hrtim.adc_event);
    }
}

void hrtim_adc_trigger_dis(hrtim_tu_number_t tu_number)
{
    LL_HRTIM_SetADCTrigSrc(HRTIM1, tu_channel[tu_number]->adc_hrtim.adc_trigger, LL_HRTIM_GetADCTrigSrc(HRTIM1, tu_channel[tu_number]->adc_hrtim.adc_trigger) & ~tu_channel[tu_number]->adc_hrtim.adc_source);
}

void hrtim_adc_triger_set(hrtim_tu_number_t tu_number, hrtim_adc_trigger_t adc_trig)
{
    tu_channel[tu_number]->adc_hrtim.adc_trigger = adc_trig;
}

hrtim_adc_trigger_t hrtim_adc_triger_get(hrtim_tu_number_t tu_number)
{
    return tu_channel[tu_number]->adc_hrtim.adc_trigger;
}

void hrtim_adc_rollover_set(hrtim_tu_number_t tu_number, hrtim_adc_edgetrigger_t adc_rollover)
{
    tu_channel[tu_number]->adc_hrtim.adc_rollover = adc_rollover;
}

hrtim_adc_edgetrigger_t hrtim_adc_rollover_get(hrtim_tu_number_t tu_number)
{
    return tu_channel[tu_number]->adc_hrtim.adc_rollover;
}

void hrtim_PeriodicEvent_configure(hrtim_tu_t tu, uint32_t repetition, hrtim_callback_t callback)
{
    /* Memorize user callback */
    user_callback = callback;

    /* Set repetition counter to repetition-1 so that an event
     * is triggered every "repetition" number of periods.
     */
    LL_HRTIM_TIM_SetRepetition(HRTIM1, tu, repetition - 1);
}

void hrtim_PeriodicEvent_en(hrtim_tu_t tu)
{
    if (LL_HRTIM_GetSyncInSrc(HRTIM1) == LL_HRTIM_SYNCIN_SRC_NONE)
        LL_HRTIM_EnableIT_REP(HRTIM1, tu); /* Enabling the interrupt on repetition counter event*/

    if (LL_HRTIM_GetSyncInSrc(HRTIM1) == LL_HRTIM_SYNCIN_SRC_EXTERNAL_EVENT)
        LL_HRTIM_EnableIT_SYNC(HRTIM1); /* Enabling interruption on synch pulse in case of slave communication mode*/

    IRQ_CONNECT(HRTIM_IRQ_NUMBER, HRTIM_IRQ_PRIO, _hrtim_callback, NULL, HRTIM_IRQ_FLAGS);
    irq_enable(HRTIM_IRQ_NUMBER);
}

void hrtim_PeriodicEvent_dis(hrtim_tu_t tu)
{
    irq_disable(HRTIM_IRQ_NUMBER);
    LL_HRTIM_DisableIT_REP(HRTIM1, tu); /* Disabling the interrupt on repetition counter event*/
}

void hrtim_PeriodicEvent_SetRep(hrtim_tu_t tu, uint32_t repetition)
{
    /* Set repetition counter to repetition-1 so that an event
     * is triggered every "repetition" number of periods.
     */
    LL_HRTIM_TIM_SetRepetition(HRTIM1, tu, repetition - 1);
}

uint32_t hrtim_PeriodicEvent_GetRep(hrtim_tu_t tu)
{
    return LL_HRTIM_TIM_GetRepetition(HRTIM1, tu) + 1;
}

void DualDAC_init(hrtim_tu_number_t tu_number)
{
    LL_HRTIM_TIM_SetDualDacResetTrigger(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, LL_HRTIM_DCDR_COUNTER);
    LL_HRTIM_TIM_SetDualDacStepTrigger(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu, LL_HRTIM_DCDS_CMP2);
    LL_HRTIM_TIM_EnableDualDacTrigger(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu);
}