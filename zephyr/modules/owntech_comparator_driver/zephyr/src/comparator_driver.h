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

/**
 * @date   2022
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

#ifndef COMPARATOR_DRIVER_H_
#define COMPARATOR_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize comparator COMP3 with predefined settings.
 *
 * This function configures GPIO and comparator settings for COMP3:
 * - Sets PC1 as the positive input (COMP3_INP) in analog mode.
 * - Routes DAC1 Channel 1 as the negative input.
 * - Configures non-inverting output, no hysteresis, and no blanking source.
 * - Disables related EXTI line (line 29) events and interrupts.
 * - Applies voltage scaler stabilization delay.
 * - Enables the comparator.
 */
void comparator_comp1_init();

/**
 * @brief Initialize comparator COMP3 with predefined settings.
 *
 * This function configures GPIO and comparator settings for COMP3:
 * - Sets PC1 as the positive input (COMP3_INP) in analog mode.
 * - Routes DAC1 Channel 1 as the negative input.
 * - Configures non-inverting output, no hysteresis, and no blanking source.
 * - Disables related EXTI line (line 29) events and interrupts.
 * - Applies voltage scaler stabilization delay.
 * - Enables the comparator.
 */
void comparator_comp3_init();

#ifdef __cplusplus
}
#endif

#endif /* COMPARATOR_DRIVER_H_ */
