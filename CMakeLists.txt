cmake_minimum_required(VERSION 3.12)

include(cmake/cmake_arg_list.cmake)

project(hhctrl-mgmt CXX)

add_compile_definitions(BOOST_ASIO_HAS_STD_COROUTINE)
add_compile_definitions(BOOST_ASIO_HAS_CO_AWAIT)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif ()

set(CMAKE_CXX_STANDARD 20)

option(ENABLE_TESTING "Enable building and executing tests" OFF)

### LIBRARIES ###
include(cmake/date_ext_lib.cmake)
include(cmake/mqtt_cpp.cmake)
include(cmake/range-v3.cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(src)

file(GLOB_RECURSE source_list "*.hpp")
add_library(headers INTERFACE ${source_list})

if (ENABLE_TESTING)
  enable_testing()
  include(cmake/catch2_ext_lib.cmake)
  add_subdirectory(test)
endif(ENABLE_TESTING)
