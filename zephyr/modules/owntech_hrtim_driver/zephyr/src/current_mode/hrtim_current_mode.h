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
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef HRTIM_CURRENT_MODE_H_
#define HRTIM_CURRENT_MODE_H_

#ifdef __cplusplus
extern "C" {
#endif

void hrtim_cm_init();
void hrtim_cm_init_gpio();
void hrtim_cm_enable();

#ifdef __cplusplus
}
#endif

#endif // HRTIM_CURRENT_MODE_H_
