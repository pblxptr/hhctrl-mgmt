cmake_minimum_required(VERSION 3.12)

add_library(home_assistant_tests OBJECT
  async_entity_client_tests.cpp
)
target_link_libraries(home_assistant_tests PRIVATE home_assistant Catch2::Catch2)
