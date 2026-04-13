/*
 * Copyright (c) 2025 Pierre Haessig
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
 * @brief  Application for islanded inverter (open loop),
 *         with fixed, adjustable amplitude and frequency,
 *         using the three-phase OwnVerter board.
 *
 * @author Pierre Haessig <pierre.haessig@centralesupelec.fr>
 */

/* --------------OWNTECH APIs---------------------------------- */
#include "TaskAPI.h"
#include "ShieldAPI.h"
#include "SpinAPI.h"

/* OWNTECH CONTROL LIBRARY (including trigonometric functions) */
#include "control_factory.h"
#include "transform.h"
#include "ScopeMimicry.h"

#include "zephyr/console/console.h"

/* --------------SETUP AND LOOP FUNCTIONS DECLARATION------------------- */

/* Set up of hardware and software at board startup */
void setup_routine();

/* Interaction with the user on the serial monitor (slow background task) */
void user_interface_task();
/* Displaying board status messages on the serial monitor (slow background task) */
void status_display_task();
/* Power converter control (critical periodic task) */
void control_task();
/* Compute duty cycles (subroutine of control task)*/
void compute_duties();
/* Read analog measurements (subroutine of control task)*/
void read_measurements();

/* -------------- VARIABLES DECLARATIONS------------------- */

static const float32_t T_control = 100e-6F; // Control task period (s)
static const uint32_t T_control_micro = (uint32_t)(T_control * 1.e6F); // Control task period (integer number of µs)

/* SINUSOIDAL SIGNAL GENERATION VARIABLES */
static float32_t v_freq = 50.0; // inverter voltage frequency (Hz)
static float32_t v_angle = 0.0; // inverter voltage angle (rad)
const float32_t freq_increment = 10.0; // frequency up or down increment (Hz)
static float32_t duty_offset = 0.50; // duty cycle offset. Should be close to 50% to offer maximal amplitude.
static float32_t duty_amplitude = 0.0; // amplitude for sinusoidal duty cycle
float32_t duty_increment = 0.01; // duty cycle amplitude up or down increment
static bool square_wave = false; // whether to use square wave modulation, rather than PWM
static bool trigger = false; // trigger for ScopeMimicry acquisition, set to true in the user interface task when the user presses 't' key

/* BOARD POWER CONVERSION STATE VARIABLES */
static bool power_enable = false; // Power conversion state of the leg (PWM activation state)
static float32_t duty_a, duty_b, duty_c; // three-phase PWM duty cycle (phases a, b, c)
static float32_t max_duty = 0.9; // maximum duty cycle to avoid overmodulation and voltage overshoot, between 0.1 and 0.9


/* Possible modes for the OwnTech board */
enum serial_interface_menu_mode
{
	IDLE_MODE = 0,
	POWER_MODE
};

uint8_t mode = IDLE_MODE; // Currently user-requested mode

/* COMMUNICATION AND MEASUREMENT VARIABLES */

uint8_t received_serial_char; // Temporary storage for serial monitor character reception

/* Measurement variables */

static float32_t V_high; // High-side voltage (DC bus)
static float32_t I_high; // High-side current (DC bus current to the legs)
// static float32_t Va, Vb, Vc; // AC-side phase voltages
static float32_t Va, Vb, Vc; // AC-side phase voltages
static float32_t Ia, Ib, Ic; // AC-side phase currents

static float meas_data; // Temporary storage for measured value

/* V_high filter (5ms lowpass)*/
static LowPassFirstOrderFilter vHigh_filter = controlLibFactory.lowpassfilter(T_control, 5.0e-3F);
static float32_t V_high_filt; // High-side voltage (DC bus), smoothed by lowpass filter

/* ScopeMimicry capture state and helpers. */

const uint16_t SCOPE_SIZE = 2096;
uint16_t k_app_idx;
ScopeMimicry scope(SCOPE_SIZE, 2);
static bool is_downloading;
static bool memory_print;


bool mytrigger()
{
	return (trigger = true);
}

void dump_scope_datas(ScopeMimicry &scope) {
	printk("begin record\n");
	scope.reset_dump();
	while (scope.get_dump_state() != finished) {
		printk("%s", scope.dump_datas());
		task.suspendBackgroundUs(200);
	}
	printk("end record\n");
}


/* -------------- SETUP FUNCTION -------------------------------*/

/**
 * Setup routine, called at board startup.
 * It is used to initialize the board (spin microcontroller and power shield)
 * and the application (set tasks).
 */
