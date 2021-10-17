cmake_minimum_required(VERSION 3.12)
project(hhctrl-mgmt)

set(CMAKE_ADDONS ${CMAKE_SOURCE_DIR}/cmake)

OPTION(BUILD_TESTS, "Build tests" OFF)

### CMAKE MODULES ###
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${CMAKE_ROOT}/Modules/FetchContent.cmake)

### LIRARIES ###
include(${CMAKE_ADDONS}/curl_ext_lib.cmake)
include(${CMAKE_ADDONS}/date_ext_lib.cmake)

add_subdirectory(src)

if (BUILD_TESTS)
  include(${CMAKE_ADDONS}/catch2_ext_lib.cmake)
  include(${CMAKE_ADDONS}/gtest_ext_lib.cmake)
  add_subdirectory(test)
endif(BUILD_TESTS)