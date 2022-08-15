cmake_minimum_required(VERSION 3.12)

add_library(common_scheduler STATIC
  scheduler.cpp
  file_task_store.cpp
)

target_include_directories(common_scheduler PUBLIC include)