/*
 * Copyright (c) 2023-present LAAS-CNRS
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
 * @author Jean Alinei <jean.alinei@owntech.org
 *
 * @brief This file is based on the device tree to initialize legs controlled
 * 		  by the HRTIM
 */

#ifndef POWER_H_
#define POWER_H_

#include <zephyr/kernel.h>
#include "arm_math.h"
#include "hrtim_enum.h"

#define LEG_TOKEN(node_id) DT_STRING_TOKEN(node_id, leg_name),

/**  @brief Parses all the legs with okay status in the device tree and fills 
 * 			this type def. 
 * 
 * 			- `LEG1` to `LEG5` - values supported by the SPIN
 * 
 * 			- `ALL` - Applies the function to all legs in the list 
 * 
*/
typedef enum
{
	DT_FOREACH_CHILD_STATUS_OKAY(DT_NODELABEL(powershield), LEG_TOKEN)
	ALL
} leg_t;

class PowerAPI
{
private:
	/* return timing unit from spin pin number */
	hrtim_tu_number_t spinNumberToTu(uint16_t spin_number);
	static hrtim_tu_number_t spinNumberToTuStatic(uint16_t spin_number);

	/* convert a duty cycle into a raw sequence value for a leg index */
	uint32_t dutyToSequenceRaw(int8_t leg_index, float32_t duty);


public:
	/**
	 * @brief Initialize the power mode for a given leg.
	 *
	 * This function configures modulation, frequency, phase shift, dead time,
	 * and other parameters for power control of a specified leg.
	 *
	 * @param leg The leg to initialize:`LEG1` to `ALL`
	 * @param leg_convention PWM Switch to be driven by the duty cycle: 
	 * 						 `PWMx1`, `PWMx2`
	 * @param leg_mode PWM mode: `VOLTAGE_MODE`, `CURRENT_MODE`
	 */
	void initMode(leg_t leg,
				  hrtim_switch_convention_t leg_convention,
				  hrtim_pwm_mode_t leg_mode);

	/**
	 * @brief Set the duty cycle for a specific leg's power control.
	 *
	 * This function sets the duty cycle for the power control of a specified 
	 * leg. 
	 * 
	 * The duty cycle determines the ON/OFF ratio of the power signal for the 
	 * leg.
	 *
	 * @param leg The leg for which to set the duty cycle: `LEG1` to `ALL` 
	 * @param duty_value The duty cycle value to set
	 * 				   (a floating-point number between `0.1` and `0.9`).
	 */
	void setDutyCycle(leg_t leg, float32_t duty_value);

	/**
	 * @brief Set the duty cycle for a specific leg's power control.
	 *
	 * This function sets the duty cycle for the power control of a specified 
	 * leg. 
	 * 
	 * The duty cycle determines the ON/OFF time ratio of the power signal 
	 * for the leg.
	 *
	 * @param leg The leg for which to set the duty cycle: `LEG1` to `ALL` 
	 * @param duty_value The duty cycle value to set an unsigned integer number 
	 * 				   between `duty_cycle_min_raw` and `duty_cycle_max_raw`.
	 */
	void setDutyCycleRaw(leg_t leg, uint16_t duty_value);


	/**
	 * @brief Start power output for a specific leg.
	 *
	 * This function initiates the power output for a specified leg by starting
	 * the PWM subunits associated with the leg's output pins. 
	 * 
	 * If the leg's output pins are not declared inactive in the device tree, 
	 * the corresponding subunits are started. eg. 
	 * 
	 * If output1 is declared inactive in the device tree, PWMA1 will not start.
	 *
	 * @param leg The leg for which to start the power output: `LEG1` to `ALL`
	 */
	void start(leg_t leg);

	/**
	 * @brief Stop power output for a specific leg.
	 *
	 * @param leg The leg for which to stop the power output: `LEG1` to `ALL`
	 */
	void stop(leg_t leg);

	/**
	 * @brief Starts the outputs of several legs with a single register
	 *        write, so that they turn on at the same instant.
	 *
	 * @param legs Legs to start: `LEG1` to `LEG5`
	 * @param count Number of legs
	 * @param at_period_start `true` to wait for the next PWM period
	 *        boundary of the first leg before enabling (at most one period),
	 *        so that no part of the previous period appears on the outputs
	 */
	void start(const leg_t *legs, uint8_t count, bool at_period_start = false);

