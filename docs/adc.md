!!! note ""
    An ADC, or Analog-to-Digital Converter, is a device that turns analog signals, like sound or temperature, into digital data that a computer or microcontroller can understand. It measures the analog voltage and converts it into a digital value that can be used by digital systems for processing, storage, or display.

!!! warning
    The ADC API is an advanced interface provided for users with technical background.
    For most data acquisition tasks, use the [Data API](dataAPI.md) which provides a more user-friendly and higher-level entry point.

## Pinout

SPIN boards have 5 independant ADC units. Each unit can measure multiple analog signals, using a measurment sequence.

!!! abstract "Capabilities"
    Each ADC channel has :
    -  12 bit resolution: 0b to 4096b
    -  Sensing full scale: 0V to 2.048V
    -  Sampling time down to 42ns

=== "SPIN v1.1.0"
    ![SPIN_1_1_0_ADC_pinout_main](images/SPIN_1.1.0_main_adc.png){ width=451 align=left }
    ![SPIN_1_1_0_ADC_pinout_inner](images/SPIN_1.1.0_inner_adc.png){ width=243 }

=== "SPIN v1.0.0"
    ![SPIN_1_0_0_ADC_pinout_main](images/SPIN_1.0.0_main_adc.png){ width=451 align=left }
    ![SPIN_1_0_0_ADC_pinout_inner](images/SPIN_1.0.0_inner_adc.png){ width=243 }


!!! info
    -  ADC12 means that the pin can be used either as ADC1 or ADC2
    -  INx means that it is channel x.

## Include

!!! note
    ```
    #include <SpinAPI.h>
    ```
    Make sure that SPIN API is included to use AdcHAL

## Initialization sequence

