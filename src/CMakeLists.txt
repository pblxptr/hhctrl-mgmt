cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(mqtt_cpp REQUIRED)
find_package(Boost COMPONENTS date_time json log log_setup thread)

add_subdirectory(common)
add_subdirectory(event)
add_subdirectory(device)
add_subdirectory(poller)
add_subdirectory(main_board)
add_subdirectory(home_assistant)
add_subdirectory(app)

#cmake_minimum_required(VERSION 3.12)
#
#set(CMAKE_CXX_STANDARD 20)
#
#find_package(fmt REQUIRED)
#find_package(spdlog REQUIRED)
#find_package(mqtt_cpp REQUIRED)
#
#find_package(Boost COMPONENTS date_time json log log_setup thread)
#
#set(COMMON_SOURCES
#  common/scheduler/scheduler.cpp
#  common/scheduler/file_task_store.cpp
#)
#
#
#add_subdirectory(common)
#add_subdirectory(event)
#add_subdirectory(device)
#add_subdirectory(poller)
#add_subdirectory(main_board)
#add_subdirectory(home_assistant)
#add_subdirectory(app)
#
#set (HEADER_SOURCES
#        home_assistant/include/home_assistant/device_identity_provider.hpp
#        home_assistant/include/home_assistant/availability.hpp
#        home_assistant/include/home_assistant/entity_factory.hpp
#        home_assistant/include/home_assistant/device_identity.hpp
#        home_assistant/include/home_assistant/unique_id.hpp
#        home_assistant/include/home_assistant/mqtt/binary_sensor.hpp
#        home_assistant/include/home_assistant/mqtt/entity.hpp device/include/device/rgb_led_t.hpp
#        home_assistant/include/home_assistant/mqtt/button.hpp
#        poller/include/poller/polling_service.hpp
#        poller/include/poller/hatch_poller.hpp
#        poller/include/poller/poller_factory.hpp
#        poller/include/poller/main_board_poller.hpp
#    )
#
#set(MGMT_SOURCES
#  app/main.cpp
#  app_config.cpp
#)
#
#
#set(SOURCES
#  ${HEADER_SOURCES}
#  ${COMMON_SOURCES}
#  ${MGMT_SOURCES}
#        home_assistant/include/home_assistant/mqtt/entity_error.hpp)
#
#add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
#
#add_dependencies(${CMAKE_PROJECT_NAME}
#  date
#)
#
#target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
##   ${protobuf_INCLUDES}
##   ${iconnect_INCLUDES}
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
#)
#
#target_link_libraries(${CMAKE_PROJECT_NAME}
#  stdc++fs
#  pthread
#  boost_date_time
#  boost_json
#  boost_log
#  boost_log_setup
#  boost_thread
#  spdlog::spdlog
#  fmt::fmt
#  ${Protobuf_LIBRARIES}
#  ${DATE_TZ_LIBRARY}
#  mqtt_cpp::mqtt_cpp
#  home_assistant
#)
#
#install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)