	/**
	 * @brief Prepares a start of several legs (driver pins, output mask)
	 *        without enabling anything; enable later with startPrepared().
	 *        Splits start(legs, count) so that the slow part runs before a
	 *        time-critical wait (it takes several us, more than a period at
	 *        200 kHz).
	 *
	 * @return mask of the HRTIM outputs to enable
	 */
	uint32_t prepareStart(const leg_t *legs, uint8_t count);

	/**
	 * @brief Enables the outputs prepared by prepareStart(), one register
	 *        write, no wait.
	 */
	void startPrepared(uint32_t outputs);

	/**
	 * @brief Stops the outputs of several legs with a single register
	 *        write, then ends their duty-cycle sequences.
	 *
	 * @param legs Legs to stop: `LEG1` to `LEG5`
	 * @param count Number of legs
	 * @param at_period_end `true` to wait until the first leg is
	 *        POWER_STOP_MARGIN_NS (2 us) before the end of its PWM period
	 *        (at most one period), so that no part of the next period
	 *        appears on the outputs. `Lft_aligned` legs only.
	 */
	void stop(const leg_t *legs, uint8_t count, bool at_period_end = false);

	/**
	 * @brief Busy-waits until the PWM counter of a leg is in the middle of
	 *        its period ([1/2, 3/4)), at most about two periods. A fixed
	 *        reference point within the period, away from the boundary.
	 *
	 * @param leg Leg: `LEG1` to `LEG5`
	 */
	void waitPeriodMiddle(leg_t leg);

	/**
	 * @brief Busy-waits for `boundaries` PWM period boundaries of a leg
	 *        (counter wraps), each bounded to about two periods.
	 *
	 * @param leg Leg: `LEG1` to `LEG5`
	 * @param boundaries Number of boundaries to wait for
	 */
	void waitPeriodStart(leg_t leg, uint16_t boundaries = 1);

	/**
	 * @brief Returns the PWM counter of a leg (0 to getPeriod(leg)), e.g.
	 *        to record where in its period an event happened.
	 *
	 * @param leg Leg: `LEG1` to `LEG5`
	 */
	uint16_t getPeriodCounter(leg_t leg);

	/**
	 * @brief Connect the electrolytic capacitor.
	 *
	 * @param leg The selected leg for which the capacitor will be connected:
	 * 		  `LEG1` to `ALL`
	 *
	 * @warning This function can only be called AFTER initializing the `LEG`.
	 */
	void connectCapacitor(leg_t leg);

	/**
	 * @brief Disconnect the electrolytic capacitor.
	 *
	 * @param leg The selected leg for which the capacitor will be disconnected:
	 * 			  `LEG1` to `ALL`
	 *
	 * @warning This function can only be called AFTER initializing the `LEG`.
	 */
	void disconnectCapacitor(leg_t leg);

	/**
	 * @brief Turns the driver of the leg ON. This enables control over the
	 * 		  power switches.
	 *
	 * @param leg The leg for which the driver will be turned on: `LEG1` to `ALL`
	 *
	 * @warning This function can only be called AFTER initializing the LEG.
	 */
	void connectDriver(leg_t leg);

	/**
	 * @brief Turns the driver of the leg OFF. 
	 * 
	 * 		  This disables control over the power switches.
	 *
	 * @param leg The leg for which the driver will be turned off: `LEG1` to `ALL`
	 *
	 * @warning This function can only be called AFTER initializing the LEG.
	 */
	void disconnectDriver(leg_t leg);

	/**
	 * @brief Set the trigger value for a specific leg's ADC trigger.
	 *
	 * This function sets the trigger value for the ADC trigger of a specified
	 * leg. 
	 * 
	 * The trigger value determines the level at which the ADC trigger is 
	 * activated.
	 *
	 * @param leg The leg for which to set the ADC trigger value: `LEG1` to `ALL`
	 * @param trigger_value The trigger value to set between 0.05 and 0.95.
	 */
	void setTriggerValue(leg_t leg, float32_t trigger_value);

	/**
	 * @brief Set the phase shift value for a specific leg's power control.
	 *
	 * @param leg The leg for which to set the phase shift value: `LEG1` to `ALL`
     * @param[in] phase_shift	phase shift value in degrees 
	 * 							between `-360` and `360`
	 */
	void setPhaseShift(leg_t leg, int16_t phase_shift);

