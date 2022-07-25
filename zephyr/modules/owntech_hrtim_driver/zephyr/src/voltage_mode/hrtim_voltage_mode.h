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
 * @defgroup    drivers_periph_hrtim OwnTech's hrtim module
 * @ingroup     drivers_periph
 * @brief       Low-level HRTIM peripheral driver
 *
 * @file
 * @brief       Low-level HRTIM peripheral driver interface definitions
 *
 * @author      Hugues Larrive <hugues.larrive@laas.fr>
 * @author      Clément Foucher <clement.foucher@laas.fr>
 * @author      Antoine Boche <antoine.boche@laas.fr>
 * @author      Luiz Villa <luiz.villa@laas.fr>
 * @author      Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef HRTIM_VOLTAGE_MODE_H_
#define HRTIM_VOLTAGE_MODE_H_

#include <stdint.h>
#include <limits.h>
#include <stm32_ll_hrtim.h>

#include <pinmux/pinmux_stm32.h>

#include "hrtim.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DT_DRV_COMPAT hrtim
#define HRTIM_NUMOF (1)

/**
 * @brief   Default HRTIM access macro
 */
#ifndef HRTIM_DEV
#define HRTIM_DEV(x)          (x)
#endif

/**
 * @brief  Default HRTIM undefined value
 */
#ifndef HRTIM_UNDEF
#define HRTIM_UNDEF           (UINT_MAX)
#endif


/**
 * @brief   HRTIM comparators definition
 */
typedef enum {
    CMP1xR = 1,
    CMP2xR = 2,
    CMP3xR = 3,
    CMP4xR = 4,
    MCMP1R = 1,
    MCMP2R = 2,
    MCMP3R = 3,
    MCMP4R = 4
} hrtim_cmp_t;



typedef enum{

    OUT1 = 1, 
    OUT2 = 2
} hrtim_out_t;


/**
 * @brief   HRTIM ADC trigger registers definition
 */
typedef enum {
    ADC1R = 1,
    ADC2R = 2,
    ADC3R = 3,
    ADC4R = 4
} hrtim_adc_t;


/**
 * @brief  HRTIM counting mode setting
 * 
 */
typedef enum {

    Lft_aligned =1,
    UpDwn = 2 

}hrtim_cnt_t;



/**
 * @brief   Initialize an HRTIM device and all these timing units for
 *          complementary pwm outputs with a dead time.
 *
 * @param[in] dev      HRTIM device to initialize
 * @param[inout] freq  HRTIM frequency in Hz
 * @param[in] dt       Desired dead time in ns
 * @param[in] leg1_upper_switch_convention   Choice of the switch convention for leg 1, can be one of the following values:
 *            @arg @ref True (Buck mode)
 *            @arg @ref False (Boost mode)
 * @param[in] leg2_upper_switch_convention   Choice of the switch convention for leg 2, can be one of the following values:
 *            @arg @ref True (Buck mode)
 *            @arg @ref False (Boost mode)
 * @param[in] leg1_tu   Timer for leg 1
 * @param[in] leg2_tu   Timer for leg 2
 * @return              actual HRTIM resolution on success
 * @return              0 on error
 */
uint16_t hrtim_init(hrtim_t dev, uint32_t *freq, uint16_t dead_time_ns, uint8_t leg1_upper_switch_convention, uint8_t leg2_upper_switch_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);

/**
 * @brief   Initialize an HRTIM device and all these timing units for
 *          complementary pwm outputs with a dead time with Up-Down mode (center alligned).
 *
 * @param[in] dev      HRTIM device to initialize
 * @param[inout] freq  HRTIM frequency in Hz
 * @param[in] dead_time_ns       Desired dead time in ns
 * @param[in] leg1_upper_switch_convention   Choice of the switch convention for leg 1, can be one of the following values:
 *            @arg @ref True (Buck mode)
 *            @arg @ref False (Boost mode)
 * @param[in] leg2_upper_switch_convention   Choice of the switch convention for leg 2, can be one of the following values:
 *            @arg @ref True (Buck mode)
 *            @arg @ref False (Boost mode)
 * @param[in] leg1_tu   Timer for leg 1
 * @param[in] leg2_tu   Timer for leg 2
 * @return              actual HRTIM resolution on success
 * @return              0 on error
 */
