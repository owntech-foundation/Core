if(CONFIG_OWNTECH_SAFETY_API)
  # Select directory to add to the include path
  zephyr_include_directories (./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled
  zephyr_library_sources(
    src/safety_setting.cpp
    src/safety_shield.cpp
    public_api/SafetyAPI.cpp
    )
endif()
