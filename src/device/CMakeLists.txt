cmake_minimum_required(VERSION 3.12)

add_library(device STATIC
  rgb_indicator.cpp
)

target_include_directories(device PUBLIC include)
target_link_libraries(device PUBLIC common_logger PRIVATE main_board common_utils)