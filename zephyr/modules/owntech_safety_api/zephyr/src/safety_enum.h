/*
 * Copyright (c) 2024 LAAS-CNRS
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
 * @date 2024
 *
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 *
 * @brief This header contains enum used for safety function
 *
 */

#ifndef SAFETY_ENUM_H_
#define SAFETY_ENUM_H_

/**
 * two types of reactions :
 *  - Open_Circuit : Open both high-side and low-side switches
 *  - Short_Circuit : Open high-side switch and close low-side switch, maitaining output in shortcircuit.
 * */
typedef enum
{
    Open_Circuit,
    Short_Circuit,
} safety_reaction_t;

#endif // SAFETY_ENUM_H_

