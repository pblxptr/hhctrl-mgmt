cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
  scheduler_test.cpp
)

add_executable(${CMAKE_PROJECT_NAME}_test ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME}_test
  ${CATCH2_MAIN_LIBRARY} ${CATCH2_LIRARY}
)

include(CTest)

catch_discover_tests(${CMAKE_PROJECT_NAME}_test)