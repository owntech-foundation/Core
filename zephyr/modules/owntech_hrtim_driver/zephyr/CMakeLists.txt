
if(CONFIG_OWNTECH_HRTIM_DRIVER)
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    owntech_hrtim_driver.c
    )
endif()
