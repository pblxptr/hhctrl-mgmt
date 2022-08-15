cmake_minimum_required(VERSION 3.12)

add_library(scheduler_tests OBJECT scheduler_test.cpp)
target_link_libraries(scheduler_tests common_scheduler GTest::gtest)