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
 */
//-------------OWNTECH DRIVERS-------------------
#include "HardwareConfiguration.h"
#include "DataAcquisition.h"
#include "Scheduling.h"
#include "power.h"

//------------ZEPHYR DRIVERS----------------------

//--------------SETUP FUNCTIONS DECLARATION-------------------
void setup_hardware(); //setups the hardware peripherals of the system
void setup_software(); //setups the scheduling of the software and the control method

//-------------LOOP FUNCTIONS DECLARATION----------------------
void loop_communication_task(); //code to be executed in the slow communication task
void loop_application_task();   //code to be executed in the fast application task
void loop_control_task();       //code to be executed in real-time at 20kHz

int8_t communication_task_number; //holds the number of the communication task
int8_t application_task_number; //holds the number of the application task


enum serial_interface_menu_mode //LIST OF POSSIBLE MODES FOR THE OWNTECH CONVERTER
{
    IDLEMODE =0,
};

uint8_t received_serial_char;
uint8_t mode = IDLEMODE;

//--------------USER VARIABLES DECLARATIONS----------------------



//---------------------------------------------------------------


//---------------SETUP FUNCTIONS----------------------------------

void setup_hardware()
{
    hwConfig.setBoardVersion(TWIST_v_1_1_2);
    //setup your hardware here
}

void setup_software()
{
    application_task_number = scheduling.defineAsynchronousTask(loop_application_task);
    scheduling.startAsynchronousTask(application_task_number);
    //setup your software scheduling here
}

//---------------LOOP FUNCTIONS----------------------------------

void loop_communication_task()
{
        //communication task code goes here
}


void loop_application_task()
{
        printk("Hello World! \n");
        hwConfig.setLedToggle();

        scheduling.suspendCurrentTaskMs(100);
}


void loop_control_task()
{
    //loop control task code goes here
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
