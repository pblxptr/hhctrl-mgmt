cmake_minimum_required(VERSION 3.12)

add_library(command_tests OBJECT command_dispatcher_tests.cpp)
target_link_libraries(command_tests PRIVATE common_command Catch2::Catch2)
