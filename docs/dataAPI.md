!!! note ""
    Data acquisition is the process of sampling signals that measure real-world physical conditions and converting the resulting samples into digital numeric values that can be manipulated by a computer.

    In the powerAPI context, the DataAPI helps the user retrieve the values sampled by the [ADC](adc.md).

    The Data API configures the DMAs to store the ADCs acquisitions for the Spin board, and dispatches them in per-channel buffers that can be read by the user. The module also provides functions to convert the raw values acquired by the sensors into values in the adequate unit.

    If using a shield such as Twist, channels are automatically made available for configuration and conversions functions are automatically calibrated using the device tree.

## Include

!!! note
    ```
    #include <DataAPI.h>
    ```

To use the Data API, include `DataAPI.h` in your source file. From there, a `data` object is available to interact with the API.

## Initialization sequence

If you want specific ADC behavior (trigger sources, discontinuous mode, etc.), you may want to configure the ADCs using the lower-level [ADC API](adc.md) first. Then, you can enable channels that you want to acquire.

After channels have been enabled, the conversion parameters can be set so that raw values can be automatically converted to the relevant unit. This is done using the `data.setParameters()` function.

After channels have been enabled (and optionnally conversion parameters have been set), there are two ways of starting the API, depending on your use of other OwnTech APIs. If your code uses an [uninterruptible task](scheduling.md), nothing more is required, the Data API will be started automatically when task is started. However, if you do not have an uninterruptible task in your code, you need to manually start the API by calling `data.start()`.

!!! Note

    === "Software triggered"
        1.  Enable acquisition on the pins you want: [`data.enableAcquisition()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-enableacquisition)
        2.  Define acquisition conversion parameter: [`data.setParameters()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-setparameters-22)
        3.  start data dispatching [`data.start()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-start)
        4. Trigger an initial adc conversion [`data.triggerAcquisition(ADCx)`](https://owntech-foundation.github.io/Documentation/powerAPI/classAdcHAL/#function-enablechannel)
        5.  Retrieve values : [`data.getLatest()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getlatest-12) or [`data.getRawValues()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getrawvalues-12)

    === "Hardware triggered"
        1.  [Make sure PWM engine is initialized](https://owntech-foundation.github.io/Documentation/core/docs/pwm/)
        2.  Enable acquisition on the pins you want: [`data.enableAcquisition()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-enableacquisition)
        3.  Define acquisition conversion parameter: [`data.setParameters()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-setparameters-22)
        4.  start data dispatching [`data.start()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-start)
        5.  Retrieve values : [`data.getLatest()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getlatest-12) or [`data.getRawValues()`](https://owntech-foundation.github.io/Documentation/core/docs/dataAPI/#function-getrawvalues-12)

!!! example

    === "Software triggered ADC"
        ```cpp
        data.enableAcquisition(1, 5); // ADC 1 ; Pin 5
        data.triggerAcquisition(1); // ADC 1
        float32_t adc_value = data.getLatest(1, 5); // ADC 1 ; Pin 5
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
        spin.adc.configureTriggerSource(1, hrtim_eev1); // ADC 1 ; HRTIM event 1
        spin.adc.configureDiscontinuousMode(1, 1); // ADC 1 ; acquire 1 pin at each event
        data.enableAcquisition(1, 5); // ADC 1 ; Pin 5
        data.start();
        float32_t adc_value = data.getLatest(1, 5); // ADC 1 ; Pin 5
        ```

## Retrieving last value

Getting the last measured value to feed the control algorithm is super simple.

!!! example
    === "Get last value"
        ```
        spin.data.getLatest(1)
        ```
        This will retrieve the last sampled value of pin 1.

## Getting values with the right unit

DataAPI contains commodity functions to convert the raw binary measurement values in meaningful units.

!!! example
    === "Converting in volts"
        ```
        data.setParameters()
        ```

## Get an array of values

DataAPI contains commodity function to retrieve an array of raw values that can be fed to a [post processing filter](controlLibrary/controlLibrary/docs/use-filters).

!!! example
    === "Retrieve 5 values"
        ```
        data.getRawValues()
        ```

::: doxy.powerAPI.class
name: DataAPI

## Using the Data API with a Twist shield

The Data API is optimized for running on Twist shields.
It provides variants of the functions that take Twist sensors as parameters instead of pins numbers.

To enable the Twist Shield channels, use `set(SHIELD twist)` in `CMakeLists.txt`. From there, this module will provide additional functions to acquire and convert Twist channels by name.

If you use the Twist sensors, you can use `data.enableTwistDefaultChannels()` to automatically perform a standard configuration. From there, all current/tension channels of the Twist shields will be enabled as part of the default configuration. Default parameters for the channels are also loaded automatically as part of the process.
