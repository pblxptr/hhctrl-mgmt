cmake_minimum_required(VERSION 3.12)

find_package(Boost REQUIRED)

add_library(poller STATIC
  polling_service.cpp
)

target_include_directories(poller PUBLIC include)

target_link_libraries(poller
  PRIVATE
    Boost::coroutine device common_coro
    common_executor common_logger
)
