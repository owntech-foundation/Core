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
#include "hrtim.h"


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
	void slopeCompensationLeg1(float32_t peak_voltage, float32_t low_voltage);
	void slopeCompensationLeg2(float32_t peak_voltage, float32_t low_voltage);

	/////
	// NGND

	void setNgndOn();
	void setNgndOff();

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

	/////
	// Power converter

	void initInterleavedBuckMode();
	void initInterleavedBuckModeCenterAligned();
	void initInterleavedBoostMode();
	void initInterleavedBoostModeCenterAligned();
	void initFullBridgeBuckMode();
	void initFullBridgeBuckModeCenterAligned(inverter_modulation_t inverter_modulation_type);
	void initFullBridgeBoostMode();
	void initFullBridgeBoostModeCenterAligned();
	void initIndependentMode(leg_operation_t leg1_operation_type, leg_operation_t leg2_operation_type);
	void initIndependentModeCenterAligned(leg_operation_t leg1_operation_type, leg_operation_t leg2_operation_type);
	void initBuckCurrentMode();

	/**
	 * @brief     This function transfer the calculated PWM value to the
	 * HRTIM peripheral and make sure it is between saturation
	 * bounds with a phase shift compatible with the interleaved application.
	 *
	 * @param[in] duty_cycle    floating point duty cycle comprised between 0 and 1.
	 */
	void setInterleavedDutyCycle(float32_t duty_cycle);

	/**
	 * @brief     This function transfer the calculated PWM value to the
	 * HRTIM peripheral and make sure it is between saturation
	 * bounds with a complementary approach to the duty cycles compatible with the HBridge application.
	 *
	 * @param[in] duty_cycle    floating point duty cycle comprised between 0 and 1.
	 */
	void setFullBridgeBuckDutyCycle(float32_t duty_cycle);

	/**
	 * @brief     This function transfer the calculated PWM value of leg_1 to the
	 * HRTIM peripheral and make sure it is between saturation
	 * bounds.
	 *
	 * @param[in] duty_cycle    floating point duty cycle of leg_1 comprised between 0 and 1.
	 */
	void setLeg1DutyCycle(float32_t duty_cycle);

	/**
	 * @brief     This function transfer the calculated PWM value of leg_2 to the
	 * HRTIM peripheral and make sure it is between saturation
	 * bounds.
	 *
	 * @param[in] duty_cycle    floating point duty cycle of leg_2 comprised between 0 and 1.
	 */
	void setLeg2DutyCycle(float32_t duty_cycle);

	/**
	 * @brief     This function updates the phase shift between the leg 1 and the master hrtim
	 *
	 * @param[in] phase_shift    floating point phase shift of leg_1 in degrees
	 */
	void setLeg1PhaseShift(float32_t phase_shift);

	/**
	 * @brief     This function updates the phase shift between the leg 1 and the master hrtim
	 *
	 * @param[in] phase_shift    floating point phase shift of leg_2 in degrees
	 */
	void setLeg2PhaseShift(float32_t phase_shift);

	/**
	 * @brief     This function updates the phase shift between the leg 1 and the master hrtim for the center aligned
	 *
	 * @param[in] phase_shift    floating point phase shift of leg_1 in degrees
	 */
	void setLeg1PhaseShiftCenterAligned(float32_t phase_shift);

	/**
	 * @brief     This function updates the phase shift between the leg 1 and the master hrtim for the center aligned
	 *
	 * @param[in] phase_shift    floating point phase shift of leg_2 in degrees
	 */
	void setLeg2PhaseShiftCenterAligned(float32_t phase_shift);

	/**
	 * @brief This function sets the dead time of the leg 1
	 */
	void setLeg1DeadTime(uint16_t rise_ns, uint16_t fall_ns);

	/**
	 * @brief This function sets the dead time of the leg 2
	 */
	void setLeg2DeadTime(uint16_t rise_ns, uint16_t fall_ns);

	/**
	 * @brief Sets the frequency of the HRTIMER
	 */
	void setHrtimFrequency(uint32_t frequency_Hz);

	/**
	 * @brief Gets the frequency of the HRTIMER
	 */
	uint32_t getHrtimFrequency();

	/**
	 * @brief Updates the minimum duty cycle of both legs
	 */
	void setHrtimMinDutyCycle(float32_t duty_cycle);

	/**
	 * @brief This updates the minimum duty cycle of both legs
	 */
	void setHrtimMaxDutyCycle(float32_t duty_cycle);

	void setInterleavedOn();
	void setFullBridgeBuckOn();
	void setLeg1On();
	void setLeg2On();

	void setInterleavedOff();
	void setFullBridgeBuckOff();
	void setLeg1Off();
	void setLeg2Off();

	/**
	 * @brief     Updates the adc trigger moment
	 *
	 * @param[in] new_trig    defines the triggering moment
	 */
	void setHrtimAdcTrigInterleaved(float32_t new_trig);

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
	 *        By default, ADC 1/2 ard in discontinuous mode, while
	 *        ADC 3/4 aren't.
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
	 *        The order in which channels are added determine
	 *        the order in which they will be acquired.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param adc_number Number of the ADC to configure.
	 * @param channel Number of the channel to to be acquired.
	 */
	void adcAddChannel(uint8_t adc_number, uint8_t channel);

	/**
	 * @brief Removes a channel from the list of channels
	 *        that are acquired by an ADC.
	 *        If a channel has been added multiple times,
	 *        then only the first occurence in the list
	 *        will be removed.
	 *
	 *        Applied configuration will only be set when ADC is started.
	 *        If ADC is already started, it must be stopped then started again.
	 *
	 * @param adc_number Number of the ADC to configure.
	 * @param channel Number of the channel to to no longer be acquired.
	 */
	void adcRemoveChannel(uint8_t adc_number, uint8_t channel);

	/**
	 * @brief Start all configured ADCs.
	 */
	void adcStart();

	/**
	 * @brief Stop all configured ADCs.
	 */
	void adcStop();

