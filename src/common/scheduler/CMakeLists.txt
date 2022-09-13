cmake_minimum_required(VERSION 3.12)

find_package(Boost REQUIRED)
find_package(fmt REQUIRED)

add_library(common_scheduler STATIC
  scheduler.cpp
  file_task_store.cpp
)

target_include_directories(common_scheduler PUBLIC include)
target_link_libraries(common_scheduler
  PUBLIC
    common_logger common_utils fmt::fmt Boost::boost
  PRIVATE
    boost_json
)
