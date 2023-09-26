/*
 * Copyright (c) 2022-2023 LAAS-CNRS
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
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */


#ifndef HARDWARECONFIGURATION_H_
#define HARDWARECONFIGURATION_H_

// Stdlib
#include <stdint.h>

// ARM lib
#include <arm_math.h>

// OwnTech API
#include "adc.h"
#include "hrtim_enum.h"


/** Hardware version. See
 *  https://gitlab.laas.fr/owntech/1leg/-/wikis/Releases
 *  for the list and specificities of versions.
 */
typedef enum
{
	nucleo_G474RE,
	O2_v_0_9,
	O2_v_1_1_2,
	SPIN_v_0_1,
	SPIN_v_0_9,
	SPIN_v_1_0,
	TWIST_v_1_1_2
} hardware_version_t;

/** Switch leg operation type.
 */
typedef enum
{
	buck,
	boost
} leg_operation_t;

/** Inverter leg operation type.
 */
typedef enum
{
	unipolar,
	bipolar
} inverter_modulation_t;


/////
// Static class definition

class HardwareConfiguration
{
public:

	/////
	// Common

	/**
	 * @brief Sets the version of the underlying hardware.
	 *        Depending on the hardware version, some software
	 *        configuration, such as pinout, has to be tweaked.
	 * @param hardware_version Enum representing the hardware version.
	 */
	void setBoardVersion(hardware_version_t hardware_version);

	/////
	// DAC

	void initDacConstValue(uint8_t dac_number);
	void setDacConstValue(uint8_t dac_number, uint8_t channel, uint32_t const_value);
	void slopeCompensationDac3(float32_t peak_voltage, float32_t low_voltage);
	void slopeCompensationDac1(float32_t peak_voltage, float32_t low_voltage);

	/////
	// NGND

#ifdef CONFIG_SHIELD_TWIST
	void setNgndOn();
	void setNgndOff();
#endif

	/////
	// LED

	void setLedOn();
	void setLedOff();
	void setLedToggle();

	/////
	// Timer

	/**
	 * @brief Launches the timer4 which is adapted for reading an encoder.
	 */
	void startLoggingIncrementalEncoder();

	/**
	 * @brief Gets the encoder step value.
	 * @return An uint32 value of the counter which corresponds to the step of the system.
	 */
	uint32_t getIncrementalEncoderValue();

	//HRTIM configuration

	void pwmInit(hrtim_tu_number_t pwmX);
	void pwmStart(hrtim_tu_number_t pwmX);
	void pwmStop(hrtim_tu_number_t pwmX);
	void pwmStartSubUnit(hrtim_tu_number_t tu, hrtim_output_number_t output);
	void pwmStopSubUnit(hrtim_tu_number_t tu, hrtim_output_number_t output);
	void pwmSetModulation(hrtim_tu_number_t pwmX, hrtim_cnt_t modulation);
	void pwmSetSwitchConvention(hrtim_tu_number_t pwmX, hrtim_switch_convention_t convention);
	void pwmSetFrequency(uint32_t value);
	void pwmSetDeadTime(hrtim_tu_number_t pwmX, uint16_t rise_ns, uint16_t fall_ns);
	void pwmSetDutyCycle(hrtim_tu_number_t pwmX, float32_t duty_cycle);
	void pwmSetPhaseShift(hrtim_tu_number_t pwmX, int16_t shift);

	void pwmSetMode(hrtim_tu_number_t pwmX, hrtim_pwm_mode_t mode);
	hrtim_pwm_mode_t pwmGetMode(hrtim_tu_number_t pwmX);
	void pwmSetEev(hrtim_tu_number_t pwmX, hrtim_external_trigger_t eev);
	hrtim_external_trigger_t pwmGetEev(hrtim_tu_number_t pwmX);

	hrtim_cnt_t pwmGetModulation(hrtim_tu_number_t pwmX);
	hrtim_switch_convention_t pwmGetSwitchConvention(hrtim_tu_number_t pwmX);
	uint16_t pwmGetPeriod(hrtim_tu_number_t pwmX);
	void pwmSetAdcTriggerPostScaler(hrtim_tu_number_t pwmX, uint32_t ps_ratio);
	void pwmSetAdcEdgeTrigger(hrtim_tu_number_t pwmX, hrtim_adc_edgetrigger_t adc_edge_trigger);
	hrtim_adc_edgetrigger_t pwmGetAdcEdgeTrigger(hrtim_tu_number_t pwmX);
	void pwmSetAdcTrig(hrtim_tu_number_t pwmX, hrtim_adc_trigger_t adc_trig);
	hrtim_adc_trigger_t pwmGetAdcTrig(hrtim_tu_number_t pwmX, hrtim_adc_trigger_t adc_trig);
	void pwmAdcTriggerEnable(hrtim_tu_number_t tu_number);
	void pwmAdcTriggerDisable(hrtim_tu_number_t tu_number);
	void pwmSetAdcTriggerInstant(hrtim_tu_number_t pwmX, float32_t trig_val);
	void pwmSetAdcDecimation(hrtim_tu_number_t pwmX,  uint32_t decimation);

