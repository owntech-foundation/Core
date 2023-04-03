if(CONFIG_OWNTECH_DATA_ACQUISITION)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled
  zephyr_library_sources(
    ./src/dma.cpp
    ./src/data_dispatch.cpp
	./src/adc_channels.cpp
    ./src/data_conversion.cpp
    ./public_api/DataAcquisition.cpp
   )
endif()
