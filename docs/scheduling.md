!!! note ""
    PowerAPI is meant to deploy easily Real-time programs that must guarantee response within specified time constraints.  
    In that context, scheduling is the action of asigning computing ressources to perform tasks.  

## Include

!!! note 
    ```
    #include <TataAPI.h>
    ```
    Make sure that Task API is included

## Deploying a real time control task

Having a periodical code execution is key to real time applications. It is easy to spawn one using the TaskAPI.

!!! example
    === "20kHz Periodic task based on PWM"
        ```
            spin.taskAPI.createCritical(my_critical_function, 50, HRTIM);
            spin.taskAPI.startCritical();
        ```
    === "10kHz Periodic task based on Timer6"
        ```
            spin.taskAPI.createCritical(my_critical_function, 100, TIM6);
            spin.taskAPI.startCritical();
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

