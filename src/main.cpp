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
 * @brief  This file it the main entry point of the
 *         OwnTech Power API. Please check the OwnTech
 *         documentation for detailed information on
 *         how to use Power API: https://docs.owntech.org/
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */

/*--------------OWNTECH APIs---------------------------------- */
#include "TaskAPI.h"
#include "ShieldAPI.h"
#include "SpinAPI.h"

/*--------------SETUP FUNCTIONS DECLARATION------------------- */
/* Setups the hardware and software of the system */
void setup_routine();

/*--------------LOOP FUNCTIONS DECLARATION-------------------- */

/* Code to be executed in the background task */
void loop_background_task();
/* Code to be executed in real time in the critical task */
void loop_critical_task();

/*--------------USER VARIABLES DECLARATIONS------------------- */



/*--------------SETUP FUNCTIONS------------------------------- */

/**
 * This is the setup routine.
 * It is used to call functions that will initialize your hardware and tasks.
 *
 * In this default main, we only spawn two tasks
 *  - A background task.
 *  - A critical task is defined but not started.
 *
 * NOTE: It is important to follow the steps and initialize the hardware first
 * and the tasks second.
 */
void setup_routine()
{
    /* STEP 1 - SETUP THE HARDWARE */

    /* STEP 2 - SETUP THE TASKS */

    uint32_t background_task_number =
                            task.createBackground(loop_background_task);

    /* Uncomment the following line if you use the critical task */
    /* task.createCritical(loop_critical_task, 500); */

    /* STEP 3 - LAUNCH THE TASKS */
    task.startBackground(background_task_number);

    /* Uncomment the following line if you use the critical task */
    /* task.startCritical(); */
}

/*--------------LOOP FUNCTIONS-------------------------------- */

/**
 * This is the code loop of the background task
 * You can use it to execute slow code such as state-machines.
 * The pause define its pseudo-periodicity.
 */
void loop_background_task()
{
    printk("Hello World! \n");
    spin.led.toggle();

    /* This pauses the task for 1000 milli seconds */
    task.suspendBackgroundMs(1000);
}

/**
 * This is the code loop of the critical task
 * It is executed every 500 micro-seconds defined in the setup_routine function.
 * You can use it to execute an ultra-fast code with the highest priority which
 * cannot be interrupted.
 */
void loop_critical_task()
{
    /* This task is left empty in this default main */
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