void setup_routine()
{
	spin.led.turnOn(); // Blink LED at board startup

	/* Set the high switch convention for all legs */
	shield.power.initBuck(ALL);
	shield.power.setDutyCycleMin(ALL, 0.0);
	shield.power.setDutyCycleMax(ALL, 1.0);

	/* Setup all the measurements */
	shield.sensors.enableDefaultOwnverterSensors();

	/* Declare tasks */
	uint32_t app_task_number = task.createBackground(status_display_task);
	uint32_t com_task_number = task.createBackground(user_interface_task);
	task.createCritical(control_task, T_control_micro);


	scope.connectChannel(Va, "Va");           /* 0 */
	scope.connectChannel(duty_a, "duty_a");                         /* 1 */
	scope.set_trigger(&mytrigger);
	scope.set_delay(0.0);
	scope.start();


	/* Start tasks */
	task.startBackground(app_task_number);
	task.startBackground(com_task_number);
	task.startCritical();
}

/* --------------LOOP FUNCTIONS (TASKS) ------------------------------- */

/**
 * User interface task, running in a loop in the background.
 * It allows controlling the application through the serial monitor.
 *
 * It waits for the user to press a key to select an action.
 * In particular, 'h' displays the help menu.
 */
void user_interface_task()
{
	received_serial_char = console_getchar();
	switch (received_serial_char) {
	case 'h':
		/* ----------SERIAL INTERFACE MENU----------------------- */

		printk( " ______________________________________________ \n"
				"|     ------- MENU ---------                   |\n"
				"|     press i   : idle mode                    |\n"
				"|     press p   : power mode                   |\n"
				"|     press s   : toggle square wave mode      |\n"
				"|     press u/o : duty cycle ampl./offset UP   |\n"
				"|     press j/l : duty cycle ampl./offset DOWN |\n"
				"|     press f   : frequency UP                 |\n"
				"|     press v   : frequency DOWN               |\n"
				"|______________________________________________|\n\n");

		/* ------------------------------------------------------ */
		break;
	case 'i':
		printk("Idle mode request\n");
		mode = IDLE_MODE;
		break;
	case 'p':
		printk("Power mode request\n");
		mode = POWER_MODE;
		break;
	case 's':
		if (square_wave) {
			printk("Toggle PWM modulation\n");
			square_wave = false;
		} else {
			printk("Togggle square wave modulation\n");
			square_wave = true;
		}
		break;
	case 'u':
		duty_amplitude += duty_increment;
		printk("Duty cycle amplitude UP (%.2f) \n", (double) duty_amplitude);
		break;
	case 'j':
		duty_amplitude -= duty_increment;
		printk("Duty cycle amplitude DOWN (%.2f) \n", (double) duty_amplitude);
		break;
	case 'o':
		duty_offset += duty_increment;
		printk("Duty cycle offset UP (%.2f) \n", (double) duty_offset);
		break;
	case 'l':
		duty_offset -= duty_increment;
		printk("Duty cycle offset DOWN (%.2f) \n", (double) duty_offset);
		break;
	case 'f':
		v_freq += freq_increment;
		printk("Frequency UP (%.2f Hz) \n", (double) v_freq);
		break;
	case 'v':
		v_freq -= freq_increment;
		printk("Frequency DOWN (%.2f Hz) \n", (double) v_freq);
		break;
	case 'r':
		is_downloading = true;
        trigger = false;
		break;
	case 't':
		/* Relaunch scope acquisition */
        trigger = true;
		scope.start();
		break;
	case 'q':
		max_duty += duty_increment;
		printk("Maximum duty cycle UP (%.2f) \n", (double) max_duty);
		break;
	case 'a':
		max_duty -= duty_increment;
		printk("Maximum duty cycle DOWN (%.2f) \n", (double) max_duty);
		break;
	default:
		break;
	}
}

/**
 * Board status display task, called pseudo-periodically.
 * It displays board measurements on the serial monitor
 * 
 * It also sets the board LED (blinking when POWER_MODE).
 */
void status_display_task()
{
    if (is_downloading) {
		dump_scope_datas(scope);
		is_downloading = false;
	}

	if (mode == IDLE_MODE) {
		spin.led.turnOn(); // Constantly ON led when IDLE
		// Display state:
		printk("IDL: ");

	} else if (mode == POWER_MODE) {
		spin.led.toggle(); // Blinking LED when POWER
		// Display state:
		printk("POW: ");
	}
	// Display duty cycle references (if not square wave):
	if (!square_wave) {
		printk("duty a=%3.0f%% o=%3.0f%% ",
		(double) (duty_amplitude*100),
		(double) (duty_offset*100)
	);
	} else {
		printk("square ");
	}
	printk("@%.0f Hz ", (double) v_freq);
	printk("| ");
	// Display measurements
	printk("Vh %5.2f V, ", (double) V_high);
	printk("Ih %4.2f A, ", (double) I_high);
	printk("\n");
	task.suspendBackgroundMs(200);
}

