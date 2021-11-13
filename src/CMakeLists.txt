cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
  main.cpp
  scheduler.cpp
  file_task_store.cpp
  sysfs_led.cpp
  sysfs_hatch.cpp
  rgb_led_service.cpp
)
find_package(fmt)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

add_dependencies(${CMAKE_PROJECT_NAME} date)

target_link_libraries(${CMAKE_PROJECT_NAME}
  fmt::fmt
  stdc++fs -pthread -lboost_date_time -lboost_json
  ${CURL_LIBRARY}
  ${DATE_TZ_LIBRARY}
)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)