if(CONFIG_OWNTECH_SPIN_API)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled

  # Main API
  zephyr_library_sources(
    public_api/SpinAPI.cpp
    src/data/data_conversion.cpp
    src/data/data_dispatch.cpp
    src/data/dma.cpp
    src/hardware_auto_configuration.cpp
    src/CompHAL.cpp
    src/DacHAL.cpp
    src/DataAPI.cpp
    src/LedHAL.cpp
    src/PwmHAL.cpp
    src/TimerHAL.cpp
  )

  # Conditional source files

  # GPIO API
  if (CONFIG_OWNTECH_GPIO_API)
    zephyr_library_sources(
      src/GpioHAL.cpp
    )
  endif()

  # UART API
  if (CONFIG_OWNTECH_UART_API)
    zephyr_library_sources(
      src/UartHAL.cpp
    )
  endif()

endif()
