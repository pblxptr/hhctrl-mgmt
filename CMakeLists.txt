cmake_minimum_required(VERSION 3.12)
project(hhctrl)

set(CMAKE_ADDONS ${CMAKE_SOURCE_DIR}/cmake)

### CMAKE MODULES ###
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

### LIRARIES ###
include(${CMAKE_ADDONS}/curl_ext_lib.cmake)
include(${CMAKE_ADDONS}/date_ext_lib.cmake)

add_subdirectory(src)