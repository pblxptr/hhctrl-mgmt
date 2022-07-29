cmake_minimum_required(VERSION 3.12)

project(hhctrl-mgmt CXX)

set(CMAKE_ADDONS ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 20)

option(ENABLE_TESTING "Enable building and executing tests" OFF)

# add_compile_options(-Wno-deprecated -Wall -fsanitize=undefined -save-temps)
# add_link_options(-fsanitize=undefined)

### Protobuf ###
include_directories(${CMAKE_BINARY_DIR})

### CMAKE MODULES ###
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${CMAKE_ROOT}/Modules/FetchContent.cmake)

### LIRARIES ###
include(${CMAKE_ADDONS}/date_ext_lib.cmake)
# include(${CMAKE_ADDONS}/iconnect.cmake)
#include(${CMAKE_ADDONS}/mqtt_cpp.cmake)

add_subdirectory(src)

if (ENABLE_TESTING)
  enable_testing()
  include(${CMAKE_ADDONS}/catch2_ext_lib.cmake)
  add_subdirectory(test)
endif(ENABLE_TESTING)