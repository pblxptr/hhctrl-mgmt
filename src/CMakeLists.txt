cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
  main.cpp
  sysfs_hatch.cpp
  scheduler.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} stdc++fs)

