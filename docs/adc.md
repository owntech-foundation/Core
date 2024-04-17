## Software triggered



## Synchronous with PWMs

!!! note
    These features are available for ADC1 and ADC2.

### Linking ADC and PWM channel

ADC1 and ADC2 can be associated with PWM channels at will. 

=== "ADC1 and PWMA"
    ![left_aligned_ADC_50](images/left_aligned_ADC_50.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT1);    
    ```

    ::: doxy.powerAPI.class.method
    name: AdcHAL
    method: void AdcHAL::configureDiscontinuousMode ()

=== "ADC1 and PWMC"
    ![left_aligned_ADC_PWM_C](images/left_aligned_ADC_PWM_C.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT2);    
    ```

### Modulation impact

!!! note 
    Hardware trigger depends on PWM modulation.

=== "Left Aligned Modulation"
    ![left_aligned_ADC_50](images/left_aligned_ADC_50.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT1);    
    ```

=== "Center aligned modulation"
    ![center_aligned_ADC_rising_edge](images/center_aligned_ADC_rising_edge.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT2);
    ```

### Rising Edge / Falling edge

!!! note 
    This only applies to center aligned modulation.

=== "Rising Edge"
    ![center_aligned_ADC_rising_edge](images/center_aligned_ADC_rising_edge.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT1);    
    ```

=== "Falling Edge"
    ![center_aligned_ADC_falling_edge](images/center_aligned_ADC_falling_edge.svg){ width=800 }


    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT2);    
    ```


### Measurment trigger instant

!!! note 
    This feature is only available in left aligned modulation.

=== "20% Duty cycle"
    ![left_aligned_ADC_20](images/left_aligned_ADC_20.svg){ width=800 }

    ```c++
        spin.pwm.setDutyCycle(PWMA, 0.2);

    ```


=== "80% Duty cycle"
    ![left_aligned_ADC_80](images/left_aligned_ADC_80.svg){ width=800 }

    ```c++
        spin.pwm.setDutyCycle(PWMA, 0.8);
    ```

### Sampling time

Sampling times can be selected in order to take longer time measurement. It is similar to the exposure time taken for a photograph. The longest is the sampling time, the more averaged the signal will be.

!!! note 
    Default value is 446ns.

=== "Sampling time 1.410us"
    ![sampling_time_1410ns](images/sampling_time_1410ns.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT2);    
    ```

=== "Sampling time 446ns"
    ![sampling_time_446ns](images/sampling_time_446ns.svg){ width=800 }
    

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT1);    
    ```


### Continuous / Discontinuous sequence

!!! note 
    This is relevant if more than one measurement is taken with the same ADC.

=== "Continuous sequence"
    ![left_aligned_continuous_sampling](images/left_aligned_continuous_sampling.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT2);    
    ```

=== "Discontinuous sequence "
    ![left_aligned_ADC_50](images/left_aligned_ADC_50.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT1);    
    ```


### Number of PWM period between acquisition

!!! note 
    This is relevant if more than one measurement is taken with the same ADC.

=== "With 2 periods"
    ![sparse_sampling_2](images/sparse_sampling_2.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT1);    
    ```

=== "With 4 periods"
    ![sparse_sampling_4](images/sparse_sampling_4.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT1);    
    ```

=== "Without interuptions"
    ![left_aligned_continuous_sampling](images/left_aligned_continuous_sampling.svg){ width=800 }

    ```c++
        spin.pwm.startSingleOutput(PWMA, TIMING_OUTPUT2);    
    ```


## API Reference 

::: doxy.powerAPI.class
name: AdcHAL


