
if(CONFIG_OWNTECH_LEG_DRIVER)
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    owntech_leg_driver.c
    )
endif()
