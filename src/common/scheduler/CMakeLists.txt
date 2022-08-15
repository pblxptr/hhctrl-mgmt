cmake_minimum_required(VERSION 3.12)

find_package(Boost COMPONENTS json)

add_library(common_scheduler STATIC
  scheduler.cpp
  file_task_store.cpp
)

target_include_directories(common_scheduler PUBLIC include)
target_link_libraries(common_scheduler
  PUBLIC
    spdlog::spdlog common_utils
  PRIVATE
    boost_json
)