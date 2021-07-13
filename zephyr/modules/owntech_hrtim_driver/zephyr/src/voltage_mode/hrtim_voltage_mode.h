/*
 * Copyright (c) 2020-2021 LAAS-CNRS
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
 */

#ifndef HRTIM_VOLTAGE_MODE_H_
#define HRTIM_VOLTAGE_MODE_H_

#include <stdint.h>
#include <limits.h>

#include <pinmux/stm32/pinmux_stm32.h>

#include <hrtim.h>

#ifdef __cplusplus
extern "C" {
#endif


#define DT_DRV_COMPAT hrtim
#define CLOCK_APB2 (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC / CONFIG_CLOCK_STM32_APB2_PRESCALER)
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
 * @brief   HRTIM Set/Reset trigger definition
 */
typedef enum {
    SOFT_TRIG = 0x00000001,     /*< Software Set/Reset trigger */
    RESYNC = 0x00000002,        /*< Timer x resynchronization */
    PER = 0x00000004,           /*< Timer x Period */
    CMP1 = 0x00000008,          /*< Timer x Compare 1 */
    CMP2 = 0x00000010,          /*< Timer x Compare 2 */
    CMP3 = 0x00000020,          /*< Timer x Compare 3 */
    CMP4 = 0x00000040,          /*< Timer x Compare 4 */
    MSTPER = 0x00000080,        /*< Master Period */
    MSTCMP1 = 0x00000100,       /*< Master Compare 1 */
    MSTCMP2 = 0x00000200,       /*< Master Compare 2 */
    MSTCMP3 = 0x00000400,       /*< Master Compare 3 */
    MSTCMP4 = 0x00000800,       /*< Master Compare 4 */
    TIMEVNT1 = 0x00001000,      /*< Timer Event 1 */
    TIMEVNT2 = 0x00002000,      /*< Timer Event 2 */
    TIMEVNT3 = 0x00004000,      /*< Timer Event 3 */
    TIMEVNT4 = 0x00008000,      /*< Timer Event 4 */
    TIMEVNT5 = 0x00010000,      /*< Timer Event 5 */
    TIMEVNT6 = 0x00020000,      /*< Timer Event 6 */
    TIMEVNT7 = 0x00040000,      /*< Timer Event 7 */
    TIMEVNT8 = 0x00080000,      /*< Timer Event 8 */
    TIMEVNT9 = 0x00100000,      /*< Timer Event 9 */
    EXTEVNT1 = 0x00200000,      /*< External Event 1 */
    EXTEVNT2 = 0x00400000,      /*< External Event 2 */
    EXTEVNT3 = 0x00800000,      /*< External Event 3 */
    EXTEVNT4 = 0x01000000,      /*< External Event 4 */
    EXTEVNT5 = 0x02000000,      /*< External Event 5 */
    EXTEVNT6 = 0x04000000,      /*< External Event 6 */
    EXTEVNT7 = 0x08000000,      /*< External Event 7 */
    EXTEVNT8 = 0x10000000,      /*< External Event 8 */
    EXTEVNT9 = 0x20000000,      /*< External Event 9 */
    EXTEVNT10 = 0x40000000,     /*< External Event 10 */
    UPDATE = 0x80000000         /*< Registers update (transfer preload
                                    to active) */
} hrtim_cb_t;

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

/**
 * @brief   HRTIM outputs definition
 */
typedef enum {
    OUT1 = 1,
    OUT2 = 2
} hrtim_out_t;

/**
 * @brief   HRTIM Timerx reset event definition
 *
 * Note: Bit definitions in stm32xxxx.h are for RSTAR (Timer A)
 * where Bits 19:30 are reset signals come from TIMB, TIMC,
 * TIMD, TIME so they are not usable for another unit.
 */
typedef enum {
    RST_UPDT = 0x00000002,          /*< Timer x Update reset */
    RST_CMP2 = 0x00000004,          /*< Timer x compare 2 reset */
    RST_CMP4 = 0x00000008,          /*< Timer x compare 4 reset */
    RST_MSTPER = 0x00000010,        /*< Master timer Period */
    RST_MSTCMP1 = 0x00000020,       /*< Master Compare 1 */
    RST_MSTCMP2 = 0x00000040,       /*< Master Compare 2 */
    RST_MSTCMP3 = 0x00000080,       /*< Master Compare 3 */
    RST_MSTCMP4 = 0x00000100,       /*< Master Compare 4 */
    RST_EXTEVNT1 = 0x00000200,      /*< External Event 1 */
    RST_EXTEVNT2 = 0x00000400,      /*< External Event 2 */
    RST_EXTEVNT3 = 0x00000800,      /*< External Event 3 */
    RST_EXTEVNT4 = 0x00001000,      /*< External Event 4 */
    RST_EXTEVNT5 = 0x00002000,      /*< External Event 5 */
    RST_EXTEVNT6 = 0x00004000,      /*< External Event 6 */
    RST_EXTEVNT7 = 0x00008000,      /*< External Event 7 */
    RST_EXTEVNT8 = 0x00010000,      /*< External Event 8 */
    RST_EXTEVNT9 = 0x00020000,      /*< External Event 9 */
    RST_EXTEVNT10 = 0x00040000,     /*< External Event 10 */
    RSTA_TBCMP1 = 0x00080000,       /*< Timer B Compare 1 for TIMA */
    RSTA_TBCMP2 = 0x00100000,       /*< Timer B Compare 2 for TIMA */
    RSTA_TBCMP4 = 0x00200000,       /*< Timer B Compare 4 for TIMA */
    RSTA_TCCMP1 = 0x00400000,       /*< Timer C Compare 1 for TIMA */
    RSTA_TCCMP2 = 0x00800000,       /*< Timer C Compare 2 for TIMA */
    RSTA_TCCMP4 = 0x01000000,       /*< Timer C Compare 4 for TIMA */
    RSTA_TDCMP1 = 0x02000000,       /*< Timer D Compare 1 for TIMA */
    RSTA_TDCMP2 = 0x04000000,       /*< Timer D Compare 2 for TIMA */
    RSTA_TDCMP4 = 0x08000000,       /*< Timer D Compare 4 for TIMA */
    RSTA_TECMP1 = 0x10000000,       /*< Timer E Compare 1 for TIMA */
    RSTA_TECMP2 = 0x20000000,       /*< Timer E Compare 2 for TIMA */
    RSTA_TECMP4 = 0x40000000,       /*< Timer E Compare 4 for TIMA */
    RSTB_TACMP1 = 0x00080000,       /*< Timer A Compare 1 for TIMB */
    RSTB_TACMP2 = 0x00100000,       /*< Timer A Compare 2 for TIMB */
    RSTB_TACMP4 = 0x00200000,       /*< Timer A Compare 4 for TIMB */
    RSTB_TCCMP1 = 0x00400000,       /*< Timer C Compare 1 for TIMB */
    RSTB_TCCMP2 = 0x00800000,       /*< Timer C Compare 2 for TIMB */
    RSTB_TCCMP4 = 0x01000000,       /*< Timer C Compare 4 for TIMB */
    RSTB_TDCMP1 = 0x02000000,       /*< Timer D Compare 1 for TIMB */
    RSTB_TDCMP2 = 0x04000000,       /*< Timer D Compare 2 for TIMB */
    RSTB_TDCMP4 = 0x08000000,       /*< Timer D Compare 4 for TIMB */
    RSTB_TECMP1 = 0x10000000,       /*< Timer E Compare 1 for TIMB */
    RSTB_TECMP2 = 0x20000000,       /*< Timer E Compare 2 for TIMB */
    RSTB_TECMP4 = 0x40000000,       /*< Timer E Compare 4 for TIMB */
    RSTC_TACMP1 = 0x00080000,       /*< Timer A Compare 1 for TIMC */
    RSTC_TACMP2 = 0x00100000,       /*< Timer A Compare 2 for TIMC */
    RSTC_TACMP4 = 0x00200000,       /*< Timer A Compare 4 for TIMC */
    RSTC_TBCMP1 = 0x00400000,       /*< Timer B Compare 1 for TIMC */
    RSTC_TBCMP2 = 0x00800000,       /*< Timer B Compare 2 for TIMC */
    RSTC_TBCMP4 = 0x01000000,       /*< Timer B Compare 4 for TIMC */
    RSTC_TDCMP1 = 0x02000000,       /*< Timer D Compare 1 for TIMC */
    RSTC_TDCMP2 = 0x04000000,       /*< Timer D Compare 2 for TIMC */
    RSTC_TDCMP4 = 0x08000000,       /*< Timer D Compare 4 for TIMC */
    RSTC_TECMP1 = 0x10000000,       /*< Timer E Compare 1 for TIMC */
    RSTC_TECMP2 = 0x20000000,       /*< Timer E Compare 2 for TIMC */
    RSTC_TECMP4 = 0x40000000,       /*< Timer E Compare 4 for TIMC */
    RSTD_TACMP1 = 0x00080000,       /*< Timer A Compare 1 for TIMD */
    RSTD_TACMP2 = 0x00100000,       /*< Timer A Compare 2 for TIMD */
    RSTD_TACMP4 = 0x00200000,       /*< Timer A Compare 4 for TIMD */
    RSTD_TBCMP1 = 0x00400000,       /*< Timer B Compare 1 for TIMD */
    RSTD_TBCMP2 = 0x00800000,       /*< Timer B Compare 2 for TIMD */
    RSTD_TBCMP4 = 0x01000000,       /*< Timer B Compare 4 for TIMD */
    RSTD_TCCMP1 = 0x02000000,       /*< Timer C Compare 1 for TIMD */
    RSTD_TCCMP2 = 0x04000000,       /*< Timer C Compare 2 for TIMD */
    RSTD_TCCMP4 = 0x08000000,       /*< Timer C Compare 4 for TIMD */
    RSTD_TECMP1 = 0x10000000,       /*< Timer E Compare 1 for TIMD */
    RSTD_TECMP2 = 0x20000000,       /*< Timer E Compare 2 for TIMD */
    RSTD_TECMP4 = 0x40000000,       /*< Timer E Compare 4 for TIMD */
    RSTE_TACMP1 = 0x00080000,       /*< Timer A Compare 1 for TIME */
    RSTE_TACMP2 = 0x00100000,       /*< Timer A Compare 2 for TIME */
    RSTE_TACMP4 = 0x00200000,       /*< Timer A Compare 4 for TIME */
    RSTE_TBCMP1 = 0x00400000,       /*< Timer B Compare 1 for TIME */
    RSTE_TBCMP2 = 0x00800000,       /*< Timer B Compare 2 for TIME */
    RSTE_TBCMP4 = 0x01000000,       /*< Timer B Compare 4 for TIME */
    RSTE_TCCMP1 = 0x02000000,       /*< Timer C Compare 1 for TIME */
    RSTE_TCCMP2 = 0x04000000,       /*< Timer C Compare 2 for TIME */
    RSTE_TCCMP4 = 0x08000000,       /*< Timer C Compare 4 for TIME */
    RSTE_TDCMP1 = 0x10000000,       /*< Timer D Compare 1 for TIME */
    RSTE_TDCMP2 = 0x20000000,       /*< Timer D Compare 2 for TIME */
    RSTE_TDCMP4 = 0x40000000,       /*< Timer D Compare 4 for TIME */
#if (HRTIM_STU_NUMOF == 6)
    RSTA_TFCMP2 = 0x80000000,       /*< Timer F Compare 2 for TIMA */
    RSTB_TFCMP2 = 0x80000000,       /*< Timer F Compare 2 for TIMB */
    RSTC_TFCMP2 = 0x80000000,       /*< Timer F Compare 2 for TIMC */
    RSTD_TFCMP2 = 0x80000000,       /*< Timer F Compare 2 for TIMD */
    RSTE_TFCMP2 = 0x80000000,       /*< Timer F Compare 2 for TIME */
    RSTF_TACMP1 = 0x00080000,       /*< Timer A Compare 1 for TIMF */
    RSTF_TACMP2 = 0x00100000,       /*< Timer A Compare 2 for TIMF */
    RSTF_TACMP4 = 0x00200000,       /*< Timer A Compare 4 for TIMF */
    RSTF_TBCMP1 = 0x00400000,       /*< Timer B Compare 1 for TIMF */
    RSTF_TBCMP2 = 0x00800000,       /*< Timer B Compare 2 for TIMF */
    RSTF_TBCMP4 = 0x01000000,       /*< Timer B Compare 4 for TIMF */
    RSTF_TCCMP1 = 0x02000000,       /*< Timer C Compare 1 for TIMF */
    RSTF_TCCMP2 = 0x04000000,       /*< Timer C Compare 2 for TIMF */
    RSTF_TCCMP4 = 0x08000000,       /*< Timer C Compare 4 for TIMF */
    RSTF_TDCMP1 = 0x10000000,       /*< Timer D Compare 1 for TIMF */
    RSTF_TDCMP2 = 0x20000000,       /*< Timer D Compare 2 for TIMF */
    RSTF_TDCMP4 = 0x40000000,       /*< Timer D Compare 4 for TIMF */
    RSTF_TECMP2 = 0x80000000,       /*< Timer E Compare 2 for TIMF */
#endif
} hrtim_rst_evt_t;

/**
 * @brief   HRTIM timing units CEN bits
 */
typedef enum {
    MCEN = HRTIM_MCR_MCEN,
    TACEN = HRTIM_MCR_TACEN,
    TBCEN = HRTIM_MCR_TBCEN,
    TCCEN = HRTIM_MCR_TCCEN,
    TDCEN = HRTIM_MCR_TDCEN,
    TECEN = HRTIM_MCR_TECEN,
#if (HRTIM_STU_NUMOF == 6)
    TFCEN = HRTIM_MCR_TFCEN
#endif
} hrtim_cen_t;

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
 * @brief   HRTIM ADC trigger register bits definitions
 */
typedef enum {
    AD13_MC1 = 0x00000001,          /*< ADC trigger on master compare 1 */
    AD13_MC2 = 0x00000002,          /*< ADC trigger on master compare 2 */
    AD13_MC3 = 0x00000004,          /*< ADC trigger on master compare 3 */
    AD13_MC4 = 0x00000008,          /*< ADC trigger on master compare 4 */
    AD13_MPER = 0x00000010,         /*< ADC trigger on master period */
    AD13_EEV1 = 0x00000020,         /*< ADC trigger on external event 1 */
    AD13_EEV2 = 0x00000040,         /*< ADC trigger on external event 2 */
    AD13_EEV3 = 0x00000080,         /*< ADC trigger on external event 3 */
    AD13_EEV4 = 0x00000100,         /*< ADC trigger on external event 4 */
    AD13_EEV5 = 0x00000200,         /*< ADC trigger on external event 5 */
    AD13_TFC2 = 0x00000400,         /*< ADC trigger on timer F compare 2 */
    AD13_TAC3 = 0x00000800,         /*< ADC trigger on timer A compare 3 */
    AD13_TAC4 = 0x00001000,         /*< ADC trigger on timer A compare 4 */
    AD13_TAPER = 0x00002000,        /*< ADC trigger on timer A period */
    AD13_TARST = 0x00004000,        /*< ADC trigger on timer A reset */
    AD13_TFC3 = 0x00008000,         /*< ADC trigger on timer F compare 3 */
    AD13_TBC3 = 0x00010000,         /*< ADC trigger on timer B compare 3 */
    AD13_TBC4 = 0x00020000,         /*< ADC trigger on timer B compare 4 */
    AD13_TBPER = 0x00040000,        /*< ADC trigger on timer B period */
    AD13_TBRST = 0x00080000,        /*< ADC trigger on timer B reset */
    AD13_TFC4 = 0x00100000,         /*< ADC trigger on timer F compare 4 */
    AD13_TCC3 = 0x00200000,         /*< ADC trigger on timer C compare 3 */
    AD13_TCC4 = 0x00400000,         /*< ADC trigger on timer C compare 4 */
    AD13_TCPER = 0x00800000,        /*< ADC trigger on timer C period */
    AD13_TFPER = 0x01000000,        /*< ADC trigger on timer F period */
    AD13_TDC3 = 0x02000000,         /*< ADC trigger on timer D compare 3 */
    AD13_TDC4 = 0x04000000,         /*< ADC trigger on timer D compare 4 */
    AD13_TDPER = 0x08000000,        /*< ADC trigger on timer D period */
    AD13_TFRST = 0x10000000,        /*< ADC trigger on timer F reset */
    AD13_TEC3 = 0x20000000,         /*< ADC trigger on timer E compare 3 */
    AD13_TEC4 = 0x40000000,         /*< ADC trigger on timer E compare 4 */
    AD13_TEPER = 0x80000000,        /*< ADC trigger on timer E period */

    AD24_MC1 = 0x00000001,          /*< ADC trigger on master compare 1 */
    AD24_MC2 = 0x00000002,          /*< ADC trigger on master compare 2 */
    AD24_MC3 = 0x00000004,          /*< ADC trigger on master compare 3 */
    AD24_MC4 = 0x00000008,          /*< ADC trigger on master compare 4 */
    AD24_MPER = 0x00000010,         /*< ADC trigger on master period */
    AD24_EEV6 = 0x00000020,         /*< ADC trigger on external event 6 */
    AD24_EEV7 = 0x00000040,         /*< ADC trigger on external event 7 */
    AD24_EEV8 = 0x00000080,         /*< ADC trigger on external event 8 */
    AD24_EEV9 = 0x00000100,         /*< ADC trigger on external event 9 */
    AD24_EEV10 = 0x00000200,        /*< ADC trigger on external event 10 */
    AD24_TAC2 = 0x00000400,         /*< ADC trigger on timer A compare 2 */
    AD24_TFC2 = 0x00000800,         /*< ADC trigger on timer F compare 2 */
    AD24_TAC4 = 0x00001000,         /*< ADC trigger on timer A compare 4 */
    AD24_TAPER = 0x00002000,        /*< ADC trigger on timer A period */
    AD24_TBC2 = 0x00004000,         /*< ADC trigger on timer B compare 2 */
    AD24_TFC3 = 0x00008000,         /*< ADC trigger on timer F compare 3 */
    AD24_TBC4 = 0x00010000,         /*< ADC trigger on timer B compare 4 */
    AD24_TBPER = 0x00020000,        /*< ADC trigger on timer B period */
    AD24_TCC2 = 0x00040000,         /*< ADC trigger on timer C compare 2 */
    AD24_TFC4 = 0x00080000,         /*< ADC trigger on timer F compare 4 */
    AD24_TCC4 = 0x00100000,         /*< ADC trigger on timer C compare 4 */
    AD24_TCPER = 0x00200000,        /*< ADC trigger on timer C period */
    AD24_TCRST = 0x00400000,        /*< ADC trigger on timer C reset */
    AD24_TDC2 = 0x00800000,         /*< ADC trigger on timer D compare 2 */
    AD24_TFPER = 0x01000000,        /*< ADC trigger on timer F period */
    AD24_TDC4 = 0x02000000,         /*< ADC trigger on timer D compare 4 */
    AD24_TDPER = 0x04000000,        /*< ADC trigger on timer D period */
    AD24_TDRST = 0x08000000,        /*< ADC trigger on timer D reset */
    AD24_TEC2 = 0x10000000,         /*< ADC trigger on timer E compare 2 */
    AD24_TEC3 = 0x20000000,         /*< ADC trigger on timer E compare 3 */
    AD24_TEC4 = 0x40000000,         /*< ADC trigger on timer E compare 4 */
    AD24_TERST = 0x80000000,        /*< ADC trigger on timer E reset */
} hrtim_adc_trigger_t;

/**
 * @brief   Initialize an HRTIM device and all these timing units for
 *          complementary pwm outputs with a dead time.
 *
 * @param[in] dev                   HRTIM device to initialize
 * @param[inout] freq               HRTIM frequency in Hz
 * @param[in] dt                    Desired dead time in ns
 * @param[in] switch_convention     Choice of the switch convention
 *
 * @return                  actual HRTIM resolution on success
 * @return                  0 on error
 */
uint16_t hrtim_init(hrtim_t dev, uint32_t *freq, uint16_t dt, uint8_t switch_convention);

/**
 * @brief   Initialize an HRTIM device master timer
 *
 * @param[in] dev           HRTIM device to initialize
 * @param[inout] freq       HRTIM frequency in Hz
 *
 * @return                  actual HRTIM resolution on success
 * @return                  0 on error
 */
uint16_t hrtim_init_master(hrtim_t dev, uint32_t *freq);

/**
 * @brief   Initialize a timing unit
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit to initialize
 * @param[inout] freq       HRTIM frequency in Hz
 *
 * @return                  actual timing unit resolution on success
 * @return                  0 on error
 */
uint16_t hrtim_init_tu(hrtim_t dev, hrtim_tu_t tu, uint32_t *freq);

/**
 * @brief   Set crossbar(s) setting
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output 1 or 2
 * @param[in] cb            Set crossbar(s)
 */
void hrtim_set_cb_set(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out,
                        hrtim_cb_t cb);

/**
 * @brief   Unset set crossbar(s)
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output 1 or 2
 * @param[in] cb            Set crossbar(s)
 */
void hrtim_set_cb_unset(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out,
                        hrtim_cb_t cb);

/**
 * @brief   Reset crossbar(s) setting
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output 1 or 2
 * @param[in] cb            Reset crossbar(s)
 */
void hrtim_rst_cb_set(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out,
                        hrtim_cb_t cb);

/**
 * @brief   Unset reset crossbar(s)
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output 1 or 2
 * @param[in] cb            Reset crossbar(s)
 */
void hrtim_rst_cb_unset(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out,
                            hrtim_cb_t cb);

/**
 * @brief   Full timing unit outputs set/reset crossbars setting for
 *          complementary pwm.
 *
 * @param[in] dev                   HRTIM device
 * @param[in] tu                    Timing unit
 * @param[in] switch_convention     Choice of the switch convention
 */
void hrtim_cmpl_pwm_out(hrtim_t dev, hrtim_tu_t tu, bool switch_convention);

/**
 * @brief   Set a period value
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit (TIM{A..F} or MSTR for master)
 * @param[in] value         Raw value to set
 */
void hrtim_period_set(hrtim_t dev, hrtim_tu_t tu, uint16_t value);

/**
 * @brief   Set a comparator value
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit (TIM{A..F} or MSTR for master)
 * @param[in] cmp           Comparator from 1 to 4
 * @param[in] value         Raw value to set
 */
void hrtim_cmp_set(hrtim_t dev, hrtim_tu_t tu, hrtim_cmp_t cmp,
                    uint16_t value);

/**
 * @brief   Enable a timing unit counter.
 *
 * @param[in] dev           HRTIM device
 * @param[in] cen           CEN mask
 */
void hrtim_cnt_en(hrtim_t dev, hrtim_cen_t cen);

/**
 * @brief   Disable a timing unit counter.
 *
 * @param[in] dev           HRTIM device
 * @param[in] cen           CEN mask
 */
void hrtim_cnt_dis(hrtim_t dev, hrtim_cen_t cen);

/**
 * @brief   Enable a Timerx reset event
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] evt           Reset event
 */
void hrtim_rst_evt_en(hrtim_t dev, hrtim_tu_t tu, hrtim_rst_evt_t evt);

/**
 * @brief   Disbable a Timerx reset event
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] evt           Reset event
 */
void hrtim_rst_evt_dis(hrtim_t dev, hrtim_tu_t tu, hrtim_rst_evt_t evt);

/**
 * @brief   Enable a given output of a given timing unit.
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output to enable
 */
void hrtim_out_en(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out);

/**
 * @brief   Disable a given output of a given timing unit.
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output to disable
 */
void hrtim_out_dis(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out);

/**
 * @brief   Setup a dead time in nano second for given complementary
 *          outputs.
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] ns            The desired dead time in nano second
 */
void hrtim_pwm_dt(hrtim_t dev, hrtim_tu_t tu, uint16_t ns);

/**
 * @brief   Enable an ADCx trigger event
 *
 * @param[in] dev           HRTIM device
 * @param[in] adc           ADC trigger register from ADC1R to ADC4R
 * @param[in] evt           Trigger event
 */
void hrtim_adc_trigger_en(hrtim_t dev, hrtim_adc_t adc, hrtim_adc_trigger_t evt);

/**
 * @brief   Disbable a ADCx trigger event
 *
 * @param[in] dev           HRTIM device
 * @param[in] adc           ADC trigger register from ADC1R to ADC4R
 * @param[in] evt           Trigger event
 */
void hrtim_adc_trigger_dis(hrtim_t dev, hrtim_adc_t adc, hrtim_adc_trigger_t evt);

#ifdef __cplusplus
}
#endif

#endif // HRTIM_VOLTAGE_MODE_H_
