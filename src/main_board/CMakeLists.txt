cmake_minimum_required(VERSION 3.12)

find_package(Boost REQUIRED COMPONENTS json)

add_library(main_board STATIC
  main_board.cpp
  devicetree.cpp
  sysfs_cpu_temp_sensor.cpp
  sysfs_ds18b20.cpp
  sysfs_hatch.cpp
  sysfs_led.cpp
  sysfs_rgbled.cpp
  pdtree.cpp
)

target_include_directories(main_board PUBLIC include)
target_link_libraries(main_board
  PRIVATE
    device common_utils common_logger
  PUBLIC
    Boost::json
    range-v3
)