uint16_t hrtim_init_updwn(hrtim_t dev, uint32_t *freq, uint16_t dt, uint8_t leg1_upper_switch_convention, uint8_t leg2_upper_switch_convention,hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);

/**
 * @brief   Updates the duty cycle
 *
 * @param[in] dev       HRTIM device to be used
 * @param[in] tu        Timing unit to updated, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF*
 * @param[in] rise_ns       Rising edge dead time in ns
 * @param[in] fall_ns       Falling edge dead time in ns
 */
void hrtim_update_dead_time(hrtim_t dev, hrtim_tu_t tu, uint16_t rise_ns, uint16_t fall_ns);

/**
 * @brief   Initialize an HRTIM device master timer
 *
 * @param[in] dev       HRTIM device to initialize
 * @param[inout] freq   HRTIM frequency in Hz
 *
 * @return              actual HRTIM resolution on success
 * @return              0 on error
 */
uint16_t hrtim_init_master(hrtim_t dev, uint32_t *freq);

/**
 * @brief   Initialize a timing unit
 *
 * @param[in] dev       HRTIM device
 * @param[in] tu        Timing unit to initialize, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 * @param[in] cnt_mode  Counting mode 
 *            @arg @ref Lft_aligned
 *            @arg @ref UpDwn 
 * @param[inout] freq   HRTIM frequency in Hz
 *
 * @return              actual timing unit resolution on success
 * @return              0 on error
 */
uint16_t hrtim_init_tu(hrtim_t dev, hrtim_tu_t tu, uint32_t *freq, hrtim_cnt_t cnt_mode);

/**
 * @brief   Set crossbar(s) setting
 *
 * @param[in] dev       HRTIM device
 * @param[in] out       Output, can be one of the following values:
 *            @arg @ref LL_HRTIM_OUTPUT_TA1
 *            @arg @ref LL_HRTIM_OUTPUT_TA2
 *            @arg @ref LL_HRTIM_OUTPUT_TB1
 *            @arg @ref LL_HRTIM_OUTPUT_TB2
 *            @arg @ref LL_HRTIM_OUTPUT_TC1
 *            @arg @ref LL_HRTIM_OUTPUT_TC2
 *            @arg @ref LL_HRTIM_OUTPUT_TD1
 *            @arg @ref LL_HRTIM_OUTPUT_TD2
 *            @arg @ref LL_HRTIM_OUTPUT_TE1
 *            @arg @ref LL_HRTIM_OUTPUT_TE2
 *            @arg @ref LL_HRTIM_OUTPUT_TF1
 *            @arg @ref LL_HRTIM_OUTPUT_TF2 
 * @param[in] cb        Set crossbar(s)
 */
void hrtim_set_cb_set(hrtim_t dev, uint32_t out,
                        uint32_t cb);

