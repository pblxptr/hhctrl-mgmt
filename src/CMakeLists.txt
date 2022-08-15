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

