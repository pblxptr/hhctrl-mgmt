cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt)
find_package(Boost 1.77 REQUIRED coroutine)
find_package(GTest)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(${CMAKE_PROJECT_NAME}_unittests main_test.cpp
  common/command_dispatcher_tests.cpp
)
target_link_libraries(${CMAKE_PROJECT_NAME}_unittests
  ${CATCH2_LIRARY}
  GTest::gmock
  pthread
)
target_compile_definitions(${CMAKE_PROJECT_NAME}_unittests PRIVATE
  GTEST_DONT_DEFINE_FAIL=1
  GTEST_DONT_DEFINE_SUCCEED=1
)

add_test(NAME UnitTests COMMAND ${CMAKE_PROJECT_NAME}_unittests)
