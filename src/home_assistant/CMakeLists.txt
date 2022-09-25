cmake_minimum_required(VERSION 3.12)

find_package(mqtt_cpp REQUIRED)

add_library(home_assistant STATIC
  main_board_event_handler.cpp
  main_board_handler.cpp
  hatch_event_handler.cpp
  hatch_handler.cpp
  temp_sensor_event_handler.cpp
  temp_sensor_handler.cpp
)

target_include_directories(home_assistant PUBLIC include)

target_link_libraries(home_assistant
  PRIVATE
    main_board device event common_utils
  PUBLIC
    mqtt_cpp::mqtt_cpp common_coro common_logger
)