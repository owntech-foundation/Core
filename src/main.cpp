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
 * SPDX-License-Identifier: LGLPV2.1
 */

/**
 * @brief  This file deploys the code for discussing with a python script for
 *         hardware in teh loop applications. Please check its documentation on the
 *         readme file or at: https://docs.owntech.org/
 *
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */

//--------------Zephyr----------------------------------------
#include <zephyr/console/console.h>

//--------------OWNTECH APIs----------------------------------
#include "SpinAPI.h"
#include "TaskAPI.h"
#include "ShieldAPI.h"
#include "pid.h"
#include "comm_protocol.h"
#include "arm_math_types.h"
#include <ScopeMimicry.h>
#include "CommunicationAPI.h"

#define RECORD_SIZE 128 // Number of point to record


//--------------SETUP FUNCTIONS DECLARATION-------------------
void setup_routine(); // Setups the hardware and software of the system

//--------------LOOP FUNCTIONS DECLARATION--------------------
void loop_application_task();   // Code to be executed in the background task
void loop_communication_task();   // Code to be executed in the background task
void loop_control_task();     // Code to be executed in real time in the critical task


//--------------USER VARIABLES DECLARATIONS----------------------


static uint32_t control_task_period = 100; //[us] period of the control task
static bool pwm_enable_leg_1 = false;            //[bool] state of the PWM (ctrl task)
static bool pwm_enable_leg_2 = false;            //[bool] state of the PWM (ctrl task)

/* Measurement  variables */

float32_t V1_low_value;
float32_t V2_low_value;
float32_t I1_low_value;
float32_t I2_low_value;
float32_t I_high_value;
float32_t V_high_value;

float32_t T1_value;
float32_t T2_value;

 float32_t delta_V1;
 float32_t V1_max = 0.0;
 float32_t V1_min = 0.0;
 float32_t delta_V2;
 float32_t V2_max = 0.0;
 float32_t V2_min = 0.0;

int8_t AppTask_num, CommTask_num;

static float32_t acquisition_moment = 0.06;

static float meas_data; // temp storage meas value (ctrl task)

float32_t starting_duty_cycle = 0.1;

float32_t duty_cycle = 0.3;
extern bool enable_acq;
static float32_t trig_ratio;
static float32_t begin_trig_ratio = 0.05;
static float32_t end_trig_ratio = 0.95;
 uint32_t num_trig_ratio_point = 1024;
//static float32_t voltage_reference = 5.0; //voltage reference

static float32_t kp = 0.000215;
static float32_t Ti = 7.5175e-5;
static float32_t Td = 0.0;
static float32_t N = 0.0;
static float32_t upper_bound = 1.0F;
static float32_t lower_bound = 0.0F;
static float32_t Ts = control_task_period * 1e-6;
static PidParams pid_params(Ts, kp, Ti, Td, N, lower_bound, upper_bound);

static Pid pid1;
static Pid pid2;

const uint16_t NB_DATAS = 2048;
const float32_t minimal_step = 1.0F / (float32_t) NB_DATAS;
static uint16_t number_of_cycle = 2;
static ScopeMimicry scope(NB_DATAS, 7);
extern bool is_downloading;

static uint32_t counter = 0;
static uint32_t print_counter = 0;

static float32_t local_analog_value=0;

//---------------------------------------------------------------

enum serial_interface_menu_mode // LIST OF POSSIBLE MODES FOR THE OWNTECH CONVERTER
{
    IDLEMODE = 0,
    POWERMODE
};

uint8_t mode_scope = IDLEMODE;

// trigger function for scope manager
bool a_trigger() {
    return enable_acq;
}

