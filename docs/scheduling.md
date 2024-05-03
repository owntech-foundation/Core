!!! note ""
    PowerAPI is meant to deploy easily Real-time programs that must guarantee response within specified time constraints.
    In that context, scheduling is the action of asigning computing ressources to perform tasks.

## Include

!!! note
    ```
    #include <TaskAPI.h>
    ```
    Make sure that Task API is included

## Deploying a real time control task

Having a periodical code execution is key to real time applications. It is easy to spawn one using the TaskAPI.


 ### The control task sources

 You can have different **source** calling the control task.

The control task is synchronous, it means that it is called at fixed period. So we need some kind of timer calling the control, this timer is considered as the **source**. There are two sources : 
- The PWM carrier
- An independant timer

#### The PWM carrier

The carrier has a period called the **switching period**, we can use it to call the control task after a fixed number of period. 

![PWM control task](images/pwm_source_task.svg)

On the figure above, the switching period is 5µs (200Khz) and we call the control every 10 switching cycle so 50µs (20Khz).

!!! warning
    There are limitations when using this method : 
        - You need to start the a PWM to start the control task
        - You can only have control period which are multiple of the switching period
        - You can not have a control period inferior to the switching period

!!! tip
    Synchronizing the control task period with PWM period can be usefull when you try to synchronize PWM between several SPIN or TWIST, in that case the control task is also synchronized.

!!! example
    === "20kHz Periodic task based on PWM"
        ```
            task.createCritical(my_critical_function, 50, HRTIM);
            task.startCritical();
        ```
    === "10kHz Periodic task based on Timer6"
        ```
            task.createCritical(my_critical_function, 100, TIM6);
            task.startCritical();
        ```

A periodic task runs on a trigger based on a counter. Each time the counter is reached, the control task is executed.

!!! note
    The control task has priority over any other task. It will preempts any [background task](#non-time-critical-tasks).
    The control task can not be preempted. That is why it is also refered as an ininteruptible task.

Often the control task has to be in sync with the output PWM signals. Has the [PWM generator is a counter](pwm/#carrier-signal-and-pwm-resolution), the trigger source of the control task can be the PWMs.

!!! tip
    If PWM counter is used as the clock source for the control task, the period has to be a multiple of PWM frequency.
    It is not the case if Timer6 is used a clock source.

!!! tip
    Periodic clock source for the control task can be choosed between PWM and Timer6.

!!! tip
    Having a control Task is required for [synchronous measurements](adc/#synchronous-with-pwms) to work correctly.

## Non time critical tasks

In the powerAPI, non time critical tasks are refered as background tasks.

!!! example
    === "Spawning a background task"
        ```
            void my_background_function(){
                do_stuff();
            }
            spin.taskAPI.createBackground(my_background_function);
        ```
        In that case `do_stuff()` will execute continuously each time the processor is not occupied by the critical task.

    === "Making it pseudo periodic"
        ```
            void my_background_function(){
                do_stuff();
                suspendBackgroundUs(500);
            }
        ```
        In that case after executing `do_stuff();` the task will be suspended for 500us and resumed after. It creates a pseudo periodical task, runs every 500us + the time taken to execute `do_stuff()`.

::: doxy.powerAPI.class
name: TaskAPI