	/**
	 * @brief Set the slope compensation in current mode for a leg
	 *
	 * @param leg The leg to initialize: `LEG1` to `ALL`
	 * @param set_voltage in volt, the DAC peak voltage for slope compensation
	 * 					  (value between `0` and `2.048`)
	 * @param reset_voltage in volt, the DAC lowest voltage for
	 * 						slope compensation (value between `0` and `2.048`)
	 *
	 * @warning Be sure that `set_voltage > reset_voltage`
	 */
	void setSlopeCompensation(leg_t leg,
							  float32_t set_voltage,
							  float32_t reset_voltage);

	/**
	 * @brief set the dead time value for a leg
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `ALL`
	 * @param ns_rising_dt  rising dead time value in nanoseconds
	 * @param ns_falling_dt falling dead time value in nanoseconds
	 */
	void setDeadTime(leg_t leg,
					 uint16_t ns_rising_dt,
					 uint16_t ns_falling_dt);

	/**
	 * @brief Sets the PWM frequency shared by the shield legs.
	 *
	 * This updates the underlying HRTIM period and refreshes duty-cycle
	 * limits so subsequent duty updates stay aligned with the new period.
	 *
	 * @param frequency switching frequency in Hz
	 */
	void setFrequency(uint32_t frequency);

	/**
	 * @brief Gets the currently configured shared PWM frequency in Hz.
	 *
	 * @return shared switching frequency in Hz
	 */
	uint32_t getFrequency();

	/**
	 * @brief Gets the minimum allowed shared PWM frequency in Hz.
	 *
	 * @return minimum switching frequency in Hz
	 */
	uint32_t getFrequencyMin();

	/**
	 * @brief sets the Minimum Duty Cycle Limit
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `ALL`
	 * @param duty_cycle    - new minimum duty cycle value between 0.0 and 1.0
	*/
	void setDutyCycleMin(leg_t leg, float32_t duty_cycle);

	/**
	 * @brief sets the Minimum Duty Cycle Limit as an unsigned int
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `ALL`
	 * @param duty_cycle - new minimum duty cycle value between `0` and `period`
	*/
	void setDutyCycleMinRaw(leg_t leg, uint16_t duty_cycle);

	/**
	 * @brief sets the Maximum Duty Cycle Limit as an unsigned int
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `ALL`
	 * @param duty_cycle - new minimum duty cycle value between `0` and `period`
	*/
	void setDutyCycleMaxRaw(leg_t leg, uint16_t duty_cycle);

	/**
	 * @brief sets the Maximum Duty Cycle Limit
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `ALL`
	 * @param duty_cycle    - new maximum duty cycle value between 0.0 and 1.0
	*/
	void setDutyCycleMax(leg_t leg, float32_t duty_cycle);

	/**
	 * @brief gets the Maximum Duty Cycle Limit as a float
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `LEG5`.
	 * @warning `ALL` is NOT supported !
	*/
	float32_t getDutyCycleMax(leg_t leg);

	/**
	 * @brief gets the Maximum Duty Cycle Limit as an unsigned integer.
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `LEG5`.
	 * @warning `ALL` is NOT supported !
	*/
	uint16_t getDutyCycleMaxRaw(leg_t leg);

	/**
	 * @brief gets the Minimum Duty Cycle Limit as a float
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `LEG5`.
	 * @warning `ALL` is NOT supported !
	*/
	float32_t getDutyCycleMin(leg_t leg);

	/**
	 * @brief gets the Minimum Duty Cycle Limit as an unsigned integer.
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `LEG5`.
	 * @warning `ALL` is NOT supported !
	*/
	uint16_t getDutyCycleMinRaw(leg_t leg);

	/**
	 * @brief returns the value of the leg period as an unsigned integer
	 *
	 * @param leg the leg for which to set dead time value: `LEG1` to `LEG5`.
	 * @warning `ALL` is NOT supported !
	*/
	uint16_t getPeriod(leg_t leg);


	/**
	 * @brief Sets ADC decimator for a leg
	 *
	 * This function sets the number of event which will be ignored between
	 * two events. ie. you divide the number of trigger in a fixed period.
	 * 
	 * For example if adc_decim = 1, nothing changes but with adc_decims = 2
	 * you have twice less adc trigger.
	 *
	 * @param leg leg for which to set adc decimator value: `LEG1` to `ALL`
	 * @param adc_decim adc decimator, a number between 1 and 32
	 *
	 */
	void setAdcDecim(leg_t leg, uint16_t adc_decim);

