!!! note ""
    Data acquisition is the process of sampling signals that measure real-world physical conditions and converting the resulting samples into digital numeric values that can be manipulated by a computer.

    In the powerAPI context, the DataAPI helps the user retrieve the values sampled by the [ADC](adc.md)

## Include

!!! note 
    ```
    #include <DataAPI.h>
    ```
    Make sure that Data API is included

## Retrieving values

Getting the last measured value to feed the control algorithm is super simple. 

!!! example 
    === "Get last value"
        ``` 
            spin.data.getLatest(1,1)
        ```
        This will retrieve the last sampled value of ADC1 channel 1. 

It is also possible to retrieve a vector containing multiple values. For that we use :


## Getting values with the right unit

DataAPI contains commodity functions to convert the raw binary measurement values in meaningful units. 

!!! example 
    === "Converting in volts"

    === "Converting in Degrees °C"

    Note that here the convertion in not a affine law but an exponential law to match a thermistor reading.

!!! tip 
    Conversion parameters are set in memory. They remain after reboot.

::: doxy.powerAPI.class
name: DataAPI