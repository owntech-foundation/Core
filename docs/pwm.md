PWM, or Pulse Width Modulation, is a method used in electronics to control the power supplied to devices like motors, LEDs, and heaters. Instead of changing the voltage or current level, PWM changes the amount of time the power is on (the pulse width) compared to the time it's off.

In power electronics, PWM is super important because it allows us to control the average power delivered to a load by varying the duty cycle of the pulse. This means we can regulate the speed of motors or the brightness of LEDs.

## Features

=== " "

    ![HRTIM pin](images/SPIN_1.1.0_main_hrtim.svg){ align=left }

    - Up to 5 pair of PWMs possible (PWMA, PWC, PWMD, PWME, PWMF)
    - Each PWM unit has a resolution of 184ps.
    !!! note
        The resolution of a PWM signal defines the maximum number of steps that can be present in a single PWM period. In other words, it determines how finely you can adjust the pulse width of the        signal. A higher resolution means you can make smaller and more precise adjustments to the pulse width.

    - Frequencies in the range of 650Hz to 1Ghz theorically possible
    !!! warning
        Only 200Khz was tested with SPIN and TWIST
    - Two different modulation mode possible : left aligned, and center aligned
    - Two ADC linkable to two differents PWM unit for precise measurement instant
    - Compatible for peak current control with the internal analog comparator
    - Positive and negative dead time set via software
    - Phase shift possible for interleaved operations

## How it works

To generate a PWM you need two signals, a variable high frequency signals called the carrier and a constant signal called the duty cycle. 

### Carrier signal

Concerning the carrier, it is usually a counter in the form of a triangle wave. For example, it can be a counter incrementing from 0 to 27200.

![PWM carrier](images/Carrier_PWM.svg)

The increment between each step is made at the PWM resolution which is here 184ps, so the carrier gets from 0 to 27200 in 5µs so a frequency of 200kHz. 

### Duty cycle

The duty cycle is a constant value compared to the carrier. 

![Duty cycle](images/changing_duty_cycle.gif)

When the carrier is superior to the duty cycle, the logic output is at high level. On the contrary, when the carrier is inferior to the duty cycle the the output logic is low level. A PWM signal is thus a sqarewave signal with a frequency (the **switching frequency**) is fixed by the carrier.

Varying the duty cycle is how to vary the output of the PWM.


## Snippet examples


### Frequency

=== "200kHz frequency"
    ![frequency 200 kHz](images/frequency_200.svg){ width=800 }

    ```c++
        spin.pwm.setFrequency(PWMA, 200000);

    ```


=== "400kHz frequency"
    ![frequency 400 kHz](images/frequency_400.svg){ width=800 }

    ```c++
        spin.pwm.setFrequency(PWMA, 400000);

    ```

### Single/Dual Output

=== "Single Output"
    ![single_output_pwm](images/single_output_pwm.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT1);    
    ```

=== "Single Output PWMx2"
    ![single_output_pwm2](images/single_output_pwm2.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT2);    
    ```

=== "Dual Output"
    ![dual_output_pwm](images/dual_output_pwm.svg){ width=800 }

    ```c++
        spin.pwm.startDualOutput(PWMA);
    ```

### Duty Cycle

=== "20% Duty cycle"
    ![duty_cycle_20](images/duty_cycle_20.svg){ width=800 }

    ```c++
        spin.pwm.setDutyCycle(PWMA, 0.2);

    ```


=== "80% Duty cycle"
    ![duty_cycle_80](images/duty_cycle_80.svg){ width=800 }

    ```c++
        spin.pwm.setDutyCycle(PWMA, 0.8);
    ```

=== "How it works"
    ![phase_shift_80](images/duty_cycle_how_it_works.svg){ width=800 }

    ```c++
        spin.pwm.setDutyCycle(PWMA, 0.8);
    ```

### Phase Shift

=== "20% Phase Shift"
    ![phase_shift_20](images/phase_shift_20.svg){ width=800 }

    ```c++
        spin.pwm.setPhaseShift(PWMA,  72);
    ```

=== "80% Phase Shift"
    ![phase_shift_80](images/phase_shift_80.svg){ width=800 }

    ```c++
        spin.pwm.setPhaseShift(PWMA,  288);
    ```

=== "How it works"
    ![phase_shift_how_it_works](images/phase_shift_with_carriers.svg){ width=800 }

    ```c++
        spin.pwm.setPhaseShift(PWMA,  288);
    ```

### Dead Time

=== "2% Dead Time"
    ![dead_time_02](images/dead_time_02.svg){ width=800 }

    ```c++
        spin.pwm.setDeadtime(PWMA,  100,  100);

    ```

=== "8% Dead Time"
    ![dead_time_08](images/dead_time_08.svg){ width=800 }

    ```c++
        spin.pwm.setDeadtime(PWMA,  400,  400);
    ```

=== "Asymetric Dead Time leading edge"
    ![dead_time_asymetric_0208](images/dead_time_asymetric_0208.svg){ width=800 }

    ```c++
        spin.pwm.setDeadtime(PWMA,  100,  400);
    ```

=== "Asymetric Dead Time trailing edge"
    ![dead_time_asymetric_0802](images/dead_time_asymetric_0802.svg){ width=800 }

    ```c++
        spin.pwm.setDeadtime(PWMA,  400,  100);
    ```

### Modulation Type

=== "Left Aligned"
    ![left_aligned_modulation_pwm](images/left_aligned_modulation_pwm.svg){ width=800 }

    ```c++
        spin.pwm.setModulation(PWMA, Lft_aligned);
        spin.pwm.setDutyCycle(PWMA, 0.28);
    ```

=== "Center Aligned"
    ![center_aligned_modulation_pwm](images/center_aligned_modulation_pwm.svg){ width=800 }

    ```c++
        spin.pwm.setModulation(PWMA, UpDwn);
        spin.pwm.setDutyCycle(PWMA, 0.28);
    ```

!!! Example 
    See this examples for more details :  

    - [How to control duty cycle of a PWM unit](https://owntech-foundation.github.io/Documentation/examples/SPIN/PWM/duty_cycle_setting/)  

    - [How to set the trigger instant by linking adc trigger to PWM unit](https://owntech-foundation.github.io/Documentation/examples/SPIN/ADC/adc_hrtim_trigger/)  

    - [Managing phase shift between two PWM units](https://owntech-foundation.github.io/Documentation/examples/SPIN/PWM/phase_shift/)
