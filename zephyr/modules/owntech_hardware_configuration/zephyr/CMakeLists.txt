if(CONFIG_OWNTECH_HARDWARE_CONFIGURATION)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled
  zephyr_library_sources(
    src/hardware_auto_configuration.cpp
	src/dac_configuration.cpp
	src/ngnd_configuration.cpp
	src/led_configuration.cpp
	src/timer_configuration.cpp
	public_api/HardwareConfiguration.cpp
    )
endif()
