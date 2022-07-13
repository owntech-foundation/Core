if(CONFIG_OWNTECH_HRTIM_DRIVER)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)
  # Define the current folder as a Zephyr library
  zephyr_library()
  # Select source files to be compiled
  zephyr_library_sources(
    ./src/voltage_mode/hrtim_voltage_mode.c
    ./src/voltage_mode/owntech_leg_driver.cpp
    ./src/current_mode/hrtim_current_mode.c
    ./src/hrtim_common.c
    )
endif()
