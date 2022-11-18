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


typedef void (*task_function_t)();

/////
// Static class definition

class Scheduling
{
public:
	/**
	 * @brief Uninterruptible synchronous task uses a timer to
	 * execute a periodic, non-interruptible user task.
	 * Use this function to define such a task.
	 * Only one task of this kind can be defined.
	 *
	 * @param periodic_task Pointer to the void(void) function
	 *        to be executed periodically.
	 * @param task_period_us Period of the function in µs.
	 *        Allowed range: 1 to 6553 µs.
	 *        Value is ignored if first parameter is NULL.
	 * @return 0 if everything went well,
	 * -1 if there was an error defining the task.
	 * An error can occur notably when an uninterruptible
	 * task has already been defined previously.
	 */
	int8_t defineUninterruptibleSynchronousTask(void (*periodic_task)(), uint32_t task_period_us);

	/**
	 * @brief Use this function to start the previously defined
	 * uninterruptible synchronous task.
	*/
	void startUninterruptibleSynchronousTask();


#ifdef CONFIG_OWNTECH_SCHEDULING_ENABLE_ASYNCHRONOUS_TASKS

	/**
	 * @brief Define an asynchronous task.
	 * Asynchronous tasks are run in background when there
	 * is no synchronous task running.
	 *
	 * @param routine Pointer to the void(void) function
	 *        that will act as the task main function.
	 * @return Number assigned to the task. Will be -1
	 * if max number of asynchronous task has been reached.
	 * In such a case, the task definition is cancelled.
	 * Increase maximum number of asynchronous tasks in
	 * prj.conf if required.
	 */
	int8_t defineAsynchronousTask(task_function_t routine);

	/**
	 * @brief Use this function to start a previously defined
	 * asynchronous task using its task number.
	 */
	void startAsynchronousTask(uint8_t task_number);

#endif // CONFIG_OWNTECH_SCHEDULING_ENABLE_ASYNCHRONOUS_TASKS

	/**
	 * @brief This function allows to suspend an asynchronous
	 * task for a specified duration expressed in milliseconds.
	 * For example, you can call this function at the end of an
	 * asynchronous task main function, when there is no need
	 * for the task to run permanently.
	 *
	 * DO NOT use this function in a synchronous task!
	 */
	void suspendCurrentTaskMs(uint32_t duration_ms);

	/**
	 * @brief This function allows to suspend an asynchronous
	 * task for a specified duration expressed in microseconds.
	 * For example, you can call this function at the end of an
	 * asynchronous task main function, when there is no need
	 * for the task to run permanently.
	 *
	 * DO NOT use this function in a synchronous task!
	 */
	void suspendCurrentTaskUs(uint32_t duration_us);

private:
	static const int DEFAULT_PRIORITY;

};


/////
// Public object to interact with the class

extern Scheduling scheduling;


#endif // SCHEDULING_H_
