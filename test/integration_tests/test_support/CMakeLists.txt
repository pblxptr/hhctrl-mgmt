cmake_minimum_required(VERSION 3.12)

find_package(fmt REQUIRED)
find_package(GTest REQUIRED)
find_package(Catch2 REQUIRED)

add_library(test_support STATIC test_main.cpp)

target_link_libraries(test_support
  PUBLIC
    GTest::gtest GTest::gmock Catch2::Catch2 fmt::fmt
)
target_include_directories(test_support PUBLIC include)

target_compile_definitions(test_support
  PRIVATE
    GTEST_DONT_DEFINE_FAIL=1
    GTEST_DONT_DEFINE_SUCCEED=1
)
