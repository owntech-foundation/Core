if(CONFIG_OWNTECH_COMPARATOR_DRIVER)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)
  # Define the current folder as a Zephyr library
  zephyr_library()
  # Select source files to be compiled
  zephyr_library_sources(
    ./src/comparator_driver.c
    ./public_api/comparator.c
    )
endif()
