if(CONFIG_OWNTECH_SHIELD_API)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)
  zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../owntech/lib/USB/control_library/src)
  zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../owntech/lib/STLink/control_library/src)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled

  # Main API
  zephyr_library_sources(
    ./src/Position.cpp
    ./src/Sensors.cpp
    ./src/Power.cpp
    ./src/power_init.cpp
    ./public_api/ShieldAPI.cpp
    )

  # Conditional source files

  # NGND driver
  if (CONFIG_OWNTECH_NGND_DRIVER)
    zephyr_library_sources(
      src/NgndHAL.cpp
  )
  endif()

endif()