	void pwmPeriodEvntDisable(hrtim_tu_t PWM_tu);
	void pwmSetPeriodEvntRep(hrtim_tu_t PWM_tu, uint32_t repetition);
	uint32_t pwmGetPeriodEvntRep(hrtim_tu_t PWM_tu);
	void pwmPeriodEvntConf(hrtim_tu_t PWM_tu, uint32_t repetition, hrtim_callback_t callback);
	void pwmPeriodEvntEnable(hrtim_tu_t PWM_tu);
	uint32_t pwmGetPeriodUs(hrtim_tu_number_t pwmX);

	/////
	// UART

	/**
	 * @brief Library initialization function for the usart communication.
	 *        This function is declared on the private section of the .c file
	 */
	void extraUartInit();

	/**
	 * @brief This function sends back a single character waiting to be treated from the usart1
	 *
	 * @return This function returns a single char which is waiting to be treated
	 *          if no char is waiting it returns an 'x' which should be treated as an error
	 */
	char extraUartReadChar();

	/**
	 * @brief This function transmits a single character through the usart 1
	 *
	 * @param data single char to be sent out
	 */
	void extraUartWriteChar(char data);

	/////
	// ADC

	/**
	 * @brief Change the trigger source of an ADC.
	 *        By default, triggger source for ADC 1/2 is on HRTIM1,
	 *        and ADC 3/4 is software-triggered.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param  adc_number Number of the ADC to configure
	 * @param  trigger_source Source of the trigger
	 */
	void adcConfigureTriggerSource(uint8_t adc_number, adc_ev_src_t trigger_source);

	/**
	 * @brief Set the discontinuous count for an ADC.
	 *        By default, ADCs are not in discontinuous mode.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param adc_number Number of the ADC to configure.
	 * @param discontinuous_count Number of channels to acquire on each
	 *        trigger event. 0 to disable discontinuous mode (default).
	 */
	void adcConfigureDiscontinuousMode(uint8_t adc_number, uint32_t dicontinuous_count);

	/**
	 * @brief ADC DMA mode configuration.
	 *        Enables DMA and circular mode on an ADC.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param adc_num Number of the ADC on which to enable DMA.
	 * @param use_dma Set to true to use DMA for this ADC,
	 *        false to not use it (default).
	 */
	void adcConfigureDma(uint8_t adc_number, bool use_dma);

	/**
	 * @brief Add a channel to the list of channels to be acquired
	 *        for an ADC.
	 *        The order in which channels are acquired is determined
	 *        by the order in which they are enabled.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param adc_number Number of the ADC to configure.
	 * @param channel Number of the channel to to be acquired.
	 */
	void adcEnableChannel(uint8_t adc_number, uint8_t channel);

	/**
	 * @brief Removes a channel from the list of channels that are
	 *        acquired by an ADC.
	 *
	 * @note  If a channel has been enabled multiple times, then only
	 *        the first occurence in the list will be removed.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param adc_number Number of the ADC to configure.
	 * @param channel Number of the channel to to no longer be acquired.
	 */
	void adcDisableChannel(uint8_t adc_number, uint8_t channel);

	/**
	 * @brief  Returns the number of enabled channels for an ADC.
	 *
	 * @param  adc_number Number of the ADC to fetch.
	 * @return Number of enabled channels on the given ADC.
	 */
	uint32_t adcGetEnabledChannelsCount(uint8_t adc_number);

	/**
	 * @brief  Triggers a conversion on an ADC which is
	 *         configured as software triggered.
	 *
	 * @note   Software trigger is default for all ADCs
	 *         unless configured differently by the user or
	 *         another module.
	 *
	 * @param  adc_number Number of the ADC to fetch.
	 * @param  number_of_acquisitions Number of channels to acquire.
	 */
	void adcTriggerSoftwareConversion(uint8_t adc_number, uint8_t number_of_acquisitions);

	/**
	 * @brief Start all configured ADCs.
	 */
	void adcStart();

	/**
	 * @brief Stop all configured ADCs.
	 */
	void adcStop();

	// DAC
	void dacConfigDac1CurrentmodeInit(hrtim_tu_t tu_src);
	void dacConfigDac3CurrentmodeInit(hrtim_tu_t tu_src);

	// Comparator
	void comparator1Initialize();
	void comparator3Initialize();

private:
	// LED
	void ledInitialize();

	// Timer
	void timerInitialize();

	// UART
	void uart1SwapRxTx();

	// ADC
	void adcInitialize();

private:
	// Common
	static hardware_version_t hardware_version;

	// LED
	static bool ledInitialized;

	// Timer
	static bool timer4init;
	static bool timer4started;

	// ADC
	static bool adcInitialized;

};


/////
// Public object to interact with the class

extern HardwareConfiguration hwConfig;


#endif // HARDWARECONFIGURATION_H_
