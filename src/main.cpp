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

//--------------OWNTECH APIs----------------------------------
#include "SpinAPI.h"
#include "TaskAPI.h"
#include "ShieldAPI.h"
#include "pid.h"
#include "comm_protocol.h"
#include "CommunicationAPI.h"

#define RECORD_SIZE 128 // Number of point to record

#define PHASE_GPIO 9
#define PWM_GPIO 9


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

int16_t phase_shift_max = 360;
int16_t phase_shift_min = 0;

uint16_t dead_time_max = 2000;
uint16_t dead_time_min = 100;

uint8_t phase_gpio_counter = 0;
uint8_t pwm_gpio_counter = 0;

int8_t AppTask_num, CommTask_num;

static float32_t acquisition_moment = 0.06;

static float meas_data; // temp storage meas value (ctrl task)

float32_t starting_duty_cycle = 0.1;

static float32_t duty_cycle_old = 0.1;
static float32_t duty_cycle_new = 0.1;

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

#ifdef CONFIG_SHIELD_OWNVERTER
static bool pwm_enable_leg_3 = false;            //[bool] state of the PWM (ctrl task)
float32_t V3_low_value;
float32_t I3_low_value;
float32_t T3_value;

float32_t delta_V3;
float32_t V3_max = 0.0;
float32_t V3_min = 0.0;
static Pid pid3;
#endif

static uint32_t counter = 0;
static uint32_t temp_meas_internal = 10;
static int16_t phase_shift_old;
static int16_t phase_shift_new;

static float32_t local_analog_value=0;

//---------------SETUP FUNCTIONS----------------------------------

