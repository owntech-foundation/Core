if(CONFIG_OWNTECH_DATA_ACQUISITION)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled
  zephyr_library_sources(
    ./src/dma.cpp
    ./src/data_dispatch.cpp
    ./src/data_conversion.cpp
    ./src/nvs_storage.cpp
    ./public_api/DataAcquisition.cpp
  )

  # Conditional source files
  if (CONFIG_SHIELD_TWIST)
    zephyr_library_sources(
      ./src/shield_channels.cpp
    )
  endif()

endif()
