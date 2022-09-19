if(CONFIG_OWNTECH_COMMUNICATION)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)
  # Define the current folder as a Zephyr library
  zephyr_library()
  # Select source files to be compiled
  zephyr_library_sources(
    src/can.cpp
    src/data_objects.cpp
    src/thingset_isotp.cpp
    src/thingset_pub_sub.cpp
    public_api/CanCommunication.cpp
    )
endif()