/**
 * @brief   Unset set crossbar(s)
 *
 * @param[in] dev       HRTIM device
 * @param[in] out       Output, can be one of the following values:
 *            @arg @ref LL_HRTIM_OUTPUT_TA1
 *            @arg @ref LL_HRTIM_OUTPUT_TA2
 *            @arg @ref LL_HRTIM_OUTPUT_TB1
 *            @arg @ref LL_HRTIM_OUTPUT_TB2
 *            @arg @ref LL_HRTIM_OUTPUT_TC1
 *            @arg @ref LL_HRTIM_OUTPUT_TC2
 *            @arg @ref LL_HRTIM_OUTPUT_TD1
 *            @arg @ref LL_HRTIM_OUTPUT_TD2
 *            @arg @ref LL_HRTIM_OUTPUT_TE1
 *            @arg @ref LL_HRTIM_OUTPUT_TE2
 *            @arg @ref LL_HRTIM_OUTPUT_TF1
 *            @arg @ref LL_HRTIM_OUTPUT_TF2 
 * @param[in] cb        Set crossbar(s)
 *            @arg @ref LL_HRTIM_OUTPUTSET_NONE
 *            @arg @ref LL_HRTIM_OUTPUTSET_RESYNC
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMPER
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_MASTERPER
 *            @arg @ref LL_HRTIM_OUTPUTSET_MASTERCMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_MASTERCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_MASTERCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_MASTERCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMAEV1_TIMBCMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMAEV2_TIMBCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMAEV3_TIMCCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMAEV4_TIMCCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMAEV5_TIMDCMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMAEV6_TIMDCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMAEV7_TIMECMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMAEV8_TIMECMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMAEV9_TIMFCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMBEV1_TIMACMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMBEV2_TIMACMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMBEV3_TIMCCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMBEV4_TIMCCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMBEV5_TIMDCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMBEV6_TIMDCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMBEV7_TIMECMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMBEV8_TIMECMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMBEV9_TIMFCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCEV1_TIMACMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCEV2_TIMACMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCEV3_TIMBCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCEV4_TIMBCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCEV5_TIMDCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCEV6_TIMDCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCEV7_TIMECMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCEV8_TIMECMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMCEV9_TIMFCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMDEV1_TIMACMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMDEV2_TIMACMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMDEV3_TIMBCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMDEV4_TIMBCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMDEV5_TIMCCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMDEV6_TIMECMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMDEV7_TIMECMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMDEV8_TIMFCMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMDEV9_TIMFCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMEEV1_TIMACMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMEEV2_TIMBCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMEEV3_TIMBCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMEEV4_TIMCCMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMEEV5_TIMCCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMEEV6_TIMDCMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMEEV7_TIMDCMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMEEV8_TIMFCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMEEV9_TIMFCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMFEV1_TIMACMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMFEV2_TIMBCMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMFEV3_TIMBCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMFEV4_TIMCCMP1
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMFEV5_TIMCCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMFEV6_TIMDCMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMFEV7_TIMDCMP4
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMFEV8_TIMECMP2
 *            @arg @ref LL_HRTIM_OUTPUTSET_TIMFEV9_TIMECMP3
 *            @arg @ref LL_HRTIM_OUTPUTSET_EEV_1
 *            @arg @ref LL_HRTIM_OUTPUTSET_EEV_2
 *            @arg @ref LL_HRTIM_OUTPUTSET_EEV_3
 *            @arg @ref LL_HRTIM_OUTPUTSET_EEV_4
 *            @arg @ref LL_HRTIM_OUTPUTSET_EEV_5
 *            @arg @ref LL_HRTIM_OUTPUTSET_EEV_6
 *            @arg @ref LL_HRTIM_OUTPUTSET_EEV_7
 *            @arg @ref LL_HRTIM_OUTPUTSET_EEV_8
 *            @arg @ref LL_HRTIM_OUTPUTSET_EEV_9
 *            @arg @ref LL_HRTIM_OUTPUTSET_EEV_10
 *            @arg @ref LL_HRTIM_OUTPUTSET_UPDATE
 *            (source = TIMy and destination = TIMx, Compare Unit = CMPz).
 */
void hrtim_set_cb_unset(hrtim_t dev, uint32_t out,
                        uint32_t cb);

