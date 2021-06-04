
if(CONFIG_OWNTECH_NGND_DRIVER)
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    owntech_ngnd_driver.c
    )
endif()