	/**
	 * @brief Initialise a leg for buck topology
	 *
	 * @param leg Leg to initialize: `LEG1` to `ALL`
	 * @param leg_mode PWM mode - `VOLTAGE_MODE` or `CURRENT_MODE`
	 * 
	 * @note `CURRENT_MODE` only available to `LEG1` and `LEG2` for the 
	 * 	      Twist board
	 *
	 */
	void initBuck(leg_t leg, hrtim_pwm_mode_t leg_mode = VOLTAGE_MODE);

	/**
	 * @brief Initialise all the legs for boost topology, current mode is not
	 * supported for boost.
	 *
	 * @param leg Leg to initialize: `LEG1` to `ALL`
	 */
	void initBoost(leg_t leg);

	/**
	 * @brief Loads a duty-cycle sequence on a leg: one duty value per PWM
	 *        period, applied by DMA without CPU intervention and repeated
	 *        until stopped.
	 *
	 *        Duty values follow the same limits as setDutyCycle()
	 *        (setDutyCycleMin/Max). `1.0` gives a full period on.
	 *
	 *        If the sequence of the leg is running, it is stopped, reloaded
	 *        and restarted.
	 *
	 * @param leg Leg: `LEG1` to `ALL` (`ALL` loads the same sequence on all legs)
	 * @param duties Duty cycle values between 0.0 and 1.0, copied
	 * @param length Number of values, 1 to CONFIG_OWNTECH_POWER_DUTY_SEQ_MAX_LEN
	 * @param double_buffered `true` to allow glitch-free updates while
	 *        running with updateDutySequence(). Uses DMA half/complete
	 *        interrupts (one every `length` PWM periods).
	 *
	 * @return `0` if ok, `-1` if the length is invalid, DMA2 is not enabled
	 *         or the leg is not `Lft_aligned`.
	 *
	 * @note  Requires `&dma2 { status = "okay"; };` and
	 *        `default-modulation = "Lft_aligned"` on the leg in app.overlay.
	 */
	int8_t setDutySequence(leg_t leg,
						   const float32_t *duties,
						   uint16_t length,
						   bool double_buffered = false);

	/**
	 * @brief Replaces the values of a loaded duty-cycle sequence, same length.
	 *
	 *        Double-buffered sequence: the new values are staged and each
	 *        half of the DMA buffer picks them up once it has been played,
	 *        so the change is applied at a sequence boundary within at most
	 *        two sequence lengths, never in the middle of a period.
	 *        Call it from the critical task, so that the DMA interrupt cannot
	 *        copy a half-written update.
	 *
	 *        Single-buffered sequence: same as setDutySequence() (stop,
	 *        reload, restart from index 0).
	 *
	 * @param leg Leg: `LEG1` to `ALL`
	 * @param duties New duty values, as many as the loaded sequence
	 *
	 * @return `0` if ok, `-1` if no sequence is loaded on the leg
	 */
	int8_t updateDutySequence(leg_t leg, const float32_t *duties);

	/**
	 * @brief Starts the duty-cycle sequence of a leg. With `ALL`, every leg
	 *        with a loaded sequence starts on the same PWM period.
	 *
	 *        Waits for the middle of a PWM period (up to about two periods,
	 *        interrupts locked), so that the first value is always applied
	 *        the same number of periods after the call. The PWM outputs must
	 *        be started with start().
	 *
	 * @param leg Leg: `LEG1` to `ALL`
	 *
	 * @return `0` if ok, `-1` if no sequence is loaded or it already runs
	 */
	int8_t startDutySequence(leg_t leg);

	/**
	 * @brief Stops the duty-cycle sequence of a leg and sets its duty cycle
	 *        to the minimum duty cycle (setDutyCycleMin). Outputs stay on.
	 *
	 * @param leg Leg: `LEG1` to `ALL`
	 */
	void stopDutySequence(leg_t leg);

	/**
	 * @brief Returns true if the duty-cycle sequence of the leg runs.
	 *
	 * @param leg Leg: `LEG1` to `LEG5`
	 * @warning `ALL` is NOT supported !
	 */
	bool isDutySequenceRunning(leg_t leg);

	/**
	 * @brief Returns the index of the next sequence value the DMA will write.
	 *
	 * @param leg Leg: `LEG1` to `LEG5`
	 * @warning `ALL` is NOT supported !
	 */
	uint16_t getDutySequenceIndex(leg_t leg);

	/**
	 * @brief Returns the number of DMA transfer errors since the last start.
	 *
	 * @param leg Leg: `LEG1` to `LEG5`
	 * @warning `ALL` is NOT supported !
	 */
	uint32_t getDutySequenceErrors(leg_t leg);

};

#endif /* POWER_H_ */