/**
 * @brief   Reset crossbar(s) setting
 *
 * @param[in] dev  HRTIM device
 * @param[in] out  Output, can be one of the following values:
 *            @arg @ref LL_HRTIM_OUTPUT_TA1
 *            @arg @ref LL_HRTIM_OUTPUT_TA2
 *            @arg @ref LL_HRTIM_OUTPUT_TB1
 *            @arg @ref LL_HRTIM_OUTPUT_TB2
 *            @arg @ref LL_HRTIM_OUTPUT_TC1
 *            @arg @ref LL_HRTIM_OUTPUT_TC2
 *            @arg @ref LL_HRTIM_OUTPUT_TD1
 *            @arg @ref LL_HRTIM_OUTPUT_TD2
 *            @arg @ref LL_HRTIM_OUTPUT_TE1
 *            @arg @ref LL_HRTIM_OUTPUT_TE2
 *            @arg @ref LL_HRTIM_OUTPUT_TF1
 *            @arg @ref LL_HRTIM_OUTPUT_TF2 
 * @param[in] cb   Reset crossbar(s), can be one of those following values: 
 *            @arg @ref LL_HRTIM_OUTPUTRESET_NONE
 *            @arg @ref LL_HRTIM_OUTPUTRESET_RESYNC
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMPER
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_MASTERPER
 *            @arg @ref LL_HRTIM_OUTPUTRESET_MASTERCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_MASTERCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_MASTERCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_MASTERCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV1_TIMBCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV2_TIMBCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV3_TIMCCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV4_TIMCCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV5_TIMDCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV6_TIMDCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV7_TIMECMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV8_TIMECMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV9_TIMFCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV1_TIMACMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV2_TIMACMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV3_TIMCCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV4_TIMCCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV5_TIMDCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV6_TIMDCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV7_TIMECMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV8_TIMECMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV9_TIMFCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV1_TIMACMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV2_TIMACMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV3_TIMBCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV4_TIMBCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV5_TIMDCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV6_TIMDCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV7_TIMECMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV8_TIMECMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV9_TIMFCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV1_TIMACMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV2_TIMACMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV3_TIMBCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV4_TIMBCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV5_TIMCCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV6_TIMECMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV7_TIMECMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV8_TIMFCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV9_TIMFCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV1_TIMACMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV2_TIMBCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV3_TIMBCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV4_TIMCCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV5_TIMCCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV6_TIMDCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV7_TIMDCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV8_TIMFCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV9_TIMFCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV1_TIMACMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV2_TIMBCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV3_TIMBCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV4_TIMCCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV5_TIMCCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV6_TIMDCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV7_TIMDCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV8_TIMECMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV9_TIMECMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_5
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_6
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_7
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_8
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_9
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_10
 *            @arg @ref LL_HRTIM_OUTPUTRESET_UPDATE
 *            (source = TIMy and destination = TIMx, Compare Unit = CMPz).
 */
void hrtim_rst_cb_set(hrtim_t dev, uint32_t out,
                        uint32_t cb);

