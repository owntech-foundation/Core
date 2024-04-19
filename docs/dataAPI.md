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

1.  if hardware triggered : [Make sure PWM engine is initialized](pwm/#initialization-sequence)
2.  [Make sure ADC is initialized](adc/#initialization-sequence) 
2.  Define acquisition conversion parameter: `data.setParameters()`
3.  if Hardware triggered : `data.start()`
4.  Retrieve values : `data.getLatest()` or `data.getRawValues()`
`
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