!!! Note

    === "Software triggered"
        1. Link an adc trigger event to the ADC [`spin.adc.configureTriggerSource(ADCx, TRIG)`](https://owntech-foundation.github.io/Documentation/powerAPI/classAdcHAL/#function-configuretriggersource)
        2.  Define acquisition sequence by enabling adc channel : [`spin.adc.enableChannel(ADCx, channelx)`](https://owntech-foundation.github.io/Documentation/powerAPI/classAdcHAL/#function-enablechannel)
        3.  trigger an adc [`data.triggerAcquisition()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-triggeracquisition)
        4.  Retrieve value : [`data.getLatest(ADCx, pinx)`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getlatest-22)

    === "Hardware triggered"
        1\. [Make sure PWM engine is initialized](pwm/#initialization-sequence)
        2. Link an adc trigger event to the ADC [`spin.adc.configureTriggerSource(ADCx, TRIG)`](https://owntech-foundation.github.io/Documentation/powerAPI/classAdcHAL/#function-configuretriggersource)
        3. Set continuous/discontinuous conversion mode. Optional : [`spin.adc.configureDiscontinuousMode(x, 0/1)`](https://owntech-foundation.github.io/Documentation/powerAPI/classAdcHAL/#function-configurediscontinuousmode)
        4.  Define acquisition sequence by enabling adc channel : [`spin.adc.enableChannel(ADCx, channelx)`](https://owntech-foundation.github.io/Documentation/powerAPI/classAdcHAL/#function-enablechannel)
        6. Start data dispatching to get acquired values [`data.start()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-start)
        7.  Retrieve value : [`data.getLatest(ADCx, pinx)`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getlatest-22)

!!! example

    === "Software triggered"
        ```cpp
        spin.adc.configureTriggerSource(1, software);
        spin.adc.enableChannel(1, 30);
        data.triggerAcquisition(1);
        float32_t adc_value = 	data.getLatest(1, 30);
        ```
    === "Hardware triggered"
        ```cpp
        /* PWM unit initialization */
        spin.pwm.setModulation(PWMA, UpDwn);
        spin.pwm.setAdcEdgeTrigger(PWMA, EdgeTrigger_up);
        spin.pwm.setAdcDecimation(PWMA, 1);
        spin.pwm.setMode(PWMA, VOLTAGE_MODE);
        spin.pwm.initUnit(PWMA);
        spin.pwm.setDeadTime(PWMA, 200,200);
        spin.pwm.setAdcTrigger(PWMA, ADCTRIG_1);
        spin.pwm.enableAdcTrigger(PWMA);
        spin.pwm.setDutyCycle(0.5);
        spin.pwm.startDualOutput(PWMA);

        /* ADC initialization */
        spin.adc.configureTriggerSource(1, hrtim_eev1);
        spin.adc.configureDiscontinuousMode(1,1);
        spin.adc.enableChannel(1, 30);
        data.start();
        float32_t adc_value = 	data.getLatest(1, 30);
        ```

## Channel sequence

Each ADC unit can measure multiple analog signal. This works by defining an acquisition sequence.

!!! tip
    By default the aquisition sequence is in continuous mode. It means than one trigger will trigger all the sequence of acquisition.
    This can be changed using [Discontinuous Mode](#continuous-discontinuous-sequence)

!!! example
    === "3 channels on ADC1"
    ```c++
    spin.adc.enableChannel(1, 2)
    spin.adc.enableChannel(1, 3)
    spin.adc.enableChannel(1, 1)
    ```
    In this example, for each trigger action, the ADC1 will measure channel 2, then channel 3, then channel 1.

    === "Single channel"
    ```spin.adc.enableChannel(1, 1)```

    In this example, for each trigger, the ADC1 will measure  channel 1.

!!! note
    Sequence order is given by `spin.adc.enableChannel()` order.

## Software triggered

=== " "
    ![adc software triggers](images/ADC_software_trigger.svg){ width=300 align=left }

    A software trigger for an ADC is a command or signal generated by the software to start the ADC conversion process. Instead of using a hardware signal or event to initiate the conversion, the ADC is triggered by software instructions, allowing for precise control and synchronization with other software processes or events. For example this trigger command can be generated from a high speed control task.

    ```C++
    spin.adc.configureTriggerSource(2, software); // ADC 2 configured in software mode
    data.triggerAcquisition(2); // Send an adc trigger to ADC2 to start conversion
    adc_value = data.getLatest(2, 35); // Get the acquired data
    ```

    !!! example
        See the following example for an application :

        - [Software trigerred ADC](https://owntech-foundation.github.io/Documentation/examples/SPIN/ADC/adc_software_trigger/)

## Synchronous with PWMs

Software triggers is generally speaking not suitable for applications that requires precise timing.
Sometimes measurement have to be taken at a specific time, in interaction with the control signal.

!!! info
    This kind of acquisition have a side benefit. It operates in the background and does not interfer with software.
    It means that periodical sampling will not interrupt code execution.

The trigger is the PWM peripheral, the ADC performs the conversion and place the result in an output buffer, the output buffer is directly placed in memory by a DMA. A rather complex software mechanics execute periodically and serve each measurement in a dedicated memory space. The periodical routine is executed at the beggining of the controlTask.

!!! info
    Synchronous measurement require having a controlTask running.

!!! tip
    This page informs on how to parameters the triggering of the measurement
    To retrieve measurement values please refer to DataAPI

The section below explain how to synchronize measurements with control signals.

!!! note
    These features are available for ADC1 and ADC2.

### The different event and trigger of ADC/HRTIM

You might have seen that there are differents variables referencing adc trigger : 

```cpp
spin.adc.configureTriggerSource(1, hrtim_eev1);
spin.pwm.setAdcTrigger(PWMA, ADCTRIG_1);
```
There is `hrtim_eevx` or `ADCTRIG_x`, let's see the role of each of them.

#### ADCTRIG_x

In the PWM API, four signals can serve as ADC triggers, initiating ADC conversions: ADC_TRIG1, ADC_TRIG2, ADC_TRIG3, and ADC_TRIG4.

The PWM is produced by a carrier (see [here](https://owntech-foundation.github.io/Documentation/core/docs/pwm/)) by using a comparator to compare a specific constant value with the carrier, we can generate an event when enabling a trigger on the PWM. For instance :  

```c++
spin.pwm.setAdcTrigger(PWMA, ADCTRIG_1);
spin.pwm.enableAdcTrigger(PWMA);
```

We are enabling and linking ADCTRIG_1 to PWMA. That means that when the comparator value and the carrier value of the PWMA we can generate an event from ADCTRIG_1 that can be used to start an ADC conversion : 

![ADC trigger](images/hrtim_adc_trigger_schema.svg)

The comparator value can be adjusted using the function `spin.pwm.setAdcTriggerInstant(PWMA, 0.5)`. For example, setting it to 0.5 means triggering an event halfway through the switching period, initiating an ADC conversion.

#### hrtim_eevx

An ADC conversion can be initiated either by software (via a function) or hardware through an external event. Numerous external events exist, but some are specifically related to PWM: `hrtim_eev1`, `hrtim_eev2`, `hrtim_eev3`, and `hrtim_eev4`.

Each of these external events is associated with events generated by ADCTRIG_x. For instance, hrtim_eev1 is linked to ADCTRIG_1, and so on.

```cpp
spin.pwm.setAdcTrigger(PWMA, ADCTRIG_1);
spin.pwm.enableAdcTrigger(PWMA);
spin.adc.configureTriggerSource(1, hrtim_eev1);
```
The above code indicates that we've connected ADC**1** to the external event **hrtim_eev1**, which, in turn, is connected to the event generated by **ADCTRIG1** when the comparator (set by the trigger value) matches the carrier of **PWMA**.

### Source ADC and PWM channel

ADC1 and ADC2 can be associated with PWM channels at will.

::: doxy.powerAPI.class.method
name: AdcHAL
method: void configureDiscontinuousMode (uint8_t adc_number, uint32_t dicontinuous_count)

=== "ADC1 and PWMA"
    ![left_aligned_ADC_50](images/left_aligned_ADC_50.svg){ width=800 }

    ```c++
        spin.pwm.setAdcTrigger(PWMA, ADCTRIG_1);
        spin.pwm.enableAdcTrigger(PWMA);
    ```

=== "ADC1 and PWMC"
    ![left_aligned_ADC_PWM_C](images/left_aligned_ADC_PWM_C.svg){ width=800 }

    ```c++
        spin.pwm.setAdcTrigger(PWMC, ADCTRIG_1);
        spin.pwm.enableAdcTrigger(PWMC);
    ```

### Modulation impact

!!! note
    Hardware trigger depends on PWM modulation.

=== "Left Aligned Modulation"
    ![left_aligned_ADC_50](images/left_aligned_ADC_50.svg){ width=800 }

    ```c++
        spin.pwm.setAdcTrigger(PWMA, ADCTRIG_1);
        spin.pwm.enableAdcTrigger(PWMA);
    ```

=== "Center aligned modulation"
    ![center_aligned_ADC_rising_edge](images/center_aligned_ADC_rising_edge.svg){ width=800 }

    ```c++
        spin.pwm.setAdcTrigger(PWMA, ADCTRIG_1);
        spin.pwm.enableAdcTrigger(PWMA);
    ```

### Rising Edge / Falling edge

!!! note
    This only applies to center aligned modulation.

=== "Rising Edge"
    ![center_aligned_ADC_rising_edge](images/center_aligned_ADC_rising_edge.svg){ width=800 }

    ```c++
        spin.pwm.setAdcEdgeTrigger(PWMA, EdgeTrigger_up);
        spin.pwm.setAdcTrigger(PWMA, ADCTRIG_1);
        spin.pwm.enableAdcTrigger(PWMA);
    ```

=== "Falling Edge"
    ![center_aligned_ADC_falling_edge](images/center_aligned_ADC_falling_edge.svg){ width=800 }


    ```c++
        spin.pwm.setAdcEdgeTrigger(PWMA, EdgeTrigger_down);
        spin.pwm.setAdcTrigger(PWMA, ADCTRIG_1);
        spin.pwm.enableAdcTrigger(PWMA);
    ```


### Measurment trigger instant

Trigger timing can be defined or changed dynamically.

=== "20% Duty cycle"
    ![left_aligned_ADC_20](images/left_aligned_ADC_20.svg){ width=800 }

    ```c++
        spin.pwm.setAdcTriggerInstant(PWMA, 0.2);

    ```

=== "80% Duty cycle"
    ![left_aligned_ADC_80](images/left_aligned_ADC_80.svg){ width=800 }

    ```c++
        spin.pwm.setAdcTriggerInstant(PWMA, 0.8);
    ```

### Continuous / Discontinuous sequence

!!! note
    This is relevant if more than one measurement is taken with the same ADC.

=== "Continuous sequence"
    ![left_aligned_continuous_sampling](images/left_aligned_continuous_sampling.svg){ width=800 }

    ```c++
        spin.adc.configureDiscontinuousMode(1, 0);
    ```

=== "Discontinuous sequence "
    ![left_aligned_ADC_50](images/left_aligned_ADC_50.svg){ width=800 }

    ```c++
        spin.adc.configureDiscontinuousMode(1, 1);
    ```


### Number of PWM period between acquisition

!!! note
    This is relevant if more than one measurement is taken with the same ADC.

=== "With 2 periods"
    ![sparse_sampling_2](images/sparse_sampling_2.svg){ width=800 }

    ```c++
        spin.pwm.setAdcTriggerPostScaler(PWMA, 2);
    ```

=== "With 4 periods"
    ![sparse_sampling_4](images/sparse_sampling_4.svg){ width=800 }

    ```c++
        spin.pwm.setAdcTriggerPostScaler(PWMA, 4);
    ```

=== "Without interuptions"
    ![left_aligned_continuous_sampling](images/left_aligned_continuous_sampling.svg){ width=800 }

    ```c++
        spin.pwm.setAdcTriggerPostScaler(PWMA, 1);
    ```
!!! example
    see the following example for an application :

    - [PWM trigerred ADC](https://owntech-foundation.github.io/Documentation/examples/SPIN/ADC/adc_hrtim_trigger/)

## API Reference

::: doxy.powerAPI.class
name: AdcHAL
