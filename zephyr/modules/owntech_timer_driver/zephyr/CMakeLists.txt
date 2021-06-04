
if(CONFIG_OWNTECH_TIMER_DRIVER)
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    stm32_timer_driver.c
    )
endif()
