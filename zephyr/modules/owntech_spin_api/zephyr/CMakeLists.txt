if(CONFIG_OWNTECH_SPIN_API)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled

  # Main API
  zephyr_library_sources(
    public_api/SpinAPI.cpp
    src/hardware_auto_configuration.cpp
    src/CompHAL.cpp
    src/DacHAL.cpp
    src/LedHAL.cpp
    src/PwmHAL.cpp
    src/TimerHAL.cpp
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

  # NGND driver
  if (CONFIG_OWNTECH_NGND_DRIVER)
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
