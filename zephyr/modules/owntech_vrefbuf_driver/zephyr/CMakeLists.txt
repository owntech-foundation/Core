if(CONFIG_OWNTECH_VREFBUF_DRIVER)
  # Define the current folder as a Zephyr library
  zephyr_library()
  # Select source files to be compiled
  zephyr_library_sources(
    ./src/vrefbuf_driver.c
    )
endif()
