cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

find_package(icon CONFIG REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Protobuf REQUIRED)

set(COMMON_SOURCES
  common/scheduler/scheduler.cpp
  common/scheduler/file_task_store.cpp
)

set(MGMT_SOURCES
  app/main2.cpp
  static/src/pdtree.cpp
  static/src/sysfs_hatch.cpp
  static/src/sysfs_led.cpp
  static/src/devicetree.cpp
  static/src/rgb_indicator.cpp
)

set(SOURCES
  ${COMMON_SOURCES}
  ${MGMT_SOURCES}
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

add_dependencies(${CMAKE_PROJECT_NAME}
  date
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
  ${protobuf_INCLUDES}
  ${iconnect_INCLUDES}
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(${CMAKE_PROJECT_NAME}
  stdc++fs
  pthread
  boost_date_time
  boost_json
  spdlog::spdlog
  fmt::fmt
  zmq
  icon::icon
  ${Protobuf_LIBRARIES}
  sodium
  ${DATE_TZ_LIBRARY}
  ${iconnect_LIBRARY}
)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)