void dump_scope_datas(ScopeMimicry &scope)  {
    uint8_t *buffer_scope = scope.get_buffer();
    uint16_t buffer_size = scope.get_buffer_size() >> 2; // we divide by 4 (4 bytes per float data)
    printk("begin record\n");
    printk("#");
    for (uint16_t k=0;k < scope.get_nb_channel(); k++) {
        printk("%s,", scope.get_channel_name(k));
    }
    printk("\n");
    printk("# %d\n", scope.get_final_idx());
    for (uint16_t k=0;k < buffer_size; k++) {
        printk("%08x\n", *((uint32_t *)buffer_scope + k));
        task.suspendBackgroundUs(100);
    }
    printk("end record\n");
}

//---------------SETUP FUNCTIONS----------------------------------

void setup_routine()
{
    shield.sensors.enableDefaultTwistSensors();

    shield.power.initBuck(LEG1);
    shield.power.initBuck(LEG2);

    // communication.sync.initSlave(); // start the synchronisation
    // // data.enableAcquisition(2, 35); // enable the analog measurement
    // // data.triggerAcquisition(2);     // starts the analog measurement
    // communication.can.setCanNodeAddr(CAN_SLAVE_ADDR);
    // communication.can.setBroadcastPeriod(10);
    // communication.can.setControlPeriod(10);

    scope.connectChannel(I1_low_value, "I1_low");
    scope.connectChannel(V1_low_value, "V1_low");
    scope.connectChannel(I2_low_value, "I2_low");
    scope.connectChannel(V2_low_value, "V2_low");
    scope.connectChannel(duty_cycle, "duty_cycle");
    scope.connectChannel(V_high_value, "V_high");
    scope.connectChannel(trig_ratio, "trig_ratio");
    scope.set_trigger(&a_trigger);
    scope.set_delay(0.0F);
    scope.start();

    trig_ratio = 0.05;



    AppTask_num = task.createBackground(loop_application_task);
    CommTask_num = task.createBackground(loop_communication_task);
    task.createCritical(&loop_control_task, control_task_period);

    pid1.init(pid_params);
    pid2.init(pid_params);

    task.startBackground(AppTask_num);
    task.startBackground(CommTask_num);
    task.startCritical();

    communication.rs485.configureCustom(buffer_tx, buffer_rx, sizeof(ConsigneStruct_t), slave_reception_function, 10625000, true); // custom configuration for RS485

}

//---------------LOOP FUNCTIONS----------------------------------

void loop_communication_task()
{
    received_char = console_getchar();
    initial_handle(received_char);
}

void loop_application_task()
{
    switch(mode)
    {
        case IDLE:    // IDLE MODE - turns data emission off
            if (is_downloading) {
                dump_scope_datas(scope);
                is_downloading = false;
            } else {
            scope.has_trigged();
            // printk("% 7d:", scope.has_trigged());
            // printk("% 7.2f:", (double)duty_cycle);
            // printk("% 7d:", num_trig_ratio_point);
            // printk("% 7.2f:", (double)V_high_value);
            // printk("% 7.2f\n", (double)V1_low_value);
            }
            spin.led.turnOff();
            if(!print_done) {
                printk("IDLE \n");
                print_done = true;
            }
            break;
        case POWER_OFF:  // POWER_OFF MODE - turns the power off but broadcasts the system state data
            spin.led.toggle();
            if(!print_done) {
                printk("POWER OFF \n");
                print_done = true;
            }
            frame_POWER_OFF();
            break;
        case POWER_ON:   // POWER_ON MODE - turns the system on and broadcasts measurement from the physical variables
            spin.led.turnOn();
            if(!print_done) {
                printk("POWER ON \n");
                print_done = true;
            }
            frame_POWER_ON();
            break;
        default:
            break;
    }

     task.suspendBackgroundMs(100);
}


