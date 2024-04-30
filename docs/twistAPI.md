!!! Note ""
    This API is designed to work with the TWIST hardware, providing a set of functions to manage and control the output power.

## Features

=== " "
    ![TWIST schema](images/TWIST_illustration.svg){ width=200 align=left }  
    - **Versatile Control Modes**: The API supports both voltage and peak current control modes, allowing you to choose the best option for your specific application.  
    - **Independent Leg Operation**: Each of the two legs can operate independently with different topologies, such as boost or buck, offering greater flexibility in power management.
    - **Configure different paramaters** for power electronics (dead time, phase shift)  
    - **Simplified ADC value retrieval**  
    - Refer to [TWIST hardware specifications](https://github.com/owntech-foundation/TWIST) for more detail on TWIST board.

## Initialization sequence 

!!! note 
    === "Voltage mode with all the legs"

        1\. Set the version of the board [`twist.setVersion(shield_TWIST_VERSION)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setversion)    
        2. Choose the wished topology : buck, boost. You can select all the legs to be in the same topology or choose a specific configuration for each one [`twist.initAllBuck()`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-initallbuck), [`twist.initAllBoost()`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-initallboost)  
        3. [OPTIONAL] Set the adc decimation to divide the number of trigger event starting the adc conversion [`twist.setAllAdcDecim(decim)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setalladcdecim)   
        4. [OPTONAL] Set the dead time [`twist.setAllDeadTime(rise_deadTime_ns, fall_deadTime_ns)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setalldeadtime)   
        5. [OPTIONAL] Set the phase shift in degree [`twist.setAllPhaseShift(ps_degree)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setallphaseshift)   
        6. Enable the ADC acquisition for twist to get voltage and current measures [`data.enableTwistDefaultChannels()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-enableshieldchannel)  
        7. Set the duty cycle to control output voltage [`twist.setAllDutyCycle(duty_cycle)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setalldutycycle)   
        8. Then start the converters [`twist.startAll()`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-startall)   

    === "Voltage mode with only LEG1"

        1\. Set the version of the board [`twist.setVersion(shield_TWIST_VERSION)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setversion)    
        2. Choose the wished topology : buck, boost [`twist.initLegBuck(LEG1/LEG2)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-initlegbuck), [`twist.initLegBoost(LEG1/LEG2)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-initlegboost)  
        3. [OPTIONAL] Set the adc decimation to divide the number of trigger event starting the adc conversion [`twist.setLegAdcDecim(LEG1/LEG2, decim)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setlegadcdecim)  
        4. [OPTONAL] Set the dead time [`twist.setLegDeadTime(LEG1/LEG2,rise_deadTime_ns, fall_deadTime_ns)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setlegdeadtime)  
        5. [OPTIONAL] Set the phase shift in degree [`twist.setLegPhaseShift(LEG1/LEG2, ps_degree)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setlegphaseshift)  
        6. Enable the ADC acquisition for twist to get voltage and current measures [`data.enableTwistDefaultChannels()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-enableshieldchannel)  
        7. Set the duty cycle to control output voltage [`twist.setLegDutyCycle(LEG1/LEG2, duty_cycle)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setlegdutycycle)  
        8. Then start the converters [`twist.startLeg(LEG1/LEG2)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-startleg)  

    === "Current mode with all the legs"

        1\. Set the version of the board [`twist.setVersion(shield_TWIST_VERSION)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setversion)    
        2. Choose the wished topology : buck, boost. You can select all the legs to be in the same topology or choose a specific configuration for each one [`twist.initAllBuck(CURRENT_MODE)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-initallbuck), [`twist.initAllBoost()`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-initallboost)  
        3. [OPTIONAL] Set the adc decimation to divide the number of trigger event starting the adc conversion [`twist.setAllAdcDecim(decim)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setalladcdecim)   
        4. [OPTONAL] Set the dead time [`twist.setAllDeadTime(rise_deadTime_ns, fall_deadTime_ns)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setalldeadtime)   
        5. [OPTIONAL] Set the phase shift in degree [`twist.setAllPhaseShift(ps_degree)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setallphaseshift)   
        6. Enable the ADC acquisition for twist to get voltage and current measures [`data.enableTwistDefaultChannels()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-enableshieldchannel)  
        7. Set the slope compensation to control the output current[`twist.setAllSlopeCompensation(1.4, 1.0)`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setallslopecompensation)   
        8. Then start the converters [`twist.startAll()`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-startall)   


!!! warning
    Only buck topology is supported for current mode control currently.

!!! example

    === "Voltage mode with all the legs"

        ```cpp
        twist.setVersion(shield_TWIST_V1_3);
        twist.initAllBuck();
        twist.setAllAdcDecim(1);
        twist.setAllDeadTime(200,200);
        twist.setAllPhaseShift(180);
        data.enableTwistDefaultChannels();
        twist.setAllDutyCycle(0.5);
        twist.startAll();
        ```
    === "Voltage mode with only LEG1"

        ```cpp
        twist.setVersion(shield_TWIST_V1_3);
        twist.initLegBuck(LEG1);
        twist.setLegAdcDecim(LEG1, 1);
        twist.setLegDeadTime(LEG1, 200,200);
        twist.setLegPhaseShift(LEG1, 180);
        data.enableTwistDefaultChannels();
        twist.setLegDutyCycle(LEG1, 0.5);
        twist.startLeg(LEG1);
        ```

    === "Current mode with all the legs"

        ```cpp
        twist.setVersion(shield_TWIST_V1_3);
        twist.initAllBuck(CURRENT_MODE);
        twist.setAllAdcDecim(1);
        twist.setAllDeadTime(200,200);
        twist.setAllPhaseShift(180);
        data.enableTwistDefaultChannels();
        twist.setAllSlopeCompensation(1.4, 1.0);
        twist.startAll();
        ```

## Voltage mode and Current mode

There is two different way to control the power delivered by TWIST : voltage and current mode. 

### Voltage mode

Voltage mode is a traditional and widely-used approach in power electronics, where the output voltage is regulated by controlling the duty cycle. The duty cycle is a crucial parameter that determines the proportion of time a switch (such as a transistor) is active during a complete switching period. By adjusting the duty cycle, you can effectively manage the power delivered to a load.

A higher duty cycle indicates that the switch remains on for a more extended period, resulting in increased power delivery to the load. Conversely, a lower duty cycle means the switch is on for a shorter duration, thereby reducing the power supplied. Consequently, the average power and voltage applied to the load can be precisely controlled by fine-tuning the duty cycle.

![duty cycle](./images/changing_duty_cycle.gif)

### Current mode

In peak current mode control,  we monitors the current flowing through the power switch. Once the current reaches a predetermined peak value, we promptly turns off the power switch. This mechanism helps maintain a constant output voltage by regulating the current flow.

Below a simple schematic example to understand how it works in a buck configuration :

![](images/current_mode_schema.svg){ width=600 }  
_Source : STM32 AN5497_

 In this setup, a clock signal determines the switching frequency and triggers the switch to close. The controller then sends a reference peak current value. When the inductor's current reaches this reference value, the switch opens.


![](images/current_mode_pwm_scheme.svg){ width=600 }  
_Source : STM32 AN5497_


However, using a constant peak current reference can lead to subharmonic oscillations. To prevent this issue, we employ a technique called slope compensation. Instead of a constant value, the peak current reference is a sawtooth waveform. The final schematic with slope compensation is shown below:

![](images/current_mode_schema_final.svg){ width=600 }  
![](images/current_mode_pwm_scheme_final.svg)

The sawtooth signal `Ipeak - Slope compensation` is generated with the function [`twist.setAllSlopeCompensation`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setallslopecompensation) or [`twist.steLegSlopeCompensation`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setallslopecompensation). These functions set the slope compensation based on the input parameters.  for example `twist.setAllSlopeCompensation(1.4, 1.0)` generates a sawtooth signal ranging from 1.4V to 1.0V. You can create a sawtooth signal between 2.048V and 0V as well.

This sawtooth signal is then compared with the ADC's current value. When selecting the sawtooth parameters, it's essential to consider the conversion of current to voltage.

On the TWIST board, a voltage value of 1.024V on the ADC corresponds to a current of 0A. The system has a gain of 100mV per ampere, meaning that for each ampere increase in current, the voltage value increases by 100 millivolts. 


## Snippets examples

### Buck topology

=== " Connection "
    ![](images/twist_buck_conf.png){ width=400 }

=== " Schematic "
    ![](images/twist_buck_schematic.svg)

```cpp
    twist.initAllBuck();
    twist.setAllDutyCycle(0.5);
    twist.startAll();
```

### Boost topology 

![](images/twist_boost_conf.png){ width=400 }

```cpp
    twist.initAllBoost();
    twist.setAllDutyCycle(0.5);
    twist.startAll();
```

### Inverter topology

![](images/twist_inverter_conf.png){ width=400 }

```cpp
    twist.initLegBuck(LEG1);
    twist.initLegBoost(LEG2);
    twist.setAllDutyCycle(0.5);
    twist.startAll();
```
!!! example

    Check the following examples for an application :  
        - [Voltage mode buck](https://owntech-foundation.github.io/Documentation/examples/TWIST/DC_DC/buck_voltage_mode/)  
        - [Current mode buck](https://owntech-foundation.github.io/Documentation/examples/TWIST/DC_DC/buck_current_mode/)  
        - [Voltage mode boost](https://owntech-foundation.github.io/Documentation/examples/TWIST/DC_DC/boost_voltage_mode/)  


::: doxy.powerAPI.class
name: TwistAPI