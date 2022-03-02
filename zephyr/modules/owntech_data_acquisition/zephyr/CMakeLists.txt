if(CONFIG_OWNTECH_DATA_ACQUISITION)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled
  zephyr_library_sources(
    ./adc/adc.c
    ./adc/adc_channels.c
    ./adc/adc_core.c
    ./adc/adc_helper.c
    ./dma/dma.c
    ./data_dispatch/data_dispatch.c
	./data_conversion/data_conversion.c
    ./public_api/DataAcquisition.cpp
   )
endif()
