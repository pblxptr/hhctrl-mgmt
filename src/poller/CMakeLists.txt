cmake_minimum_required(VERSION 3.12)

add_library(poller STATIC
  polling_service.cpp
)

target_include_directories(poller PUBLIC include)

target_link_libraries(poller PRIVATE device common_coro common_executor common_logger)