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
      src/AdcHAL.cpp
      src/CompHAL.cpp
      src/DacHAL.cpp
      src/LedHAL.cpp
      src/PwmHAL.cpp
      src/TimerHAL.cpp
      src/UartHAL.cpp
      src/VersionHAL.cpp
    )

    # Conditional source files

    # Data API
    if(CONFIG_OWNTECH_DATA_API)
      zephyr_library_sources(
        src/data/data_conversion.cpp
        src/data/data_dispatch.cpp
        src/data/dma.cpp
        src/DataAPI.cpp
      )
    endif()

    if (CONFIG_SHIELD_TWIST)
      zephyr_library_sources(
        src/NgndHAL.cpp
      )
    endif()

    # GPIO API
    if (CONFIG_OWNTECH_GPIO_API)
      zephyr_library_sources(
        src/GpioHAL.cpp
      )
    endif()

  endif()

endif()
