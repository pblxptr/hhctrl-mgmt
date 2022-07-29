cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

find_package(icon CONFIG REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Protobuf REQUIRED)
find_package(mqtt_cpp_iface REQUIRED)
find_package(Boost COMPONENTS date_time json log log_setup thread)

set(COMMON_SOURCES
  common/scheduler/scheduler.cpp
  common/scheduler/file_task_store.cpp
)

set (HEADER_SOURCES
  home_assistant/device_identity_provider.hpp
  home_assistant/availability.hpp
  home_assistant/entity_factory.hpp
  home_assistant/device_identity.hpp
  home_assistant/unique_id.hpp
  home_assistant/mqtt/binary_sensor.hpp
  home_assistant/mqtt/entity.hpp device/rgb_led_t.hpp
  home_assistant/mqtt/button.hpp
  device/polling_service.hpp
  poller/hatch_poller.hpp
  poller/poller_factory.hpp
  poller/main_board_poller.hpp
    )

set(MGMT_SOURCES
  app/main.cpp
  main_board.cpp
  pdtree.cpp
  sysfs_hatch.cpp
  sysfs_led.cpp
  devicetree.cpp
  rgb_indicator.cpp
  hatch_handler.cpp
  ha_hatch_event_handler.cpp
  ha_main_board_event_handler.cpp
        main_board_handler.cpp sysfs_rgbled.cpp polling_service.cpp
        app_config.cpp
        )


set(SOURCES
  ${HEADER_SOURCES}
  ${COMMON_SOURCES}
  ${MGMT_SOURCES}
        home_assistant/mqtt/entity_error.hpp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

add_dependencies(${CMAKE_PROJECT_NAME}
  date
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
#   ${protobuf_INCLUDES}
#   ${iconnect_INCLUDES}
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(${CMAKE_PROJECT_NAME}
  stdc++fs
  pthread
  boost_date_time
  boost_json
  boost_log
  boost_log_setup
  boost_thread
  spdlog::spdlog
  fmt::fmt
  # zmq
  # icon::icon
  ${Protobuf_LIBRARIES}
  sodium
  ${DATE_TZ_LIBRARY}
  ${iconnect_LIBRARY}
  mqtt_cpp_iface::mqtt_cpp_iface
)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)

