cmake_minimum_required(VERSION 3.12)

find_package(GTest REQUIRED)
find_package(Boost COMPONENTS json)
find_package(Catch2 REQUIRED)

add_subdirectory(common)
add_subdirectory(home_assistant)

add_executable(${CMAKE_PROJECT_NAME}_unittests test_main.cpp)
add_dependencies(${CMAKE_PROJECT_NAME}_unittests common_tests)
add_dependencies(${CMAKE_PROJECT_NAME}_unittests home_assistant_tests)

target_link_libraries(${CMAKE_PROJECT_NAME}_unittests
  PRIVATE
    GTest::gtest GTest::gmock boost_json Catch2::Catch2
    common_tests
    home_assistant_tests
)
target_compile_definitions(${CMAKE_PROJECT_NAME}_unittests PRIVATE
  GTEST_DONT_DEFINE_FAIL=1
  GTEST_DONT_DEFINE_SUCCEED=1
)
add_test(NAME UnitTests COMMAND ${CMAKE_PROJECT_NAME}_unittests)


#find_package(fmt)
#find_package(Boost 1.77 REQUIRED coroutine)

#
#include_directories(${CMAKE_SOURCE_DIR}/src)
#
#add_executable(${CMAKE_PROJECT_NAME}_unittests test_main/test_main.cpp
#  common/command_dispatcher_tests.cpp
#)
#target_link_libraries(${CMAKE_PROJECT_NAME}_unittests
#  ${CATCH2_LIRARY}
#  GTest::gmock
#  pthread
#)

#

