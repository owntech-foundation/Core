if(CONFIG_OWNTECH_SHIELD_API)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)
  # Define the current folder as a Zephyr library
  zephyr_library()
  # Select source files to be compiled
  zephyr_library_sources(
    ./src/shield_channels.cpp
    ./src/Sensors.cpp
    ./public_api/ShieldAPI.cpp
    )
endif()
