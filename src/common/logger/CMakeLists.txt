cmake_minimum_required(VERSION 3.12)

find_package(spdlog REQUIRED)

add_library(common_logger INTERFACE)

target_include_directories(common_logger INTERFACE include)
target_link_libraries(common_logger INTERFACE spdlog::spdlog)
