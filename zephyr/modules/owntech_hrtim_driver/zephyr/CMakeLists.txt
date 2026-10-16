if(CONFIG_OWNTECH_HRTIM_DRIVER)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)
  # Define the current folder as a Zephyr library
  zephyr_library()
  # Select source files to be compiled
  zephyr_library_sources(
    ./src/hrtim.c
    ./src/hrtim_dma.c
    )
endif()
