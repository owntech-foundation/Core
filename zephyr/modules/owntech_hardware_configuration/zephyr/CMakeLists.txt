if(CONFIG_OWNTECH_HARDWARE_CONFIGURATION)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled
  zephyr_library_sources(
    src/hardware_auto_configuration.cpp
    src/DacHAL.cpp
    src/LedHAL.cpp
    src/timer_configuration.cpp
    src/PwmHAL.cpp
    src/uart_configuration.cpp
    src/adc_configuration.cpp
    src/CompHAL.cpp
    src/common_configuration.cpp
  )

  # Conditional source files
  if (CONFIG_SHIELD_TWIST)
    zephyr_library_sources(
      src/ngnd_configuration.cpp
    )
  endif()

endif()
