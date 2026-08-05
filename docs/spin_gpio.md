!!! note ""

    A GPIO, or General Purpose Input/Output, is a pin on a microcontroller or processor that can be programmed to function either as an input or an output. As an input, it can read the state of an external electrical signal, such as a switch or a sensor. As an output, it can send an electrical signal, such as a high or low logic level, to an external component, such as an LED.


## Initialization sequence

!!! note

    === "Set the output pin"  
        1\. Configure the pin in output mode.  
        2\. Set the pin.  
    
    === "Reset the output pin"  
        1\. Configure the pin in output mode.  
        2\. Reset the pin  
    
    === "Read the input pin"  
        1\. Configure the pin in input mode.  
        2\. Read the pin input.  

!!! example 

    === "Set the output pin"  
        spin.gpio.configurePin(9, OUTPUT);  
        spin.gpio.setPin(9);  

    === "Reset the output pin"  
        spin.gpio.configurePin(9, OUTPUT);  
        spin.gpio.resetPin(9);  
    
    === "Read the input pin"  
        spin.gpio.configurePin(9, INPUT);  
        uint8_t return_gpio = spin.gpio.readPin(9);  



## API Reference

::: doxy.powerAPI.class
name: GpioHAL
