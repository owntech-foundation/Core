/*
 * Copyright (c) 2021-2024 LAAS-CNRS
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
 * @brief  This file it the main entry point of the
 *         OwnTech Power API. Please check the OwnTech
 *         documentation for detailed information on
 *         how to use Power API: https://docs.owntech.org/
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

//--------------Zephyr----------------------------------------
#include <zephyr/console/console.h>

//--------------OWNTECH APIs----------------------------------
#include "SpinAPI.h"
#include "ShieldAPI.h"
#include "TaskAPI.h"
#include "SafetyAPI.h"

//--------------OWNTECH Libraries-----------------------------
#include "pid.h"

//--------------SETUP FUNCTIONS DECLARATION-------------------
void setup_routine(); // Setups the hardware and software of the system

//--------------LOOP FUNCTIONS DECLARATION--------------------
void loop_communication_task(); // code to be executed in the slow communication task
void loop_application_task();   // Code to be executed in the background task
void loop_critical_task();     // Code to be executed in real time in the critical task

//--------------USER VARIABLES DECLARATIONS-------------------

static uint32_t control_task_period = 100; //[us] period of the control task
static bool pwm_enable = false;            //[bool] state of the PWM (ctrl task)

uint8_t received_serial_char;

/* Measure variables */

static float32_t VIN_value;
static float32_t VOUT_value;
static float32_t IL_value;

static int8_t test_V_OUT;
static int8_t test_V_IN; 
static int8_t test_I_L;  

static uint8_t dataValid;


static float meas_data; // temp storage meas value (ctrl task)

float32_t duty_cycle = 0.3;

static float32_t voltage_reference = 15; //voltage reference

/* PID coefficient for a 8.6ms step response*/

static float32_t kp = 0.000215;
static float32_t Ti = 7.5175e-5;
static float32_t Td = 0.0;
static float32_t N = 0.0;
static float32_t upper_bound = 1.0F;
static float32_t lower_bound = 0.0F;
static float32_t Ts = control_task_period * 1e-6;
static PidParams pid_params(Ts, kp, Ti, Td, N, lower_bound, upper_bound);
static Pid pid;

//---------------------------------------------------------------

enum serial_interface_menu_mode // LIST OF POSSIBLE MODES FOR THE OWNTECH CONVERTER
{
    IDLEMODE = 0,
    POWERMODE
};

uint8_t mode = IDLEMODE;

//--------------SETUP FUNCTIONS-------------------------------

/**
 * This is the setup routine.
 * It is used to call functions that will initialize your spin, twist, data and/or tasks.
 * In this example, we setup the version of the spin board and a background task.
 * The critical task is defined but not started.
 */
void setup_routine()
{
    /* buck voltage mode */
    shield.power.initBuck(LEG1);

	spin.data.configureTriggerSource(ADC_1, hrtim_ev1);

	uint32_t num_discontinuous_meas = 1;
	spin.data.configureDiscontinuousMode(ADC_1, num_discontinuous_meas);

    test_V_OUT = shield.sensors.enableSensor(V_OUT,ADC_1);
    test_V_IN  = shield.sensors.enableSensor(V_IN,ADC_1);
    test_I_L   = shield.sensors.enableSensor(I_L,ADC_1);

    // safety.init_shield();
    // safety.setChannelReaction(Open_Circuit);
    // safety.enableSafetyApi();    

    // shield.sensors.setConversionParametersLinear(I_L,1.0,0.0);

    pid.init(pid_params);

    // Then declare tasks
    uint32_t app_task_number = task.createBackground(loop_application_task);
    uint32_t com_task_number = task.createBackground(loop_communication_task);
    task.createCritical(loop_critical_task, 100); // Uncomment if you use the critical task

    // Finally, start tasks
    task.startBackground(app_task_number);
    task.startBackground(com_task_number);
    task.startCritical(); // Uncomment if you use the critical task
}

//--------------LOOP FUNCTIONS--------------------------------

void loop_communication_task()
{
    received_serial_char = console_getchar();
    switch (received_serial_char)
    {
    case 'h':
        //----------SERIAL INTERFACE MENU-----------------------
        printk(" ________________________________________\n");
        printk("|     ---- MENU buck voltage mode ----   |\n");
        printk("|     press i : idle mode                |\n");
        printk("|     press p : power mode               |\n");
        printk("|     press u : voltage reference UP     |\n");
        printk("|     press d : voltage reference DOWN   |\n");
        printk("|________________________________________|\n\n");
        //------------------------------------------------------
        break;
    case 'i':
        printk("idle mode\n");
        mode = IDLEMODE;
        break;
    case 'p':
        printk("power mode\n");
        mode = POWERMODE;
        break;
    case 'u':
        voltage_reference += 0.5;
        break;
    case 'd':
        voltage_reference -= 0.5;
        break;
    default:
        break;
    }
}

/**
 * This is the code loop of the background task
 * It is executed second as defined by it suspend task in its last line.
 * You can use it to execute slow code such as state-machines.
 */
void loop_application_task()
{
    if (mode == IDLEMODE)
    {
        spin.led.turnOff();
    }
    else if (mode == POWERMODE)
    {
        spin.led.turnOn();
    }

    printk("%.3f:", (double)IL_value);
    printk("%d:", dataValid);
    printk("%d:", test_V_OUT);
    printk("%d:", test_V_IN);
    printk("%d:", test_I_L);
    printk("%.3f:", (double)VOUT_value);
    printk("%.3f:", (double)voltage_reference);
    printk("%.3f:", (double)VIN_value);
    printk("%.3f:", (double)duty_cycle);
    printk("\n");

    task.suspendBackgroundMs(100);
}

/**
 * This is the code loop of the critical task
 * It is executed every 500 micro-seconds defined in the setup_software function.
 * You can use it to execute an ultra-fast code with the highest priority which cannot be interruped.
 * It is from it that you will control your power flow.
 */
void loop_critical_task()
{
    meas_data = shield.sensors.getLatestValue(I_L,&dataValid);
    if (meas_data != NO_VALUE) IL_value = meas_data;

    meas_data = shield.sensors.getLatestValue(V_OUT);
    if (meas_data != NO_VALUE) VOUT_value = meas_data;

    meas_data = shield.sensors.getLatestValue(V_IN);
    if (meas_data != NO_VALUE) VIN_value = meas_data;

    if (mode == IDLEMODE)
    {
        if (pwm_enable == true)
        {
            shield.power.stop(LEG1);
        }
        pwm_enable = false;
    }
    else if (mode == POWERMODE)
    {
        duty_cycle = pid.calculateWithReturn(voltage_reference, VOUT_value);
        shield.power.setDutyCycle(LEG1,duty_cycle);

        /* Set POWER ON */
        if (!pwm_enable)
        {
            pwm_enable = true;
            shield.power.start(LEG1);
        }
    }

}

/**
 * This is the main function of this example
 * This function is generic and does not need editing.
 */
int main(void)
{
    setup_routine();

    return 0;
}
