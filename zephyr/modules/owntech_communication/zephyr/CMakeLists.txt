if(CONFIG_OWNTECH_COMMUNICATION)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  zephyr_library_sources(
    public_api/CommunicationAPI.cpp
  )

  if (CONFIG_OWNTECH_COMMUNICATION_ENABLE_ANALOG)
    zephyr_library_sources(
      src/AnalogCommunication.cpp
    )
  endif()

  if (CONFIG_OWNTECH_COMMUNICATION_ENABLE_CAN)
    zephyr_library_sources(
      src/data_objects.cpp
      src/thingset_can.c
      src/CanCommunication.cpp
    )
  endif()

  if (CONFIG_OWNTECH_COMMUNICATION_ENABLE_RS485)
    zephyr_library_sources(
      src/Rs485Communication.cpp
      src/Rs485.cpp
    )
  endif()

  if (CONFIG_OWNTECH_COMMUNICATION_ENABLE_SYNC)
    zephyr_library_sources(
      src/SyncCommunication.cpp
    )
  endif()

endif()