private:
	// LED
	void ledInitialize();

	// Timer
	void timerInitialize();

	// UART
	void uart1SwapRxTx();

	// Power driver
	void powerDriverInitialize();
	void powerDriverLeg1On();
	void powerDriverLeg2On();
	void powerDriverLeg1Off();
	void powerDriverLeg2Off();
	void powerDriverInterleavedOn();
	void powerDriverInterleavedOff();

	// HRTIM
	void hrtimLegTu(hrtim_tu_t tu1, hrtim_tu_t tu2);
	void hrtimInitInterleavedBuckMode();
	void hrtimInitInterleavedBuckModeCenterAligned();
	void hrtimInitInterleavedBoostMode();
	void hrtimInitInterleavedBoostModeCenterAligned();
	void hrtimInitIndependentMode(bool leg1_buck_mode, bool leg2_buck_mode);
	void hrtimInitIndependentModeCenterAligned(bool leg1_buck_mode, bool leg2_buck_mode);
	void hrtimInitFullBridgeBuckMode(bool SPIN_board_V_1_1_2);
	void hrtimInitFullBridgeBuckModeCenterAligned(bool bipolar_mode,bool SPIN_board_V_1_1_2);
	void hrtimInitCurrentMode(bool leg1_buck, bool leg2_buck, hrtim_tu_t leg1_tu, hrtim_tu_t leg2_tu);
	void hrtimStopInterleaved();
	void hrtimStopFullBridgeBuck();
	void hrtimStopLeg1();
	void hrtimStopLeg2();
	void hrtimStartInterleaved();
	void hrtimStartFullBridgeBuck();
	void hrtimStartLeg1();
	void hrtimStartLeg2();

	// ADC
	void adcInitialize();

	// DAC
	void dacConfigDac1CurrentmodeInit(hrtim_tu_t tu_src);
	void dacConfigDac3CurrentmodeInit(hrtim_tu_t tu_src);

	// Comparator
	void comparatorInitialize();

private:
	// Common
	static hardware_version_t hardware_version;

	// LED
	static bool ledInitialized;

	// Timer
	static bool timer4init;
	static bool timer4started;

	// Power driver
	static bool powerDriverInitialized;

	// HRTIM
	static uint16_t hrtimPwmPeriod;
	static uint16_t hrtimPwmPhaseShift;
	static uint16_t hrtimPwmPhaseShiftLeg1;
	static uint16_t hrtimPwmPhaseShiftLeg2;
	static bool     hrtimFullBridgeBipolarMode;

	static hrtim_tu_t hrtimLeg1Tu;
	static hrtim_tu_t hrtimLeg2Tu;

	// ADC
	static bool adcInitialized;

};


/////
// Public object to interact with the class

extern HardwareConfiguration hwConfig;


#endif // HARDWARECONFIGURATION_H_
