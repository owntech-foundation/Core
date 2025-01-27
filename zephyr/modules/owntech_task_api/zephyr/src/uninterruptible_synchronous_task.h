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

/**
 * @date   2023
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef UNINTERRUPTIBLESYNCHRONOUSTASK_H_
#define UNINTERRUPTIBLESYNCHRONOUSTASK_H_

/* Stdlib */
#include <stdint.h>

/* OwnTech Power API */
#include "TaskAPI.h"


void scheduling_set_uninterruptible_synchronous_task_interrupt_source(
                                    scheduling_interrupt_source_t int_source);

int8_t scheduling_define_uninterruptible_synchronous_task(
                                    task_function_t periodic_task,
                                    uint32_t task_period_us);

void scheduling_start_uninterruptible_synchronous_task(
                                    bool manage_data_acquisition);

void scheduling_stop_uninterruptible_synchronous_task();


#endif /* UNINTERRUPTIBLESYNCHRONOUSTASK_H_ */
