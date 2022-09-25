cmake_minimum_required(VERSION 3.12)

find_package(GTest REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(unit_tests
  test_main.cpp
  command_dispatcher_tests.cpp
  scheduler_test.cpp
)
target_link_libraries(unit_tests
  PRIVATE
    GTest::gtest GTest::gmock Catch2::Catch2
    common_scheduler
    common_command
)
target_compile_definitions(unit_tests
  PRIVATE
    GTEST_DONT_DEFINE_FAIL=1
    GTEST_DONT_DEFINE_SUCCEED=1
)
add_test(NAME UnitTests COMMAND unittests)
