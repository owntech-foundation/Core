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


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/////
// Error codes

#define ECHANNOTFOUND   -1
#define EUNITITIALIZED  -2
#define ESTARTED        -3
#define EALREADYSTARTED -4
#define EALREADYINIT    -5


/////
// Configuration API

/**
 * This function initializes the Data Acquisition module.
 * It must be called prior to any other function in this module.
 * @return 0 if initialization went well,
 *         EALREADYINIT is already initialized.
 */
int8_t data_acquisition_init();

/**
 * Use this function to set ADC 1 and ADC 2 in dual mode.
 *
 * This function must be called AFTER data_acquisition_init()
 * and BEFORE data_acquisition_start().
 *
 * @param  dual_mode Status of the dual mode: true to enable,
 *         false to disable. false by default.
 * @return 0 is everything went well,
 *         EUNITITIALIZED if the module has not been initialized,
 *         EALREADYSTARTED if the module has already been started.
 */
int8_t data_acquisition_set_adc12_dual_mode(uint8_t dual_mode);

/**
 * This function is used to configure the channels to be
 * enabled on a given ADC.
 *
 * This function must be called AFTER data_acquisition_init()
 * and BEFORE data_acquisition_start().
  *
 * @param  adc_number Number of the ADC on which channel configuration is
 *         to be done.
 * @param  channel_list List of channels to configure. This is a list of
 *         names as defined in the device tree (field `label`). The order
 *         of the names in the array sets the acquisition ranks (order in
 *         which the channels are acquired).
 * @param  channel_count Number of channels defined in `channel_list`.
 * @return 0 is everything went well,
 *         ECHANNOTFOUND if at least one of the channels
 *           is not available in the given ADC. Available channels are the
 *           ones defined in the device tree.
 *         EUNITITIALIZED if the module has not been initialized,
 *         EALREADYSTARTED if the module has already been started.
 */
int8_t data_acquisition_configure_adc_channels(uint8_t adc_number, char* channel_list[], uint8_t channel_count);

/**
 * This function is used to configure the trigger source of an ADC.
 *
 * This function must be called AFTER data_acquisition_init()
 * and BEFORE data_acquisition_start().
 *
 * TODO: Use an enumeration instead of LL constants for source.
 *
 * @param  adc_number Number of the ADC to configure
 * @param  trigger_source Source of the trigger as defined
 *         in stm32gxx_ll_adc.h (LL_ADC_REG_TRIG_***)
 * @return 0 is everything went well,
 *         EUNITITIALIZED if the module has not been initialized,
 *         EALREADYSTARTED if the module has already been started.
 */
int8_t data_acquisition_configure_adc_trigger_source(uint8_t adc_number, uint32_t trigger_source);

/**
 * This functions starts the acquisition chain. It must be called
 * after all module configuration has been carried out. No
 * configuration change is allowed after module has been started.
 *
 * @return 0 is everything went well,
 *         EUNITITIALIZED if the module has not been initialized,
 *         EALREADYSTARTED if the module has already been started.
 */
int8_t data_acquisition_start();


/////
// Accessor API

/**
 * This function returns the name of an enabled channel.
 *
 * This function must be called AFTER data_acquisition_init()
 * and AFTER data_acquisition_configure_adc_channels() function
 * has been called for this channel.
 *
 * @param  adc_number Number of the ADC
 * @param  channel_rank Rank of the ADC channel to query.
 *         Rank ranges from 0 to (number of enabled channels)-1
 * @return Name of the channel as defined in the device tree, or
 *         NULL if channel configuration has not been made or
 *         channel_rank is over (number of enabled channels)-1.
 */
char* data_acquisition_get_channel_name(uint8_t adc_number, uint8_t channel_rank);

/**
 * Function to access the acquired data for each channel.
 * Each function provides a buffer in which all data that
 * have been acquired since last call are stored. The count
 * of these values is returned as an output parameter: the
 * user has to define a variable and pass a pointer to this
 * variable as the parameter of the function. The variable
 * will be updated with the number of values that are available
 * in the buffer.
 *
 * NOTE 1: When calling one of these functions, it invalidates
 *         the buffer returned by a previous call to the same function.
 * NOTE 2: All function buffers are independent.
 *
 * @param  number_of_values_acquired Pass a pointer to an uint32_t variable.
 *         This variable will be updated with the number of values that
 *         have been acquired for this channel.
 * @return Pointer to a buffer in which the acquired values are stored.
 *         If number_of_values_acquired is 0, do not try to access the
 *         buffer as it may be NULL.
 */
uint16_t* data_acquisition_get_v1_low_values(uint32_t* number_of_values_acquired);
uint16_t* data_acquisition_get_v2_low_values(uint32_t* number_of_values_acquired);
uint16_t* data_acquisition_get_v_high_values(uint32_t* number_of_values_acquired);
uint16_t* data_acquisition_get_i1_low_values(uint32_t* number_of_values_acquired);
uint16_t* data_acquisition_get_i2_low_values(uint32_t* number_of_values_acquired);
uint16_t* data_acquisition_get_i_high_values(uint32_t* number_of_values_acquired);
uint16_t* data_acquisition_get_temp_sensor_values(uint32_t* number_of_values_acquired);


#ifdef __cplusplus
}
#endif

#endif // DATA_ACQUISITION_H_
