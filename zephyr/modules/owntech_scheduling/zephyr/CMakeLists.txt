if(CONFIG_OWNTECH_SCHEDULING)
  # Select directory to add to the include path
  zephyr_include_directories(./public_api)

  # Define the current folder as a Zephyr library
  zephyr_library()

  # Select source files to be compiled
  zephyr_library_sources(
    public_api/Scheduling.cpp
    src/scheduling_common.cpp
    src/uninterruptible_synchronous_task.cpp
    src/asynchronous_tasks.cpp
    )
endif()
