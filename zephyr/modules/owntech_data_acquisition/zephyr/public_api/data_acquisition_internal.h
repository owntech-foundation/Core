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

#ifndef DATA_ACQUISITION_INTERNAL_H_
#define DATA_ACQUISITION_INTERNAL_H_


/**
 * @brief Force full dispatch.
 *
 * For internal use only, do not call in user code.
 */
void data_dispatch_do_full_dispatch();


#endif // DATA_ACQUISITION_INTERNAL_H_
