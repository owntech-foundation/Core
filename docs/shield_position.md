!!! note ""
    Shield position is an API that allows to access position sensors connected through a Spin shield.
    It provides a single entry point, ``shield.position``, for Hall effect sensors, incremental encoders, and Sin/Cos encoders.
    Whatever the sensor type, the API can provide the same outputs: mechanical angle, electrical angle, mechanical speed, and electrical speed.

## Include

!!! note
    ```cpp
    #include <ShieldAPI.h>
    ```
    Make sure that Shield API is included to use ``shield.position``.

## Initialization sequence

For any position sensor, the recommended sequence is:

1. Select the sensor with ``shield.position.init(...)``
2. Adjust parameters if needed
3. Call ``shield.position.update(sample_time)`` periodically
4. Read angle and speed values

!!! example

    ```cpp
    shield.position.init(ABZ);
    shield.position.setCountsPerRevolution(4096);
    shield.position.setPolePairs(4);

    shield.position.update(100e-6F);

    float32_t electrical_angle = shield.position.getElectricalAngle();
    float32_t electrical_speed = shield.position.getElectricalSpeed();
    ```

## Common outputs

Once initialized, all supported sensor types provide the same estimated outputs:

- ``shield.position.getMechanicalAngle()``
- ``shield.position.getElectricalAngle()``
- ``shield.position.getMechanicalSpeed()``
- ``shield.position.getElectricalSpeed()``
- ``shield.position.getMechanicalPulsation()``
- ``shield.position.getElectricalPulsation()``

## Common configuration functions

The following functions can be used regardless of the selected sensor:

- ``shield.position.setDirectionSign(...)``
- ``shield.position.setPolePairs(...)``
- ``shield.position.setElectricalOffset(...)``

These parameters are typically used to adapt the sensor to the motor and control convention:

- reverse the positive rotation direction
- declare the number of motor pole pairs
- align the electrical zero used by the control

`direction-sign` must be either `-1` or `1`, and `pole-pairs` must be strictly positive.

For Hall sensors:

- ``HALL_INTERPOLATION_NONE`` keeps a discrete Hall-based estimator and does not start the PLL
- with ``HALL_INTERPOLATION_NONE``, the speed output is only updated on Hall transitions, so it does not provide a continuous speed estimate
- ``HALL_INTERPOLATION_LINEAR`` enables the PLL-based interpolation to smooth angle and speed between Hall transitions
- the Hall mechanical angle is derived from Hall sectors and pole pairs, so it is not shifted by ``electrical-offset``

## Hall effect sensor

Use the Hall sensor mode when your motor provides 3 digital Hall signals.

### Minimal initialization

1. Initialize the Hall sensor:

```cpp
shield.position.init(HALL);
```

2. Configure the Hall-related parameters if needed:

```cpp
uint8_t hall_table[6] = {5, 1, 0, 3, 4, 2};

shield.position.setPolePairs(4);
shield.position.setElectricalOffset(0.0F);
shield.position.setHallSectorTable(hall_table);
shield.position.setHallInterpolation(HALL_INTERPOLATION_LINEAR);
```

3. Update the estimator in your control loop:

```cpp
shield.position.update(100e-6F);
```

4. Read the outputs:

```cpp
float32_t mechanical_angle = shield.position.getMechanicalAngle();
float32_t electrical_angle = shield.position.getElectricalAngle();
float32_t electrical_speed = shield.position.getElectricalSpeed();
```

### Raw Hall access

If needed, the raw Hall state remains accessible:

```cpp
uint8_t hall_state = shield.position.getHallState();
```

Hall sensors only provide electrical-sector information, so ``getMechanicalAngle()`` is a mechanical angle modulo one electrical revolution divided by the number of pole pairs.

### Typical Hall parameters

The Hall effect sensor typically uses these parameters:

- ``direction-sign``
- ``pole-pairs``
- ``electrical-offset``
- ``hall-sector-table``
- ``hall-interpolation``

The default Hall sequence used by OwnVerter is:

- ``001 -> 011 -> 010 -> 110 -> 100 -> 101``

which corresponds to the default sector table:

```cpp
{5, 1, 0, 3, 4, 2}
```

### Example

```cpp
shield.position.init(HALL);

uint8_t hall_table[6] = {5, 1, 0, 3, 4, 2};

shield.position.setDirectionSign(1);
shield.position.setPolePairs(4);
shield.position.setElectricalOffset(0.0F);
shield.position.setHallSectorTable(hall_table);
shield.position.setHallInterpolation(HALL_INTERPOLATION_LINEAR);

shield.position.update(100e-6F);

float32_t angle = shield.position.getElectricalAngle();
float32_t speed = shield.position.getElectricalSpeed();
```

## Incremental encoder

Use the incremental encoder mode when your motor provides AB signals, with the timer peripheral handling the encoder interface wiring.