/**
 * @brief   Unset reset crossbar(s)
 *
 * @param[in] dev HRTIM device
 * @param[in] out Output, can be one of the following values:
 *            @arg @ref LL_HRTIM_OUTPUT_TA1
 *            @arg @ref LL_HRTIM_OUTPUT_TA2
 *            @arg @ref LL_HRTIM_OUTPUT_TB1
 *            @arg @ref LL_HRTIM_OUTPUT_TB2
 *            @arg @ref LL_HRTIM_OUTPUT_TC1
 *            @arg @ref LL_HRTIM_OUTPUT_TC2
 *            @arg @ref LL_HRTIM_OUTPUT_TD1
 *            @arg @ref LL_HRTIM_OUTPUT_TD2
 *            @arg @ref LL_HRTIM_OUTPUT_TE1
 *            @arg @ref LL_HRTIM_OUTPUT_TE2
 *            @arg @ref LL_HRTIM_OUTPUT_TF1
 *            @arg @ref LL_HRTIM_OUTPUT_TF2
 * @param[in] cb  Reset crossbar(s), can be one of those following values: 
 *            @arg @ref LL_HRTIM_OUTPUTRESET_NONE
 *            @arg @ref LL_HRTIM_OUTPUTRESET_RESYNC
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMPER
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_MASTERPER
 *            @arg @ref LL_HRTIM_OUTPUTRESET_MASTERCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_MASTERCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_MASTERCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_MASTERCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV1_TIMBCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV2_TIMBCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV3_TIMCCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV4_TIMCCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV5_TIMDCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV6_TIMDCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV7_TIMECMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV8_TIMECMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMAEV9_TIMFCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV1_TIMACMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV2_TIMACMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV3_TIMCCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV4_TIMCCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV5_TIMDCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV6_TIMDCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV7_TIMECMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV8_TIMECMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMBEV9_TIMFCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV1_TIMACMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV2_TIMACMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV3_TIMBCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV4_TIMBCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV5_TIMDCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV6_TIMDCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV7_TIMECMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV8_TIMECMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMCEV9_TIMFCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV1_TIMACMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV2_TIMACMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV3_TIMBCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV4_TIMBCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV5_TIMCCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV6_TIMECMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV7_TIMECMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV8_TIMFCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMDEV9_TIMFCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV1_TIMACMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV2_TIMBCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV3_TIMBCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV4_TIMCCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV5_TIMCCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV6_TIMDCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV7_TIMDCMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV8_TIMFCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMEEV9_TIMFCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV1_TIMACMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV2_TIMBCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV3_TIMBCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV4_TIMCCMP1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV5_TIMCCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV6_TIMDCMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV7_TIMDCMP4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV8_TIMECMP2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_TIMFEV9_TIMECMP3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_1
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_2
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_3
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_4
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_5
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_6
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_7
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_8
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_9
 *            @arg @ref LL_HRTIM_OUTPUTRESET_EEV_10
 *            @arg @ref LL_HRTIM_OUTPUTRESET_UPDATE
 *            (source = TIMy and destination = TIMx, Compare Unit = CMPz).
 */
void hrtim_rst_cb_unset(hrtim_t dev, uint32_t out,
                            uint32_t cb);

/**
 * @brief   Sets up the switching convention of the leg upper switch
 *
 * @param[in] hrtim     HRTIM device
 * @param[in] tu        Timing unit, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 * @param[in] cnt_mode  Counting mode 
 *            @arg @ref Lft_aligned
 *            @arg @ref UpDwn
 * @param[in] leg1_upper_switch_convention     Choice of the switch convention - 1 for buck mode and 0 for boost mode
 */
void hrtim_cmpl_pwm_out1(hrtim_t hrtim, hrtim_tu_t tu, bool leg_upper_switch_convention, hrtim_cnt_t cnt_mode);


/**
 * @brief   Sets up the switching convention of the leg lower switch 
 *
 * @param[in] hrtim     HRTIM device
 * @param[in] tu        Timing unit, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 * @param[in] cnt_mode  Counting mode 
 *            @arg @ref Lft_aligned
 *            @arg @ref UpDwn
 * @param[in] leg1_upper_switch_convention     Choice of the switch convention - 1 for buck mode and 0 for boost mode
*/

void hrtim_cmpl_pwm_out2(hrtim_t hrtim, hrtim_tu_t tu, bool leg_upper_switch_convention, hrtim_cnt_t cnt);


/**
 * @brief   Set a period value
 *
 * @param[in] dev       HRTIM device
 * @param[in] tu        Timing unit, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 *            @arg @ref MSTR 
 * @param[in] value     Raw value to set
 */
void hrtim_period_set(hrtim_t dev, hrtim_tu_t tu, uint16_t value);

/**
 * @brief   Set a comparator value
 *
 * @param[in] dev       HRTIM device
 * @param[in] tu        Timing unit, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 *            @arg @ref MSTR 
 * @param[in] cmp       Comparator from 1 to 4, can be one of the following values:
 *            @arg @ref CMP1xR
 *            @arg @ref CMP2xR
 *            @arg @ref CMP3xR
 *            @arg @ref CMP4xR
 *            @arg @ref MCMP1R
 *            @arg @ref MCMP2R
 *            @arg @ref MCMP3R
 *            @arg @ref MCMP4R
 * @param[in] value     Raw value to set
 */
void hrtim_cmp_set(hrtim_t dev, hrtim_tu_t tu, hrtim_cmp_t cmp,
                    uint16_t value);

