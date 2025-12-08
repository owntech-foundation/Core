## Introduction

Synchronization (Sync) provides precise alignment of PWM signals and real-time control tasks across multiple SPIN boards.  

This is particularly useful for advanced applications that require time-deterministic behavior, such as decentralized control or high-resolution PWM coordination beyond the capability of a single board.

!!! success "Synchronized PWM carriers"
    - Using this synchronization features permits to have PWM carriers synched down to **25ns ± 5ns** (results obtained with 30cm CAT5 S-FTP RJ-45 cables).  
    - Jitter on synched control tasks is on the order of a micro second.

## How it works

**Board A** is defined as the sync Master, it effectively defines the time reference. 
The PWM generator of the Master triggers its real time control task based on an internal repetition timer.  

When entering the real time control task, the special sync output pin (pin number 42) is set in alternate mode.  

This allows to send the next repetition event to **Board B**. Alternate mode is maintained for few microseconds in order to wait for the next incoming PWM event.  

The sync pin is then returned to normal mode, effectively halting event propagation until the next real-time control task deadline. 

![Working principle of the synchronization](images/communication_sync.svg)

!!! warning "A small delay is to be expected between **Board A** and **Board B** control tasks"
    Current sync behavior does introduce a delay, because of the busy wait needed to control event propagation.
    However PWM carrier are perfectly synched.


## How to use. 

All there is to do is initialize both the Master and the Slave(s).

=== "Initialize the master board"  

    ```
    void setup_routine()
    {
        /* Initialize the PWM engine first */
        shield.power.initBuck(ALL);
        /* Set up synchronization mode as Master */
        communication.sync.initMaster();

        /* Declare tasks */
        uint32_t background_task_number =
                        task.createBackground(loop_background_task);

        uint32_t app_task_number = task.createBackground(application_task);

        /* Make sure the real time task is based on the PWM engine */
        task.createCritical(loop_critical_task, control_task_period, source_hrtim);

        /* Finally, start tasks */
        task.startBackground(background_task_number);
        task.startBackground(app_task_number);
        task.startCritical();
    }
    ```  

=== "Initialize the slave(s) board(s)"
    ```
    void setup_routine()
    {
        /* Initialize the PWM engine first */
        shield.power.initBuck(ALL);
        /* Set up synchronization mode as Slave */
        communication.sync.initSlave();

        /* Declare tasks */
        uint32_t background_task_number =
                        task.createBackground(loop_background_task);

        uint32_t app_task_number = task.createBackground(application_task);

        /* Make sure the real time task is based on the PWM engine */
        task.createCritical(loop_critical_task, control_task_period, source_hrtim);

        /* Finally, start tasks */
        task.startBackground(background_task_number);
        task.startBackground(app_task_number);
        task.startCritical();
    }
    ```


## Current Limitations

This synchronization feature is optimized for systems where the real-time control task frequency is significantly lower than the PWM frequency—typically by a factor of 10 or more.

However, in scenarios where the PWM frequency approaches or matches the control task frequency, the current implementation becomes ineffective. This is because:

 - The Master board relies on a busy-wait mechanism to control the propagation of the synchronization signal.

 - When the timing between PWM events and control tasks becomes too close, this busy-wait duration would need to be extended substantially.

 - Such long blocking periods would interfere with real-time responsiveness and potentially degrade system performance.

As a result, the current design does not support synchronization in setups where the control and PWM frequencies are too close.

## API Reference
::: doxy.powerAPI.class
name: SyncCommunication
