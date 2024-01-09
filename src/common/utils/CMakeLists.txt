cmake_minimum_required(VERSION 3.12)

find_package(fmt REQUIRED)

add_library(common_utils INTERFACE)

target_include_directories(common_utils INTERFACE include)
target_link_libraries(common_utils
  INTERFACE
    fmt::fmt date-tz common_logger
)
