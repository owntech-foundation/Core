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

#ifndef SCHEDULING_H_
#define SCHEDULING_H_


// Stdlib
#include <stdint.h>

// Zephyr
#include <zephyr.h>


typedef void (*thread_function_t)();

/////
// Static class definition

class Scheduling
{
private:
	static void threadEntryPoint(void* thread_function_p, void*, void*);

public:
	/**
	 * @brief This function uses Timer 6 to execute the periodic user task.
	 *        The task is immediately started.
	 *
	 * @param periodic_task Pointer to the void(void) function
	 *        to be executed periodically.
	 *        Can be NULL if no task has to be executed.
	 * @param task_period_us Period of the function in µs.
	 *        Allowed range: 1 to 6553 µs.
	 *        Value is ignored if first parameter is NULL.
	 */
	static void startControlTask(void (*periodic_task)(), uint32_t task_period_us);

	/**
	 * @brief Schedule the communication thread.
	 *        The task is immediately started.
	 *
	 * @param routine Pointer to the void(void) function
	 *        that will act as the thread main function.
	 * @param priority Priority of the thread. This
	 *        parameter can be omitted and will take
	 *        its default value.
	 */
	static void startCommunicationTask(thread_function_t routine, int priority = DEFAULT_PRIORITY);

	/**
	 * @brief Schedule the application thread.
	 *        The task is immediately started.
	 *
	 * @param routine Pointer to the void(void) function
	 *        that will act as the thread main function.
	 * @param priority Priority of the thread. This
	 *        parameter can be omitted and will take
	 *        its default value.
	 */
	static void startApplicationTask(thread_function_t routine, int priority = DEFAULT_PRIORITY);

private:
	static const struct device* timer6;
	static const int DEFAULT_PRIORITY;

	static k_tid_t communicationThreadTid;
	static k_tid_t applicationThreadTid;

	static k_thread communicationThreadData;
	static k_thread applicationThreadData;
};


/////
// Public object to interact with the class

extern Scheduling scheduling;


#endif // SCHEDULING_H_
