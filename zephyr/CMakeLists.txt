# Macro to list all subdirectories containing a Zephyr module in a specified directory
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      IF(IS_DIRECTORY ${curdir}/${child}/zephyr)
        IF(EXISTS ${curdir}/${child}/zephyr/CMakeLists.txt)
          LIST(APPEND dirlist ${curdir}/${child})
        ENDIF()
      ENDIF()
    ENDIF()
  ENDFOREACH()
  # Make sure modules are always in the same order for incremental compilation
  LIST(SORT dirlist)
  SET(${result} ${dirlist})
ENDMACRO()

# This macro check for app.extension in curdir folder, where curdir and extension
# are given as inputs. If found it returns the absolute path of app.extension.
MACRO(FILELIST result curdir extension)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/app.${extension})
  # Check that the macro found a matching file. If not return void result
  IF(DEFINED children AND NOT "${children}" STREQUAL "")
    # If found returns app.extension path as a result.
    SET(${result} ${curdir}/${children})
  ENDIF()
ENDMACRO()

# This macro check for app.extension in curdir folder, where curdir and extension
# are given as inputs. If found it returns the absolute path of app.extension.
MACRO(CONFLIB result curdir extension)
  FILE(GLOB_RECURSE children RELATIVE ${curdir} ${curdir}/lib.${extension})
  # Check that the macro found a matching file. If not return void result
  IF(DEFINED children AND NOT "${children}" STREQUAL "")
    # If found returns app.extension path as a result.
    FOREACH(child ${children})
      LIST(APPEND ${result} ${curdir}/${child})
    ENDFOREACH()
  ENDIF()
ENDMACRO()

# Define SUBDIRS as the list of all subdirectories of the modules directory
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR}/modules)

# Append found modules to zephyr extra modules variable
list(APPEND ZEPHYR_EXTRA_MODULES ${SUBDIRS})

# Add extra config and overlay files
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

# Looking for app.conf file
FILELIST(EXTRA_APP_CONF_FILE ${PARENT_DIR}/src conf)
# Add app.conf path to zephyr variables
set(EXTRA_CONF_FILE ${EXTRA_APP_CONF_FILE})

# Looking for lib.conf file
# BUILD_ENV_NAME is passed from pio_extra.ini using board_build.cmake_extra_args.
CONFLIB(EXTRA_LIB_CONF_FILE ${PARENT_DIR}/owntech/lib/${BUILD_ENV_NAME} conf)
# Add app.conf path to zephyr variables
list(APPEND EXTRA_CONF_FILE ${EXTRA_LIB_CONF_FILE})

# Looking for app.overlay file
FILELIST(EXTRA_OVERLAY_APP_FILE ${PARENT_DIR}/src overlay)
# Add app.overlay path to zephyr variables
set(EXTRA_DTC_OVERLAY_FILE ${EXTRA_OVERLAY_APP_FILE})

# Check for third party modules
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party_modules)
  SUBDIRLIST(THIRDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/third_party_modules)
  list(APPEND ZEPHYR_EXTRA_MODULES ${THIRDPARTY})
endif()

# Add OwnTech Board repository
# Board repository contains all boards and shields dts overlays and 
# config files required for the build
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Set Board - It will be changed automatically by pre_version.py
# Use platformio.ini and define board and board_version
# board_version is concatenated to board with an extra "@"
# "@" is the default versionning notation for zephyr build system. 

# Set Shield - It will be changed automatically by pre_version.py
# Use platformio.ini and define board_shield and board_shield_version
# board_shield_version is concatenated to board_shield. 

set(BOARD spin@1_2_0)
set(SHIELD twist_v1_4_1)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "")
# Configure Zephyr
cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(owntech_power_api)
target_sources(app PRIVATE ../src/main.cpp)
zephyr_include_directories(../src)