void setup_routine()
{
    shield.power.initBuck(LEG1);
    shield.power.initBuck(LEG2);

    shield.power.setDutyCycleMin(0.0);
    shield.power.setDutyCycleMax(1.0);

    spin.gpio.configurePin(PHASE_GPIO,OUTPUT);
    // spin.gpio.configurePin(PWM_GPIO,OUTPUT);


#ifdef CONFIG_SHIELD_TWIST
    shield.sensors.enableDefaultTwistSensors();
#endif


    AppTask_num = task.createBackground(loop_application_task);
    CommTask_num = task.createBackground(loop_communication_task);
    task.createCritical(&loop_control_task, control_task_period);

    pid1.init(pid_params);
    pid2.init(pid_params);

    // communication.analog.init();
    // communication.sync.initSlave();
    // communication.rs485.configure(buffer_tx, buffer_rx, sizeof(ConsigneStruct_t), slave_reception_function); // custom configuration for RS485
    // communication.can.setCanNodeAddr(CAN_SLAVE_ADDR);
    // communication.can.setCanNodeAddr
    // communication.can.setBroadcastPeriod(10);
    // communication.can.setControlPeriod(10);


    task.startBackground(AppTask_num);
    task.startBackground(CommTask_num);
    task.startCritical();

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

#ifdef CONFIG_SHIELD_OWNVERTER
            meas_data = shield.sensors.getLatestValue(TEMP_SENSOR);

            counter++;
            if(counter == temp_meas_internal){
                shield.sensors.setOwnverterTempMeas(TEMP_1);
                if (meas_data != NO_VALUE) T3_value = meas_data;
            } else if(counter == 2*temp_meas_internal){
                shield.sensors.setOwnverterTempMeas(TEMP_2);
                if (meas_data != NO_VALUE) T1_value = meas_data;
            } else if(counter == 3*temp_meas_internal){
                shield.sensors.setOwnverterTempMeas(TEMP_3);
                if (meas_data != NO_VALUE) T2_value = meas_data;
                counter = 0;
            }
#endif

#ifdef CONFIG_SHIELD_TWIST

            counter++;
            if(counter == temp_meas_internal){
                shield.sensors.triggerTwistTempMeas(TEMP_SENSOR_1);
                meas_data = shield.sensors.getLatestValue(TEMP_SENSOR_2);
                if (meas_data != NO_VALUE) T2_value = meas_data;
            } else if(counter == 2*temp_meas_internal){
                shield.sensors.triggerTwistTempMeas(TEMP_SENSOR_2);
                meas_data = shield.sensors.getLatestValue(TEMP_SENSOR_1);
                if (meas_data != NO_VALUE) T1_value = meas_data;
                counter = 0;
            }
#endif
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

#ifdef CONFIG_SHIELD_OWNVERTER
    meas_data = shield.sensors.getLatestValue(V3_LOW);
    if (meas_data != NO_VALUE)
        V3_low_value = meas_data;

    meas_data = shield.sensors.getLatestValue(I3_LOW);
    if (meas_data != NO_VALUE)
        I3_low_value = meas_data;
#endif


    // local_analog_value = communication.analog.getAnalogCommValue();
    // analog_value = (uint16_t)local_analog_value;

    // ctrl_slave_counter++; //counter for the slave function

    // can_test_ctrl_enable = communication.can.getCtrlEnable();
    // can_test_reference_value = communication.can.getCtrlReference();



    //----------- DEPLOYS MODES----------------
    switch(mode){
        case IDLE:         // IDLE and POWER_OFF modes turn the power off
        case POWER_OFF:
            shield.power.stop(LEG1);
            pwm_enable_leg_1 = false;
            V1_max  = 0;

            shield.power.stop(LEG2);
            pwm_enable_leg_2 = false;
            V2_max  = 0;

#ifdef CONFIG_SHIELD_OWNVERTER
            shield.power.stop(LEG3);
            pwm_enable_leg_3 = false;
            V3_max  = 0;
#endif
            break;

        case POWER_ON:     // POWER_ON mode turns the power ON
            phase_shift_new = power_leg_settings[LEG2].phase_shift;

            if(phase_gpio_counter>0) {
                phase_gpio_counter++;             // counts until we reach a certain number
                if(phase_gpio_counter>21) {
                    phase_gpio_counter=0;  //sets the counter to zero and stops counting
                    spin.gpio.resetPin(PHASE_GPIO);
                }
            }

            if( phase_shift_new != phase_shift_old){
                spin.gpio.setPin(PHASE_GPIO);
                phase_gpio_counter++;             // counts until we reach a certain number
            } 

            phase_shift_old = phase_shift_new;

            //Tests if the legs were turned off and does it only once ]
            if(!pwm_enable_leg_1 && power_leg_settings[LEG1].settings[BOOL_LEG]) {shield.power.start(LEG1); pwm_enable_leg_1 = true;}
            if(!pwm_enable_leg_2 && power_leg_settings[LEG2].settings[BOOL_LEG]) {shield.power.start(LEG2); pwm_enable_leg_2 = true;}

#ifdef CONFIG_SHIELD_OWNVERTER
            if(!pwm_enable_leg_3 && power_leg_settings[LEG3].settings[BOOL_LEG]) {shield.power.start(LEG3); pwm_enable_leg_3 = true;}
#endif

            //Tests if the legs were turned on and does it only once ]
            if(pwm_enable_leg_1 && !power_leg_settings[LEG1].settings[BOOL_LEG]) {shield.power.stop(LEG1); pwm_enable_leg_1 = false;}
            if(pwm_enable_leg_2 && !power_leg_settings[LEG2].settings[BOOL_LEG]) {shield.power.stop(LEG2); pwm_enable_leg_2 = false;}

#ifdef CONFIG_SHIELD_OWNVERTER
            if(pwm_enable_leg_3 && !power_leg_settings[LEG3].settings[BOOL_LEG]) {shield.power.stop(LEG3); pwm_enable_leg_3 = false;}
#endif

            //calls the pid calculation if the converter in either in mode buck or boost for a given dynamically set reference value
            if(power_leg_settings[LEG1].settings[BOOL_BUCK] || power_leg_settings[LEG1].settings[BOOL_BOOST]){
                power_leg_settings[LEG1].duty_cycle = pid1.calculateWithReturn(power_leg_settings[LEG1].reference_value , *power_leg_settings[LEG1].tracking_variable);
            }

            if(power_leg_settings[LEG2].settings[BOOL_BUCK] || power_leg_settings[LEG2].settings[BOOL_BOOST]){
                power_leg_settings[LEG2].duty_cycle = pid2.calculateWithReturn(power_leg_settings[LEG2].reference_value , *power_leg_settings[LEG2].tracking_variable);
            }

#ifdef CONFIG_SHIELD_OWNVERTER
            if(power_leg_settings[LEG3].settings[BOOL_BUCK] || power_leg_settings[LEG3].settings[BOOL_BOOST]){
                power_leg_settings[LEG3].duty_cycle = pid2.calculateWithReturn(power_leg_settings[LEG3].reference_value , *power_leg_settings[LEG3].tracking_variable);
            }
#endif


            if(power_leg_settings[LEG1].settings[BOOL_LEG]){


                if(power_leg_settings[LEG1].settings[BOOL_BOOST]){
                    shield.power.setDutyCycle(LEG1, (1-power_leg_settings[LEG1].duty_cycle) ); //inverses the convention of the leg in case of changing from buck to boost
                } else {
                    shield.power.setDutyCycle(LEG1, power_leg_settings[LEG1].duty_cycle ); //uses the normal convention by default
                }
            }

            if(power_leg_settings[LEG2].settings[BOOL_LEG]){

                duty_cycle_new = power_leg_settings[LEG2].duty_cycle;

                if(pwm_gpio_counter>0) {
                    pwm_gpio_counter++;             // counts until we reach a certain number
                    if(pwm_gpio_counter>21) {
                        spin.gpio.resetPin(PWM_GPIO);
                        pwm_gpio_counter =0 ; //sets the counter to zero and stops counting
                    }
                }

                if(duty_cycle_new != duty_cycle_old){
                    spin.gpio.setPin(PWM_GPIO);
                    pwm_gpio_counter++;             // turns the gpio on
                } 

                duty_cycle_old = duty_cycle_new;

                if(power_leg_settings[LEG2].settings[BOOL_BOOST]){
                    shield.power.setDutyCycle(LEG2, (1-power_leg_settings[LEG2].duty_cycle) ); //inverses the convention of the leg in case of changing from buck to boost
                }else{
                    shield.power.setDutyCycle(LEG2, power_leg_settings[LEG2].duty_cycle); //uses the normal convention by default
                }



            }

#ifdef CONFIG_SHIELD_OWNVERTER
            if(power_leg_settings[LEG3].settings[BOOL_LEG]){
                if(power_leg_settings[LEG3].settings[BOOL_BOOST]){
                    shield.power.setDutyCycle(LEG3, (1-power_leg_settings[LEG3].duty_cycle) ); //inverses the convention of the leg in case of changing from buck to boost
                }else{
                    shield.power.setDutyCycle(LEG3, power_leg_settings[LEG3].duty_cycle); //uses the normal convention by default
                }
            }
#endif


            if(V1_low_value>V1_max) V1_max = V1_low_value;  //gets the maximum V1 voltage value. This is used for the capacitor test
            if(V2_low_value>V2_max) V2_max = V2_low_value;  //gets the maximum V2 voltage value. This is used for the capacitor test

#ifdef CONFIG_SHIELD_OWNVERTER
            if(V3_low_value>V3_max) V3_max = V3_low_value;  //gets the maximum V2 voltage value. This is used for the capacitor test
#endif

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