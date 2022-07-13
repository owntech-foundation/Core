if(CONFIG_OWNTECH_DAC_DRIVER)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)
  # Define the current folder as a Zephyr library
  zephyr_library()
  # Select source files to be compiled
  zephyr_library_sources(
    ./src/stm32_dac_driver.c
    )
endif()
