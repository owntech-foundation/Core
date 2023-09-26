if(CONFIG_OWNTECH_POWER_API)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled
  zephyr_library_sources(
    ./public_api/power.cpp
    ./src/power_init.cpp
    )
endif()