/**
 * @brief   Enable a timing unit counter.
 *
 * @param[in] dev       HRTIM device
 * @param[in] tu        Timing unit, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 *            @arg @ref MSTR
 */
void hrtim_cnt_en(hrtim_t dev, hrtim_tu_t tu);

/**
 * @brief   Disable a timing unit counter.
 *
 * @param[in] dev       HRTIM device
 * @param[in] tu        Timing unit, Timing unit, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 *            @arg @ref MSTR
 */
void hrtim_cnt_dis(hrtim_t dev, hrtim_tu_t tu);

/**
 * @brief   Enable a Timerx reset event
 *
 * @param[in] dev       HRTIM device
 * @param[in] tu        Timing unit, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 * @param[in] evt       Reset event, can be one of the following values: 
 *            @arg @ref LL_HRTIM_RESETTRIG_NONE
 *            @arg @ref LL_HRTIM_RESETTRIG_UPDATE
 *            @arg @ref LL_HRTIM_RESETTRIG_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_MASTER_PER
 *            @arg @ref LL_HRTIM_RESETTRIG_MASTER_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_MASTER_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_MASTER_CMP3
 *            @arg @ref LL_HRTIM_RESETTRIG_MASTER_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_1
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_2
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_3
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_4
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_5
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_6
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_7
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_8
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_9
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_10
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER1_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER1_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER1_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER2_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER2_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER2_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER3_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER3_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER3_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER4_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER4_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER4_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER5_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER5_CMP2
 */
void hrtim_rst_evt_en(hrtim_t dev, hrtim_tu_t tu, uint32_t evt);

/**
 * @brief   Disbable a TimerX (X = A,B...F) reset event
 *
 * @param[in] dev       HRTIM device
 * @param[in] tu        Timing unit, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 * @param[in] evt       Reset event, can be one of the following values: 
 *            @arg @ref LL_HRTIM_RESETTRIG_NONE
 *            @arg @ref LL_HRTIM_RESETTRIG_UPDATE
 *            @arg @ref LL_HRTIM_RESETTRIG_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_MASTER_PER
 *            @arg @ref LL_HRTIM_RESETTRIG_MASTER_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_MASTER_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_MASTER_CMP3
 *            @arg @ref LL_HRTIM_RESETTRIG_MASTER_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_1
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_2
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_3
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_4
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_5
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_6
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_7
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_8
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_9
 *            @arg @ref LL_HRTIM_RESETTRIG_EEV_10
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER1_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER1_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER1_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER2_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER2_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER2_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER3_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER3_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER3_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER4_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER4_CMP2
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER4_CMP4
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER5_CMP1
 *            @arg @ref LL_HRTIM_RESETTRIG_OTHER5_CMP2
 */
void hrtim_rst_evt_dis(hrtim_t dev, hrtim_tu_t tu, uint32_t evt);

/**
 * @brief   Enable a given output of a given timing unit.
 *
 * @param[in] dev       HRTIM device
 * @param[in] tu        Timing unit, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 * @param[in] out       Output to enable, can be one of the following values: 
 *            @arg @ref OUT1
 *            @arg @ref OUT2
 */
void hrtim_out_en(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out);

/**
 * @brief   Disable a given output of a given timing unit.
 *
 * @param[in] dev       HRTIM device
 * @param[in] tu        Timing unit, can be one of the following values:
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF
 * @param[in] out       Output to disable, can be one of the following values: 
 *            @arg @ref OUT1
 *            @arg @ref OUT2
 */
void hrtim_out_dis(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out);

/**
 * @brief   Setup a dead time in nano second for given complementary
 *          outputs.
 *
 * @param[in] dev       HRTIM device
 * @param[in] tu        Timing unit, can be one of the following values: 
 *            @arg @ref TIMA
 *            @arg @ref TIMB
 *            @arg @ref TIMC
 *            @arg @ref TIMD
 *            @arg @ref TIME
 *            @arg @ref TIMF          
 * @param[in] rise_ns        The desired dead time of the rising edge in nano second
 * @param[in] fall_ns        The desired dead time of the falling edge in nano second
 */
