/*
 * Copyright (c) 2021-2023 LAAS-CNRS
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
 * @brief   This file it the main entry point of the
 *          OwnTech Power API. Please check the README.md
 *          file at the root of this project for basic
 *          information on how to use the Power API,
 *          or refer the the wiki for detailed information.
 *          Wiki: https://gitlab.laas.fr/owntech/power-api/core/-/wikis/home
 *
 * @author  Clément Foucher <clement.foucher@laas.fr>
 * 			Luiz Villa <luiz.villa@laas.fr>
 */
//-------------OWNTECH API INCLUDES-------------------
#include "HardwareConfiguration.h"
#include "DataAcquisition.h"
#include "Scheduling.h"
#include "power.h"


//--------------SETUP FUNCTIONS DECLARATION-------------------
void setup_hardware(); //setups the hardware peripherals of the system
void setup_software(); //setups the scheduling of the software and the control method

//-------------LOOP FUNCTIONS DECLARATION----------------------
void loop_application_task();   //code to be executed in the fast application task
void loop_control_task();       //code to be executed in real-time at 20kHz

//--------------USER VARIABLES DECLARATIONS----------------------

// Add the variables you will use in your code here

//---------------------------------------------------------------


//---------------SETUP FUNCTIONS----------------------------------

/**
 * This is the setup hardware function
 * It is used to setup your hardware architecture. 
 * The base architecture of this example is composed of a SPIN board.
 */

void setup_hardware()
{
    hwConfig.setBoardVersion(TWIST_v_1_1_2);
    // Setup your hardware here
}

/**
 * This is the setup software function
 * It is used to setup your software architecture. 
 * The base architecture of this example is composed of a slow application task and a fast control task.
 * The slow task is asynchronous, meaning it is called by the embeeded RTOS. 
 * The fast control task is synchronous and driven by an interruption. 
 */

void setup_software()
{
    uint8_t control_task_period_us = 1000; // Sets the control task period in micro-seconds
 
    int8_t application_task_number = scheduling.defineAsynchronousTask(loop_application_task);
    scheduling.defineUninterruptibleSynchronousTask(loop_control_task, control_task_period_us);
 
    scheduling.startAsynchronousTask(application_task_number);
    scheduling.startUninterruptibleSynchronousTask();
}

//---------------LOOP FUNCTIONS----------------------------------

/**
 * This is the application task
 * It is executed second as defined by it suspend task in its last line.
 * You can use it to execute slow code such as state-machines.
 */
void loop_application_task()
{
	printk("Application on! \n");
	hwConfig.setLedToggle();

	scheduling.suspendCurrentTaskMs(1000);
}

/**
 * This is the control task
 * It is executed every 1000 micro-seconds defined in the setup_software function.
 * You can use it to execute ultra-fast code and control your power flow. 
 */
void loop_control_task()
{
	printk("Control on! \n");
}



/**
 * This is the main function of this example
 * This function is generic and does not need editing.
 */
int main(void)
{
    setup_hardware();
    setup_software();
	return 0;
}
