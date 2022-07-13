if(CONFIG_OWNTECH_NGND_DRIVER)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)
  # Define the current folder as a Zephyr library
  zephyr_library()
  # Select source files to be compiled
  zephyr_library_sources(
    src/owntech_ngnd_driver.c
    )
endif()
