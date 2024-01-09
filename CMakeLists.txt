cmake_minimum_required(VERSION 3.12)

project(hhctrl-mgmt LANGUAGES CXX VERSION 1.0.1)

add_compile_definitions(BOOST_ASIO_HAS_STD_COROUTINE)
add_compile_definitions(BOOST_ASIO_HAS_CO_AWAIT)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif ()

set(CMAKE_CXX_STANDARD 20)

option(ENABLE_TESTING "Enable building and executing tests" OFF)

include(cmake/sanitizers.cmake)
include(cmake/date_ext_lib.cmake)
include(cmake/mqtt_cpp.cmake)
include(cmake/async_mqtt.cmake)
include(cmake/range-v3.cmake)
include(cmake/expected.cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if (USE_ASAN OR USE_LSAN OR USE_UBSAN)
  enable_sanitizers()
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(src)

if (ENABLE_TESTING)
  enable_testing()
  include(cmake/catch2_ext_lib.cmake)
  add_subdirectory(test)
endif(ENABLE_TESTING)
