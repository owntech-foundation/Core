cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(owntech_power_api)

target_sources(app PRIVATE ../src/main.cpp)
zephyr_include_directories(../src)
