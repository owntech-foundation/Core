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
 * @date   2023
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

// voltage_mode header
#include "../voltage_mode/hrtim_voltage_mode.h"

#ifndef HRTIM_CURRENT_MODE_H_
#define HRTIM_CURRENT_MODE_H_


#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize dual DAC reset and trigger. The selected timing unit CMP2
 *          will trigger the step (Decrement/Increment of sawtooth) and the reset 
 *          (return to initial value).  
 *
 * @param[in] tu  timing unit
 */
void CM_DualDAC_init(hrtim_tu_t tu);


/**
 * @brief   This function manage the set/reset crossbar of a timing unit, according to the leg 
 *          convention (buck or boost). The leg1 is linked to EEV4 (COMP1 output)
 *
 * @param[in] tu  timing unit
 * @param[in] leg_upper_switch_convention  1-->buck and 0-->boost
 * @param[in] cnt_mode left aligned or center aligned
 * 
 */
void CM_hrtim_pwm_leg1(hrtim_tu_t tu, bool leg_upper_switch_convention, hrtim_cnt_t cnt_mode);

/**
 * @brief   This function manage the set/reset crossbar of a timing unit, according to the leg 
 *          convention (buck or boost). The leg2 is linked to EEV5 (COMP3 output)
 *
 * @param[in] tu  timing unit
 * @param[in] leg_upper_switch_convention  1-->buck and 0-->boost
 * @param[in] cnt_mode left aligned or center aligned
 * 
 */
void CM_hrtim_pwm_leg2(hrtim_tu_t tu, bool leg_upper_switch_convention, hrtim_cnt_t cnt_mode);


/**
 * @brief   Initialize an HRTIM device and all these timing units for
 *          complementary pwm outputs with a dead time.
 *
 * @param[inout] freq  HRTIM frequency in Hz
 * @param[in] dt       Desired dead time in ns
 * @param[in] leg1_upper_switch_convention   Choice of the switch convention for leg 1, can be one of the following values:
 * @param[in] leg2_upper_switch_convention   Choice of the switch convention for leg 2, can be one of the following values:
 * @param[in] leg1_tu   Timer for leg 1
 * @param[in] leg2_tu   Timer for leg 2
 * @return              actual HRTIM resolution on success
 * @return              0 on error
 */
uint16_t CM_hrtim_init(uint32_t *freq, uint16_t dead_time_ns, uint8_t leg1_upper_switch_convention, uint8_t leg2_upper_switch_convention, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);

/**
 * @brief   Set the phase shift for a given
 *          timing unit
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] shift         Phase shifting
 */
void CM_hrtim_pwm_set(hrtim_tu_t tu, uint16_t shift);

#ifdef __cplusplus
}
#endif

#endif // HRTIM_CURRENT_MODE_H_
