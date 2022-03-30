cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

## TODO: Add separate cmake files, build as libraries ###

find_package(icon CONFIG REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Protobuf REQUIRED)

set(HW_SOURCES
  hw/bootstrap.cpp
  hw/board_ctrl/board_ctrl_server.cpp
  hw/drivers/misc/sysfs_hatch.cpp
  hw/drivers/led/sysfs_led.cpp
  hw/drivers/led/rgb3led.cpp
  hw/platform_device/pdtree.cpp
  hw/platform_device_ctrl/pdctrl_server.cpp
  hw/platform_device_ctrl/pdctrl_hatch_handler.cpp
  hw/platform_device_ctrl/pdctrl_rgbled_handler.cpp
)

set(COMMON_SOURCES
  common/scheduler/scheduler.cpp
  common/scheduler/file_task_store.cpp
)

set(MGMT_SOURCES
  mgmt/bootstrap.cpp
  mgmt/board_ctrl/board_ctrl_client.cpp
  mgmt/platform_device/hatch/hatch_pd_client.cpp
  mgmt/platform_device/hatch/hatch_pd.cpp
)

set(BOOTSTRAP_SOURCES
  main.cpp
)

set(SOURCES
  ${HW_SOURCES}
  ${COMMON_SOURCES}
  ${MGMT_SOURCES}
  ${BOOTSTRAP_SOURCES}
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

