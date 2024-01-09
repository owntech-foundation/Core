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
    src/TimerHAL.cpp
    src/PwmHAL.cpp
    src/UartHAL.cpp
    src/AdcHAL.cpp
    src/CompHAL.cpp
    src/VersionHAL.cpp
  )

  # Conditional source files
  if (CONFIG_SHIELD_TWIST)
    zephyr_library_sources(
      src/NgndHAL.cpp
    )
  endif()

endif()
