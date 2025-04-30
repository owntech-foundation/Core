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

/*
 * @date   2023
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef UNINTERRUPTIBLESYNCHRONOUSTASK_H_
#define UNINTERRUPTIBLESYNCHRONOUSTASK_H_

/* Stdlib */
#include <stdint.h>

/* OwnTech Power API */
#include "TaskAPI.h"


/**
 * @brief Set the interrupt source used for the uninterruptible synchronous task.
 *
 * This determines whether the task will be triggered using a hardware timer
 * (e.g., TIM6) or HRTIM. Must be called before defining the task.
 *
 * @param int_source Interrupt source to use (e.g., source_tim6 or source_hrtim).
 */
void scheduling_set_uninterruptible_synchronous_task_interrupt_source(
                                    scheduling_interrupt_source_t int_source);

                                    /**
 * @brief Define a periodic task to be run in an uninterruptible synchronous 
 *        context.
 *
 * Registers a user-defined periodic task to be triggered by a hardware timer.
 * The period must be compatible with the selected interrupt source.
 * 
 * - For `TIM6`: configures a timer to trigger the task.
 * 
 * - For `HRTIM`: configures a periodic event tied to the HRTIM master period.
 *
 * @param periodic_task Pointer to the task function (must not be `NULL`).
 * @param task_period_us Task execution period in microseconds.
 *
 * @return `0` on success, 
 *         `-1` on failure (invalid period, task already defined, etc.).
 */
int8_t scheduling_define_uninterruptible_synchronous_task(
                                    task_function_t periodic_task,
                                    uint32_t task_period_us);

                                    /**
 * @brief Start the uninterruptible synchronous task.
 *
 * Enables the periodic task previously defined using the selected interrupt 
 * source. Optionally starts data acquisition if not already active and 
 * configures it for external triggering and synchronized dispatching.
 *
 * @param manage_data_acquisition Set to `true` if data acquisition should 
 *                              be managed automatically during task execution.
 */
void scheduling_start_uninterruptible_synchronous_task(
                                    bool manage_data_acquisition);

/**
 * @brief Stop the currently running uninterruptible synchronous task.
 *
 * Disables the interrupt source triggering the task (either `TIM6` or `HRTIM`)
 * and updates the task status accordingly.
 */
void scheduling_stop_uninterruptible_synchronous_task();


#endif /* UNINTERRUPTIBLESYNCHRONOUSTASK_H_ */
