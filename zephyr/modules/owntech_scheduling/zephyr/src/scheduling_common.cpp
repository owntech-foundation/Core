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


// OwnTech Power API
#include "scheduling_common.hpp"

void scheduling_common_start_task(task_information_t& task_info, k_thread_entry_t entry_point)
{
	k_tid_t tid = k_thread_create(&task_info.thread_data,
	                              task_info.stack,
	                              task_info.stack_size,
	                              entry_point,
	                              (void*)task_info.routine, NULL, NULL,
	                              task_info.priority,
	                              K_FP_REGS,
	                              K_NO_WAIT);

	task_info.thread_id = tid;
}

void scheduling_common_suspend_task(task_information_t& task_info)
{
	k_thread_suspend(task_info.thread_id);
}

void scheduling_common_resume_task(task_information_t& task_info)
{
	k_thread_resume(task_info.thread_id);
}
