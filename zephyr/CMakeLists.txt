# Macro to list all subdirectories in a specified directory
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${curdir}/${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# Define SUBDIRS as the list of all subdirectories of the modules directory
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR}/modules)

# Append found modules to zephyr extra modules variable
list(APPEND ZEPHYR_EXTRA_MODULES ${SUBDIRS})

# Check for third party modules
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party_modules)
  SUBDIRLIST(THIRDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/third_party_modules)
  list(APPEND ZEPHYR_EXTRA_MODULES ${THIRDPARTY})
endif()

# Board
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Configure Zephyr
cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(owntech_power_api)

target_sources(app PRIVATE ../src/main.cpp)
zephyr_include_directories(../src)
