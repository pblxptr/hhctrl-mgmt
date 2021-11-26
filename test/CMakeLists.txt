cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt)

set(SOURCES
  main_test.cpp
  ../src/common/scheduler/scheduler.cpp
  ../src/common/scheduler/file_task_store.cpp
)

set(TEST_SOURCES scheduler_test.cpp)

enable_testing()

add_executable(${CMAKE_PROJECT_NAME}_test ${TEST_SOURCES} ${SOURCES})

target_compile_definitions(${CMAKE_PROJECT_NAME}_test PRIVATE
  GTEST_DONT_DEFINE_FAIL=1
  GTEST_DONT_DEFINE_SUCCEED=1
)
target_include_directories(${CMAKE_PROJECT_NAME}_test PRIVATE ../src)
target_link_libraries(${CMAKE_PROJECT_NAME}_test
  ${CATCH2_LIRARY}
  ${GMOCK_LIBRARY}
  ${GTEST_LIBRARY}
  fmt::fmt
  stdc++fs -pthread -lboost_date_time -lboost_json ${CURL_LIBRARY}
  ${DATE_TZ_LIBRARY}
)

include(CTest)
catch_discover_tests(${CMAKE_PROJECT_NAME}_test)