/* Read measurements from analog sensors, possibly applying some filters,
   through microcontroller ADCs (Analog to Digital Converters).

   Measured signals:
   - currents: Ia, Ib, Ic, I_high
   - voltages: V_high (with smoothed lowpass filtered version)
 */
inline void read_measurements()
{
	meas_data = shield.sensors.getLatestValue(V1_LOW);
	if (meas_data != NO_VALUE) {
		Va = meas_data;
	}

    meas_data = shield.sensors.getLatestValue(I1_LOW);
	if (meas_data != NO_VALUE) {
		Ia = meas_data;
	}

	meas_data = shield.sensors.getLatestValue(I2_LOW);
	if (meas_data != NO_VALUE) {
		Ib = meas_data;
	}

	meas_data = shield.sensors.getLatestValue(I3_LOW);
	if (meas_data != NO_VALUE) {
		Ic = meas_data;
	}

	meas_data = shield.sensors.getLatestValue(I_HIGH);
	if (meas_data != NO_VALUE) {
		I_high = meas_data;
	}

	meas_data = shield.sensors.getLatestValue(V_HIGH);
	if (meas_data != NO_VALUE) {
		V_high = meas_data;
	}

	/* Apply filters */
	// Smooth V_high (lowpass)
	V_high_filt = vHigh_filter.calculateWithReturn(V_high);
}

/* Compute sinusoidal duty cycles for each phase a,b,c 

CODE TO BE MODIFIED! -> DONE
Instruction: implement three-phase sinusoidal duty cycles
*/
inline void compute_duties()
{
	// Update inverter phase (∫ω(t).dt, computed with Euler approximation, modulo 2π)
	float32_t omega = 2*PI*v_freq; // frequency conversion (Hz -> rad/s): ω = 2π.f 
	v_angle = ot_modulo_2pi(v_angle + omega*T_control);
	// Compute duty cycles: CODE TO BE MODIFIED!  -> DONE
	duty_a = duty_offset + duty_amplitude * ot_sin(v_angle);
	if (duty_a > 1.0) duty_a = 1.0;
	else if (duty_a < 0.0) duty_a = 0.0;

	duty_b = duty_offset + duty_amplitude * ot_sin(v_angle - 2.0/3.0*PI);
	if (duty_b > 1.0) duty_b = 1.0;
	else if (duty_b < 0.0) duty_b = 0.0;

	duty_c = duty_offset + duty_amplitude * ot_sin(v_angle - 4.0/3.0*PI);
	if (duty_c > 1.0) duty_c = 1.0;
	else if (duty_c < 0.0) duty_c = 0.0;

	// Square wave inverter variant
	if (square_wave) {
		if (v_angle <= PI) duty_a = max_duty;
		else duty_a = 0.0;
		if (ot_modulo_2pi(v_angle - 2.0/3.0*PI) <= PI) duty_b = max_duty;
		else duty_b = 0.0;
		if (ot_modulo_2pi(v_angle - 4.0/3.0*PI) <= PI) duty_c = max_duty;
		else duty_c = 0.0;
	}
}

/**
 * This is the code loop of the critical task.
 * It is executed every T_control seconds (100 µs by default).
 * 
 * Actions:
 * - measure voltage and currents (in subfunction)
 * - compute duty cycle (in subfunction)
 * - control the power converter leg (ON/OFF state and duty cycle)
 */
void control_task()
{
	/* Retrieve sensor values */
	read_measurements();

	/* Compute sinusoidal duty cycles*/
	compute_duties();

	/* Manage POWER/IDLE modes */
	if (mode == IDLE_MODE) {
		if (power_enable == true) {
			shield.power.stop(ALL);
		}
		power_enable = false;
	} else if (mode == POWER_MODE) {
		/* Set duty cycles of all three legs */
		shield.power.setDutyCycle(LEG1, duty_a);
		shield.power.setDutyCycle(LEG2, duty_b);
		shield.power.setDutyCycle(LEG3, duty_c);
		/* Set POWER ON */
		if (!power_enable) {
			power_enable = true;
			shield.power.start(ALL);
		}
	}
	scope.acquire();
}

/**
 * Main function of the application.
 * This function is generic and does not need editing.
 */
int main(void)
{
	setup_routine();
	return 0;
}