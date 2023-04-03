if(CONFIG_OWNTECH_HARDWARE_CONFIGURATION)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled
  zephyr_library_sources(
    src/hardware_auto_configuration.cpp
    src/dac_configuration.cpp
    src/ngnd_configuration.cpp
    src/led_configuration.cpp
    src/timer_configuration.cpp
    src/hrtim_configuration.cpp
    src/uart_configuration.cpp
    src/adc_configuration.cpp
    src/power_driver_configuration.cpp
    src/comparator_configuration.cpp
    src/common_configuration.cpp
    )
endif()
