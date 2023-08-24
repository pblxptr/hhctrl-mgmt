cmake_minimum_required(VERSION 3.12)

find_package(${MQTT_CPP_LIBNAME} REQUIRED)

add_subdirectory(v2)

add_library(home_assistant STATIC
  main_board_event_handler.cpp
  main_board_handler.cpp
  hatch_event_handler.cpp
  hatch_handler.cpp
  temp_sensor_event_handler.cpp
  temp_sensor_handler.cpp
        v2/include/home_assistant/mqtt/async_mqtt_client.hpp
)

target_include_directories(home_assistant PUBLIC include)

target_link_libraries(home_assistant
  PRIVATE
    main_board device event common_utils
  PUBLIC
    ${MQTT_CPP_LIBNAME}::${MQTT_CPP_LIBNAME} common_coro common_logger
    home-assistant-v2
)
