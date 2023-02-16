/*
 * Copyright (c) 2023 LAAS-CNRS
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
 *
 * Header to give access to scheduling internal API
 * to other OwnTech modules.
 *
 * Only for use in OwnTech modules.
 */

#ifndef SCHEDULING_INTERNAL_H_
#define SCHEDULING_INTERNAL_H_


#include <stdint.h>

#include "Scheduling.h"


/**
 * @brief Get the period of the uninterruptible task in µs.
 *
 * For internal use only, do not call in user code.
 */
uint32_t scheduling_get_uninterruptible_synchronous_task_period_us();

/**
 * @brief Obtain the configured interrupt source for
 * uninterruptible synchronous task.
 *
 * For internal use only, do not call in user code.
 */
scheduling_interrupt_source_t scheduling_get_uninterruptible_synchronous_task_interrupt_source();

/**
 * @brief Set uninterruptible task in charge of data dispatch.
 *
 * For internal use only, do not call in user code.
 */
void scheduling_set_data_dispatch_at_task_start(bool enable);


#endif // SCHEDULING_INTERNAL_H_
