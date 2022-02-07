cmake_minimum_required(VERSION 3.12)

project(hhctrl-mgmt CXX)

set(CMAKE_ADDONS ${CMAKE_SOURCE_DIR}/cmake)

OPTION(BUILD_TESTS, "Build tests" OFF)

add_compile_options("-Wno-deprecated")

### Protobuf ###
include_directories(${CMAKE_BINARY_DIR})

### CMAKE MODULES ###
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${CMAKE_ROOT}/Modules/FetchContent.cmake)

### LIRARIES ###
# include(${CMAKE_ADDONS}/curl_ext_lib.cmake)
include(${CMAKE_ADDONS}/date_ext_lib.cmake)
include(${CMAKE_ADDONS}/iconnect.cmake)

add_subdirectory(src)

# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()

if (BUILD_TESTS)
  include(${CMAKE_ADDONS}/catch2_ext_lib.cmake)
  include(${CMAKE_ADDONS}/gtest_ext_lib.cmake)
  add_subdirectory(test)
endif(BUILD_TESTS)