void loop_control_task()
{
    // ------------- GET SENSOR MEASUREMENTS ---------------------
    meas_data = shield.sensors.getLatestValue(V1_LOW);
    if (meas_data != NO_VALUE)
        V1_low_value = meas_data;

    meas_data = shield.sensors.getLatestValue(V2_LOW);
    if (meas_data != NO_VALUE)
        V2_low_value = meas_data;

    meas_data = shield.sensors.getLatestValue(V_HIGH);
    if (meas_data != NO_VALUE)
        V_high_value = meas_data;

    meas_data = shield.sensors.getLatestValue(I1_LOW);
    if (meas_data != NO_VALUE)
        I1_low_value = meas_data;

    meas_data = shield.sensors.getLatestValue(I2_LOW);
    if (meas_data != NO_VALUE)
        I2_low_value = meas_data;

    meas_data = shield.sensors.getLatestValue(I_HIGH);
    if (meas_data != NO_VALUE)
        I_high_value = meas_data;

    //----------- DEPLOYS MODES----------------
    switch(mode){
        case IDLE:         // IDLE and POWER_OFF modes turn the power off
        case POWER_OFF:
            shield.power.stop(LEG1);
            shield.power.stop(LEG2);
            pwm_enable_leg_1 = false;
            pwm_enable_leg_2 = false;
            V1_max  = 0;
            V2_max  = 0;
            break;

        case POWER_ON:     // POWER_ON mode turns the power ON

            scope.acquire();

            //Tests if the legs were turned off and does it only once ]
            if(!pwm_enable_leg_1 && power_leg_settings[LEG1].settings[BOOL_LEG]) {shield.power.start(LEG1); pwm_enable_leg_1 = true;}
            if(!pwm_enable_leg_2 && power_leg_settings[LEG2].settings[BOOL_LEG]) {shield.power.start(LEG2); pwm_enable_leg_2 = true;}

            //Tests if the legs were turned on and does it only once ]
            if(pwm_enable_leg_1 && !power_leg_settings[LEG1].settings[BOOL_LEG]) {shield.power.stop(LEG1); pwm_enable_leg_1 = false;}
            if(pwm_enable_leg_2 && !power_leg_settings[LEG2].settings[BOOL_LEG]) {shield.power.stop(LEG2); pwm_enable_leg_2 = false;}

            //calls the pid calculation if the converter in either in mode buck or boost for a given dynamically set reference value
            if(power_leg_settings[LEG1].settings[BOOL_BUCK] || power_leg_settings[LEG1].settings[BOOL_BOOST]){
                power_leg_settings[LEG1].duty_cycle = pid1.calculateWithReturn(power_leg_settings[LEG1].reference_value , *power_leg_settings[LEG1].tracking_variable);
            }

            if(power_leg_settings[LEG2].settings[BOOL_BUCK] || power_leg_settings[LEG2].settings[BOOL_BOOST]){
                power_leg_settings[LEG2].duty_cycle = pid2.calculateWithReturn(power_leg_settings[LEG2].reference_value , *power_leg_settings[LEG2].tracking_variable);
            }

            if(power_leg_settings[LEG1].settings[BOOL_LEG]){
                if(power_leg_settings[LEG1].settings[BOOL_BOOST]){
                    shield.power.setDutyCycle(LEG1, (1-power_leg_settings[LEG1].duty_cycle) ); //inverses the convention of the leg in case of changing from buck to boost
                } else {
                    shield.power.setDutyCycle(LEG1, power_leg_settings[LEG1].duty_cycle ); //uses the normal convention by default
                }
            }

            if(power_leg_settings[LEG2].settings[BOOL_LEG]){
                if(power_leg_settings[LEG2].settings[BOOL_BOOST]){
                    shield.power.setDutyCycle(LEG2, (1-power_leg_settings[LEG2].duty_cycle) ); //inverses the convention of the leg in case of changing from buck to boost
                }else{
                    shield.power.setDutyCycle(LEG2, power_leg_settings[LEG2].duty_cycle); //uses the normal convention by default
                }
            }

            if(V1_low_value>V1_max) V1_max = V1_low_value;  //gets the maximum V1 voltage value. This is used for the capacitor test
            if(V2_low_value>V2_max) V2_max = V2_low_value;  //gets the maximum V2 voltage value. This is used for the capacitor test

            break;
        default:
            break;
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