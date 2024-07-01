if(CONFIG_OWNTECH_SPIN_API OR CONFIG_OWNTECH_GPIO_API)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled

  # Main API
  if (CONFIG_OWNTECH_GPIO_API)
    zephyr_library_sources(
      src/hardware_auto_configuration.cpp
      src/DacHAL.cpp
      src/LedHAL.cpp
      src/TimerHAL.cpp
      src/PwmHAL.cpp
      src/AdcHAL.cpp
      src/CompHAL.cpp
      src/VersionHAL.cpp
    )

    # Conditional source files
    if (CONFIG_OWNTECH_NGND_DRIVER)
      zephyr_library_sources(
        src/NgndHAL.cpp
    )
    endif()
  endif()

  # UART API
  if (CONFIG_OWNTECH_UART_API)
  zephyr_library_sources(
    src/UartHAL.cpp
  )
  endif()

  # GPIO API
  if (CONFIG_OWNTECH_GPIO_API)
    zephyr_library_sources(
      src/GpioHAL.cpp
    )
  endif()

endif()
