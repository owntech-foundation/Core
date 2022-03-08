if(CONFIG_OWNTECH_ADC_DRIVER)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled
  zephyr_library_sources(
    ./src/adc_channels.c
    ./src/adc_core.c
    ./src/adc_helper.c
    ./public_api/adc.c
    )
endif()
