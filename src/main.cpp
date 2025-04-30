/*
 * Copyright (c) 2021-present LAAS-CNRS
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
 * @brief  This examples shows how to use SpinAPI to define fast and precise
 *         PWM signals.
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 * @author Ayoub Farah Hassan <ayoub.farah-hassan@laas.fr>
 */

/* --------------Zephyr---------------------------------------- */
#include <zephyr/console/console.h>

/* --------------OWNTECH APIs---------------------------------- */
#include "SpinAPI.h"
#include "TaskAPI.h"


/* --------------SETUP FUNCTIONS DECLARATION------------------- */
/* Setups the hardware and software of the system */
void setup_routine();

/* --------------LOOP FUNCTIONS DECLARATION-------------------- */
/* Code to be executed in the slow communication task */
void loop_communication_task();
/* Code to be executed in the background task */
void loop_application_task();
/* Code to be executed in real time in the critical task  */
void loop_critical_task();

/* --------------USER VARIABLES DECLARATIONS------------------- */
uint8_t received_serial_char;

float32_t duty_cycle = 0.3;
float32_t frequency_min = 50000;
float32_t frequency = 200000;

/* --------------------------------------------------------------- */

/* List of possible modes for the OwnTech Board. */
enum serial_interface_menu_mode
{
    IDLEMODE = 0,
    POWERMODE
};

uint8_t mode = IDLEMODE;

/* --------------SETUP FUNCTIONS------------------------------- */

/**
 * This is the setup routine.
 * Here we define a simple PWM signal on PWMA and we spawn three tasks.
 */
void setup_routine()
{
    /* Set frequency of PWM */
    spin.pwm.initVariableFrequency(frequency,frequency_min);
    /* Timer initialization */

    spin.pwm.setModulation(PWMA,UpDwn);
    spin.pwm.setModulation(PWMC,UpDwn);
    spin.pwm.setModulation(PWMD,UpDwn);
    spin.pwm.setModulation(PWME,UpDwn);
    spin.pwm.setModulation(PWMF,UpDwn);

    spin.pwm.setSwitchConvention(PWMA,PWMx1);
    spin.pwm.setSwitchConvention(PWMC,PWMx1);
    spin.pwm.setSwitchConvention(PWMD,PWMx1);
    spin.pwm.setSwitchConvention(PWME,PWMx1);
    spin.pwm.setSwitchConvention(PWMF,PWMx1);

    spin.pwm.initUnit(PWMA);
    spin.pwm.initUnit(PWMC);
    spin.pwm.initUnit(PWMD);
    spin.pwm.initUnit(PWME);
    spin.pwm.initUnit(PWMF);

    /* Start PWM */
    spin.pwm.startDualOutput(PWMA);
    spin.pwm.startDualOutput(PWMC);
    spin.pwm.startDualOutput(PWMD);
    spin.pwm.startDualOutput(PWME);
    spin.pwm.startDualOutput(PWMF);

    /* Then we declare tasks */
    uint32_t app_task_number = task.createBackground(loop_application_task);
    uint32_t com_task_number = task.createBackground(loop_communication_task);
    task.createCritical(loop_critical_task, 100);

    /* Finally, we start tasks */
    task.startBackground(app_task_number);
    task.startBackground(com_task_number);
    task.startCritical();
}

/* --------------LOOP FUNCTIONS-------------------------------- */

/**
 * Here we implement a minimalistic USB serial interface.
 * Duty cycle can be controlled by pressing U and D keys.
 * This will respectively Increase or Decrease PWM duty cycle.
 */
void loop_communication_task()
{
    received_serial_char = console_getchar();
    switch (received_serial_char)
    {
    case 'h':
        /* ----------SERIAL INTERFACE MENU----------------------- */
        printk(" ________________________________________ \n"
               "|     ------- MENU ---------             |\n"
               "|     press u : duty cycle UP            |\n"
               "|     press d : duty cycle DOWN          |\n"
               "|________________________________________|\n\n");
        /* ------------------------------------------------------ */
        break;
    case 'u':
        duty_cycle += 0.05;
        break;
    case 'd':
        duty_cycle -= 0.05;
        break;
    case 'r':
        frequency += 1000;
        spin.pwm.setFrequency(frequency);
        break;
    case 'f':
        frequency -= 1000;
        spin.pwm.setFrequency(frequency);
        break;
    default:
        break;
    }
}

/**
 * This is the code loop of the background task
 * Here the task is sending the duty cycle that have been set
 * on the USB serial console every second.
 */
void loop_application_task()
{
    /* Task content */
    printk("%f\n", (double)duty_cycle);

    /* Pause between two runs of the task */
    task.suspendBackgroundMs(1000);

}

/**
 * This is the code loop of the critical task
 * In this task than runs periodically in Real Time at 10kHz, we simply
 * update the duty cycle provided through the serial communication.
 */
void loop_critical_task()
{
    spin.pwm.setDutyCycle(PWMC, duty_cycle);
    spin.pwm.setDutyCycle(PWMA, duty_cycle);
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
