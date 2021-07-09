if(CONFIG_OWNTECH_TIMER_DRIVER)
  zephyr_include_directories(./public_include)
  zephyr_library()
  zephyr_library_sources(
    ./src/stm32_timer_driver.c
    )
endif()
