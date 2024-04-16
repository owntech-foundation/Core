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
    ![phase_shift_80](images/duty_cycle_how_it_works.svg)){ width=800 }

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
    ![phase_shift_80](images/phase_shift_80.svg)){ width=800 }

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

### Multiple PWM