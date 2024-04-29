!!! Note ""
    This API is compatible with TWIST, with a set of functions to control the output power.

## Features

=== " "
    ![TWIST schema](images/TWIST_illustration.svg){ width=200 align=left }  
    - Different control mode : voltage and peak current mode.  
    - 2 legs can independently work with different topology : boost, buck  
    - Configure different paramaters for power electronics (dead time, phase shift)  
    - Simplified ADC value retrieval  
    - See [TWIST hardware specifications](https://github.com/owntech-foundation/TWIST) for more detail on TWIST board.

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

Voltage mode is the classic way where we control the output voltage from the duty cycle. The duty cycle in power electronics controls the power delivered to a load by regulating the proportion of time that a switch (such as a transistor) is on compared to the total switching period. A higher duty cycle means the switch is on for a greater portion of the time, delivering more power to the load. Conversely, a lower duty cycle reduces the power delivered. Thus, by adjusting the duty cycle, the average power and voltage applied to the load can be controlled effectively. 

![duty cycle](./images/changing_duty_cycle.gif)

### Current mode

In peak current mode control, the controller monitors the current flowing through the power switch. When the current reaches its peak value, the controller turns off the power switch. This helps in maintaining a constant output voltage.

Below a simple schematic example to understand how it works in a buck configuration :

![](images/current_mode_schema.svg){ width=600 }  
_Source : STM32 AN5497_

 A clock generate the moment to close the switch (it defines the switching frequency), the controller sends a reference of the peak current and when the inductors current reach this value we open the switch.


![](images/current_mode_pwm_scheme.svg){ width=600 }  
_Source : STM32 AN5497_


However having a constant peak current reference can causes subharmonic oscillations. To avoid that we use what we call **slope compensation**. The peak current reference is a sawtooth instead of a constant value. So the final schema will look like this :

![](images/current_mode_schema_final.svg){ width=600 }  
![](images/current_mode_pwm_scheme_final.svg)

The sawtooth signal `Ipeak - Slope compensation` is generated with the function [`twist.setAllSlopeCompensation`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setallslopecompensation) or [`twist.steLegSlopeCompensation`](https://owntech-foundation.github.io/Documentation/powerAPI/classTwistAPI/#function-setallslopecompensation). It will set the slope compensation so that you get the input parameter of the functions for example `twist.setAllSlopeCompensation(1.4, 1.0)` generate a sawtooth signal between 1.4V and 1.0V, you can have a sawtooth between 2.048V and  0V. This signal is then compared with the value of the current coming in the ADC, so you need to take into account the conversion of current to voltage when choosing the sawtooth parameter.


## Snippets examples

### Buck topology

![](images/twist_buck_conf.png){ width=400 }

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
