!!! note ""
    Data acquisition is the process of sampling signals that measure real-world physical conditions and converting the resulting samples into digital numeric values that can be manipulated by a computer.

    In the powerAPI context, the DataAPI helps the user retrieve the values sampled by the [ADC](adc.md)

## Include

!!! note 
    ```
    #include <DataAPI.h>
    ```
    Make sure that Data API is included

## Initialization sequence 

&emsp; -**If hardware triggered** :  
    &emsp; &emsp; 1\. [Make sure PWM engine is initialized](pwm/#initialization-sequence)   
2.  [Make sure ADC is initialized](adc/#initialization-sequence)   
3.  Define acquisition conversion parameter: [`data.setParameters()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-setparameters-22)       
&emsp; **-If Hardware triggered** :  
    &emsp; &emsp; 4.  start data dispatching [`data.start()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-start)  
&emsp; **-If Software triggered** :  
    &emsp; &emsp; 5. Trigger an initial adc conversion [`data.triggerAcquisition(ADCx)`](https://owntech-foundation.github.io/Documentation/powerAPI/classAdcHAL/#function-enablechannel)   
6.  Retrieve values : [`data.getLatest()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getlatest-12) or [`data.getRawValues()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getrawvalues-12)  
`
!!! Note

    === "Software triggered" 
        1.  [Make sure ADC is initialized](https://owntech-foundation.github.io/Documentation/core/docs/adc/)   
        2.  Define acquisition conversion parameter: [`data.setParameters()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-setparameters-22)       
        3. Trigger an initial adc conversion [`data.triggerAcquisition(ADCx)`](https://owntech-foundation.github.io/Documentation/powerAPI/classAdcHAL/#function-enablechannel)   
        4.  Retrieve values : [`data.getLatest()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getlatest-12) or [`data.getRawValues()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getrawvalues-12)  

    === "Hardware trigerred"
        1\. [Make sure PWM engine is initialized](https://owntech-foundation.github.io/Documentation/core/docs/pwm/)   
        2.  [Make sure ADC is initialized](https://owntech-foundation.github.io/Documentation/core/docs/adc/)   
        3.  Define acquisition conversion parameter: [`data.setParameters()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-setparameters-22)       
        4.  start data dispatching [`data.start()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-start)  
        5.  Retrieve values : [`data.getLatest()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getlatest-12) or [`data.getRawValues()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getrawvalues-12)  

!!! example 
    
    === "Software triggered ADC"
        ```cpp
        spin.adc.configureTriggerSource(1, software);
        data.triggerAcquisition(1);
        spin.adc.enableChannel(1, 30);
        float32_t adc_value = 	data.getLatest(1, 30);
        ```
    === "Hardware triggered ADC"
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

## Retrieving last value

Getting the last measured value to feed the control algorithm is super simple. 

!!! example 
    === "Get last value"
        ``` 
            spin.data.getLatest(1,1)
        ```
        This will retrieve the last sampled value of ADC1 channel 1. 

## Getting values with the right unit

DataAPI contains commodity functions to convert the raw binary measurement values in meaningful units. 

!!! example 
    === "Converting in volts"
        ```data.setParameters()```

## Get an array of values

DataAPI contains commodity function to retrieve an array of raw values that can be fed to a [post processing filter](controlLibrary/controlLibrary/docs/use-filters).

!!! example 
    === "Retrieve 5 values"
        ```data.getRawValues()```

::: doxy.powerAPI.class
name: DataAPI