cmake_minimum_required(VERSION 3.18)

message(STATUS "Conan HenhouseCtrl Management Wrapper")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(${CMAKE_SOURCE_DIR}/CMakeLists.txt)
