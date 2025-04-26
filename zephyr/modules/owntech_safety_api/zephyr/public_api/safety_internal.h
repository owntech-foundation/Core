/*
 * Copyright (c) 2024-present LAAS-CNRS
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
 * @date 2024
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 *
 * @brief This header contains the function safety_task which will be used
 *        by the uninterruptible task.
 *
 * @warning Only for internal use.
 */

#ifndef SAFETY_INTERNAL_H_
#define SAFETY_INTERNAL_H_

#include "arm_math.h"

/**
 * @brief This function first watches the measure from the monitored channels,
 *    and then compares it with the threshold values max/min to detect faults.
 *        
 * If an error was detected, the switches will either in 
 * open-circuit mode or in short-circuit mode.
 *
 * @return `0` if no error was detected, `-1` else
*/
int8_t safety_task();

#endif /* SAFETY_INTERNAL_H_ */
