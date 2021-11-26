cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

## TODO: Add separate cmake files, build as libraries ###

set(HW_SOURCES
  hw/drivers/sysfs_hatch.cpp
  hw/drivers/sysfs_led.cpp
  hw/services/rgb_led_service.cpp
)

set(COMMON_SOURCES
  common/scheduler/scheduler.cpp
  common/scheduler/file_task_store.cpp
)

set(CORE_SOURCES
  core/main.cpp
)

set(SOURCES
  ${HW_SOURCES}
  ${COMMON_SOURCES}
  ${CORE_SOURCES}
)
find_package(fmt)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

add_dependencies(${CMAKE_PROJECT_NAME}
  date
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(${CMAKE_PROJECT_NAME}
  fmt::fmt
  stdc++fs -pthread -lboost_date_time -lboost_json
  ${CURL_LIBRARY}
  ${DATE_TZ_LIBRARY}
  ${ICONNECT_LIBRARY}
)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)