void hrtim_pwm_dt(hrtim_t dev, hrtim_tu_t tu, uint16_t rise_ns, uint16_t fall_ns);

/**
 * @brief   Set the HRTIM event postsaler. Postscaler ratio indicates
 *          how many potential events will be ignored between two
 *          events which are effectively generated.
 *
 * @param[in] dev       HRTIM device
 * @param[in] ps_ratio  Post scaler ratio (0 = no post scaler, default)
 */
void hrtim_adc_trigger_set_postscaler(hrtim_t dev, uint32_t ps_ratio);

/**
 * @brief   Configures and enables an ADC trigger event.
 *
 * @param[in] event_number  Number of the event to configure
 * @param[in] source_timer  Source timer of the event. 0 = Master timer, 1 = Timer A, 2 = Timer B, etc.
 * @param[in] event     Trigger event
 *                      For ADC trigger 1 and ADC trigger 3 this parameter can be a
 *                      combination of the following values:
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_NONE
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_MCMP1
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_MCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_MCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_MCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_MPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_EEV1
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_EEV2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_EEV3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_EEV4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_EEV5
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMACMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMACMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMAPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMARST
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMBCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMBCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMBPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMBRST
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMCCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMCCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMCPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMDCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMDCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMDPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMECMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMECMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMEPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMFCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMFCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMFCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMFPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMFRST
 *
 *                      For ADC trigger 2 and ADC trigger 4 this parameter can be a
 *                      combination of the following values:
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_NONE
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_MCMP1
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_MCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_MCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_MCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_MPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_EEV6
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_EEV7
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_EEV8
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_EEV9
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_EEV10
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMACMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMACMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMAPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMBCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMBCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMBPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMCCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMCCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMCPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMCRST
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMDCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMDCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMDPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMDRST
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMECMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMECMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMECMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMERST
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMFCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMFCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMFCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMFPER                   
 */
void hrtim_adc_trigger_en(uint32_t event_number, uint32_t source_timer, uint32_t event);


/**
 * @brief   Disbable a ADCx trigger event
 *
 * @param[in] dev       HRTIM device
 * @param[in] adc       ADC trigger register, can be one of the following values: 
 *            @arg @ref ADC1R
 *            @arg @ref ADC2R
 *            @arg @ref ADC3R
 *            @arg @ref ADC4R           
 * @param[in] evt       Trigger event
 *                      For ADC trigger 1 and ADC trigger 3 this parameter can be a
 *                      combination of the following values:
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_NONE
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_MCMP1
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_MCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_MCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_MCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_MPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_EEV1
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_EEV2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_EEV3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_EEV4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_EEV5
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMACMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMACMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMAPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMARST
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMBCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMBCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMBPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMBRST
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMCCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMCCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMCPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMDCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMDCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMDPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMECMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMECMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMEPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMFCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMFCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMFCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMFPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC13_TIMFRST
 *
 *                      For ADC trigger 2 and ADC trigger 4 this parameter can be a
 *                      combination of the following values:
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_NONE
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_MCMP1
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_MCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_MCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_MCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_MPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_EEV6
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_EEV7
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_EEV8
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_EEV9
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_EEV10
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMACMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMACMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMAPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMBCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMBCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMBPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMCCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMCCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMCPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMCRST
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMDCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMDCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMDPER
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMDRST
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMECMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMECMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMECMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMERST
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMFCMP2
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMFCMP3
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMFCMP4
 *            @arg @ref LL_HRTIM_ADCTRIG_SRC24_TIMFPER           
 */
void hrtim_adc_trigger_dis(hrtim_t hrtim, hrtim_adc_t adc, uint32_t evt);



int hrtim_get_apb2_clock();

#ifdef __cplusplus
}
#endif

#endif // HRTIM_VOLTAGE_MODE_H_
