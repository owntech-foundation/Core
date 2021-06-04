/*
 * Copyright (c) 2021 LAAS-CNRS
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
 * @author Clément Foucher <clement.foucher@laas.fr>
 */

#ifndef DATA_ACQUISITION_H_
#define DATA_ACQUISITION_H_

// Indirect public headers
#include "adc/adc_channels.h"
#include "data_dispatch/data_dispatch.h"
#include "dma/dma.h"

#ifdef __cplusplus
extern "C" {
#endif


// Current file public function
void data_acquisition_init();


#ifdef __cplusplus
}
#endif

#endif // DATA_ACQUISITION_H_
