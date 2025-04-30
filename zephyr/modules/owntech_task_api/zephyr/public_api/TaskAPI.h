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

#ifndef TASKAPI_H_
#define TASKAPI_H_


/* Stdlib */
#include <stdint.h>

/* Zephyr */
#include <zephyr/kernel.h>

/**
 *  Public types
 */

typedef void (*task_function_t)();

typedef enum { source_uninitialized,
			   source_hrtim,
			   source_tim6 }
			   scheduling_interrupt_source_t;

/**
 *  Static class definition
 */

class TaskAPI
{
public:
	/**
	 * @brief Creates a time critical task.
	 * 
	 *        A critical task is an Uninterruptible Synchronous Task
	 *        that uses a precise timer to execute a periodic,
	 *        non-interruptable user task.
	 * 
	 *        Use this function to define such a task.
	 * 
	 *        Only one task of this kind can be defined.
	 * 
	 *        This function can be used to redefine (replace) a
	 *        previously defined uninterruptible synchronous task,
	 *        but the previously defined task must have been suspended
	 *        (or never started). An error will be returned if the
	 *        previously defined task is still running.
	 * 
	 * @note  If the `HRTIM` is used to trigger the task (which is the
	 *        default behavior), then the `HRTIM` must have been
	 *        configured *before* calling this function.
	 *
	 * @param periodic_task Pointer to the void(void) function
	 *        to be executed periodically.
	 * 
	 * @param task_period_us Period of the function in µs.
	 *        Allowed range: 1 to 6553 µs.
	 *        If interrupt source is `HRTIM`, this value *must* be an
	 *        integer multiple of the `HRTIM` period.
	 * 
	 * @param int_source Interrupt source that triggers the task.
	 *        By default, the `HRTIM` is the source, but this optional
	 *        parameter can be provided to set TIM6 as the source in
	 *        case the `HRTIM` is not used or if the task can't be
	 *        correlated to an `HRTIM` event.
	 *        Allowed values are source_hrtim and source_tim6.
	 * 
	 * @return `0` if everything went well,
	 *         `-1` if there was an error defining the task.
	 * 
	 *         An error can occur notably when an uninterruptible
	 *         task has already been defined previously.
	 */
	int8_t createCritical(
				task_function_t periodic_task,
				uint32_t task_period_us,
				scheduling_interrupt_source_t int_source = source_hrtim
			);

	/**
	 * @brief Use this function to start a previously defined
	 *        a critical task.
	 *
	 *        A critical task is an Uninterruptible Synchronous Task that uses
	 * 		  a precise timer to execute a periodic, non-interruptable user task.
	 *
	 *        If no value is provided
	 *        for the parameter and Data Acquisition has not been started
	 *        yet, Scheduling will automatically start Data Acquisition.
	 *        Thus, make sure all ADC configuration has been carried
	 *        out before starting the uninterruptible task.
	 *
	 * @param manage_data_acquisition Set to false if you want
	 *        the Scheduling module to not be in charge of Data
	 *        Acquisition scheduling. If set to false, Data Acquisition
	 *        has to be manually started if you want to use it.
	 */
	void startCritical(bool manage_data_acquisition = true);

	/**
	 * @brief Stop the previously started critical task.
	 *        A critical task is an Uninterruptible Synchronous Task
	 * 		  that uses a precise timer to execute a periodic, non-interruptable
	 * 		  user task. The task can be then resumed by calling
	 *        startCritical() again.
	 */
	void stopCritical();


#ifdef CONFIG_OWNTECH_TASK_ENABLE_ASYNCHRONOUS_TASKS

	/**
	 * @brief Creates a background task.
	 *        Background tasks are asynchronous tasks that run in the
	 * 		  background when there is no critical task running.
	 *
	 * @param routine Pointer to the void(void) function
	 *        that will act as the task main function.
	 * @return Number assigned to the task. Will be -1 if max
	 *         number of asynchronous task has been reached.
	 *         In such a case, the task definition is ignored.
	 *         Increase maximum number of asynchronous tasks
	 *         in prj.conf if required.
	 */
	int8_t createBackground(task_function_t routine);

	/**
	 * @brief Use this function to start a previously defined
	 *        background task using its task number.
	 *
	 *        Background tasks are asynchronous tasks that run in the background
	 * 		  when there is no critical task running.
	 *
	 * @param task_number Number of the task to start, obtained
	 *        using the defineAsynchronousTask() function.
	 */
	void startBackground(uint8_t task_number);

	/**
	 * @brief Use this function to stop a previously started
	 *        background task using its task number.
	 *
	 *        Background tasks are asynchronous tasks that run in the background
	 * 		  when there is no critical task running.
	 *        The task can be then resumed by calling
	 *        startAsynchronousTask() again.
	 *
	 * @param task_number Number of the task to start, obtained
	 *        using the defineAsynchronousTask() function.
	 */
	void stopBackground(uint8_t task_number);

	/**
	 * @brief This function allows to suspend a background task
	 *        for a specified duration expressed in milliseconds.
	 *        For example, you can call this function at the end of a
	 *        background task function, when there is no need
	 *        for the task to run permanently.
	 *
	 *        DO NOT use this function in a critical task!
	 */
	void suspendBackgroundMs(uint32_t duration_ms);

	/**
	 * @brief This function allows to suspend a background
	 *        task for a specified duration expressed in microseconds.
	 *        For example, you can call this function at the end of a
	 *        background task function, when there is no need
	 *        for the task to run permanently.
	 *
	 *        DO NOT use this function in a critical task!
	 */
	void suspendBackgroundUs(uint32_t duration_us);

#endif /* CONFIG_OWNTECH_TASK_ENABLE_ASYNCHRONOUS_TASKS */

private:
	static const int DEFAULT_PRIORITY;

};

/**
 *  Public object to interact with the class
 */

extern TaskAPI task;


#endif /* TASKAPI_H_ */
