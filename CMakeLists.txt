cmake_minimum_required(VERSION 3.12)

project(hhctrl-mgmt CXX)

#add_compile_definitions(BOOST_ASIO_ENABLE_HANDLER_TRACKING)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif ()

set(CMAKE_ADDONS ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 20)

option(ENABLE_TESTING "Enable building and executing tests" OFF)
option(USE_SANITIZERS, "Use sanitizers (address, undefined)" OFF)

if (USE_SANITIZERS)
   add_compile_options(-fsanitize=undefined,address )
   add_link_options(-fsanitize=undefined,address)
endif()

### CMAKE MODULES ###
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${CMAKE_ROOT}/Modules/FetchContent.cmake)

### LIBRARIES ###
include(${CMAKE_ADDONS}/date_ext_lib.cmake)
include(${CMAKE_ADDONS}/mqtt_cpp.cmake)

add_subdirectory(src)

if (ENABLE_TESTING)
  enable_testing()
  include(${CMAKE_ADDONS}/catch2_ext_lib.cmake)
  add_subdirectory(test)
endif(ENABLE_TESTING)