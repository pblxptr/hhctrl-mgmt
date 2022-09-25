cmake_minimum_required(VERSION 3.12)

add_library(event INTERFACE)
target_include_directories(event INTERFACE include)
target_link_libraries(event INTERFACE common_event)