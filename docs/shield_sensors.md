!!! note ""
    Shield sensors is an API that allows to access sensors from a Spin shield easily.
    Acquiring data without a shield is done using [Data API](dataAPI.md), but Shield sensors API builds on Data API to provide access to sensors by name as well as automatically configuring the parameters for the raw data conversion.

## Quick start with Shield sensor API

To enable the Twist Shield channels, indicate which shield you are using by editing the file ``platformio.ini`` to define the ``board_shield=`` and ``board_shield_version=`` commands. From there, the Shield sensors API will be available simply by including ``ShieldAPI.h`` in your code.

Using shield sensor API is done quite similarly to the [Data API](dataAPI.md). The difference is that you do not have to know which pin a sensor is connected to.

For example, if you want to access the V1_LOW sensor from the Twist shield using Data API, you would have to consult the pinout of the shield and identify that it is connected to pin 29 of the Spin, then write:
```
spin.data.enableAcquisition(29);
```
However, using Shield sensor API, you just have to write;
```
shield.sensors.enableAcquisition(V1_LOW);
```

Everything else should be used similarly as Data API, just replace ``spin.data`` by ``shield.sensors`` in your code, then use the sensor name instead of the pin number as a parameter for the functions.

## Auto-configuration of the API

Moreover, as Shield sensor API is aware of what sensors are available on the shield you use, it provides an auto-configuration that enables acquisition for all the sensors from the shield.

The function is named ``shield.sensors.enableDefault***Sensors()``. For example, for the Twist shield, the function is named ``shield.sensors.enableDefaultTwistSensors()``. Just call this function then start the API, and you're ready to go!

::: doxy.powerAPI.class
name: SensorsAPI