### Minimal initialization

1. Initialize the encoder:

```cpp
shield.position.init(ABZ);
```

2. Configure the encoder parameters:

```cpp
shield.position.setCountsPerRevolution(4096);
shield.position.setDirectionSign(1);
shield.position.setPolePairs(4);
shield.position.setElectricalOffset(0.0F);
```

3. Update the estimator:

```cpp
shield.position.update(100e-6F);
```

4. Read the outputs:

```cpp
float32_t mechanical_angle = shield.position.getMechanicalAngle();
float32_t electrical_angle = shield.position.getElectricalAngle();
float32_t mechanical_speed = shield.position.getMechanicalSpeed();
```

### Raw encoder access

If needed, the raw encoder count remains accessible:

```cpp
uint32_t encoder_count = shield.position.getIncrementalEncoderValue();
```

### Typical incremental encoder parameters

The incremental encoder typically uses these parameters:

- ``counts-per-revolution``
- ``direction-sign``
- ``pole-pairs``
- ``electrical-offset``

### Example

```cpp
shield.position.init(ABZ);

shield.position.setCountsPerRevolution(4096);
shield.position.setDirectionSign(1);
shield.position.setPolePairs(4);
shield.position.setElectricalOffset(0.0F);

shield.position.update(100e-6F);

float32_t angle = shield.position.getElectricalAngle();
float32_t speed = shield.position.getElectricalSpeed();
```

## Sin/Cos encoder

Use the Sin/Cos mode when your position sensor provides 2 analog channels: one sine and one cosine.

!!! warning
    ``shield.position.init(SINCOS)`` enables the semantic shield sensors, but converted Sin/Cos samples are only available once the ADC acquisition path has been started. Call the relevant data acquisition start sequence before relying on ``shield.position.update(...)``.

### Minimal initialization

1. Initialize the Sin/Cos encoder:

```cpp
shield.position.init(SINCOS);
```

2. Configure the common motor parameters:

```cpp
shield.position.setDirectionSign(1);
shield.position.setPolePairs(4);
shield.position.setElectricalOffset(0.0F);
```

3. Update the estimator:

```cpp
shield.position.update(100e-6F);
```

4. Read the outputs:

```cpp
float32_t mechanical_angle = shield.position.getMechanicalAngle();
float32_t electrical_angle = shield.position.getElectricalAngle();
float32_t electrical_speed = shield.position.getElectricalSpeed();
```

### Raw Sin/Cos access

If needed, the raw analog channels remain accessible:

```cpp
float32_t sin_value = shield.position.getSinValue();
float32_t cos_value = shield.position.getCosValue();
```

### Typical Sin/Cos parameters

The Sin/Cos encoder typically uses these parameters:

- ``direction-sign``
- ``pole-pairs``
- ``electrical-offset``

### Example

```cpp
shield.position.init(SINCOS);

shield.position.setDirectionSign(1);
shield.position.setPolePairs(4);
shield.position.setElectricalOffset(0.0F);

shield.position.update(100e-6F);

float32_t angle = shield.position.getElectricalAngle();
float32_t speed = shield.position.getElectricalSpeed();
```

## Using a default sensor

If your application always uses the same position sensor, you can define a default one in ``src/app.overlay``:

```dts
/ {
    chosen {
        owntech,position-sensor = &abz;
    };
};
```

Then initialize it in code with:

```cpp
shield.position.initDefault();
```

## Overriding default hardware parameters

Most users can configure the sensor directly from C++ with the runtime setters shown above.

If you want to change the default values stored by the shield, create or edit ``src/app.overlay``.

### Example for a Hall application

```dts
&default_motor {
    pole-pairs = <4>;
    direction-sign = <1>;
    electrical-offset = <0x00000000>;
    hall-sector-table = <5 1 0 3 4 2>;
    hall-interpolation = "LINEAR";
};
```

### Example for an incremental encoder application

```dts
&default_motor {
    counts-per-revolution = <4096>;
    pole-pairs = <4>;
    direction-sign = <1>;
    electrical-offset = <0x00000000>;
};
```

### Example for a Sin/Cos application

```dts
&default_motor {
    pole-pairs = <4>;
    direction-sign = <1>;
    electrical-offset = <0x3dcccccd>;
};
```

## Choosing between the 3 sensor types

### Hall effect sensor

Use Hall effect sensor when:

- you need a simple digital position feedback
- you want a robust and low-cost solution
- moderate angle precision is enough

### Incremental encoder

Use an incremental encoder when:

- you need high angular resolution
- you want precise speed estimation
- your sensor provides AB signals

### Sin/Cos encoder

Use a Sin/Cos encoder when:

- you have 2 analog sensor channels available
- you want continuous analog position information
- you want the API to estimate angle from the sine and cosine channels

## API Reference
::: doxy.powerAPI.class
name: PositionAPI
