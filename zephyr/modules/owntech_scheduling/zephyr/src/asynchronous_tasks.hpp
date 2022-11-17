/*
 * Copyright (c) 2022 LAAS-CNRS
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
 * @date   2022
 * @author Clément Foucher <clement.foucher@laas.fr>
 */


#ifndef ASYNCHRONOUSTASKS_HPP_
#define ASYNCHRONOUSTASKS_HPP_

#ifdef CONFIG_OWNTECH_SCHEDULING_ENABLE_ASYNCHRONOUS_TASKS


// OwnTech Power API
#include "Scheduling.h"


int8_t scheduling_define_asynchronous_task(task_function_t routine);
void scheduling_start_asynchronous_task(uint8_t task_number);


#endif // CONFIG_OWNTECH_SCHEDULING_ENABLE_ASYNCHRONOUS_TASKS

#endif // ASYNCHRONOUSTASKS_HPP_
