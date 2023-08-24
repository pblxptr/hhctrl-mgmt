cmake_minimum_required(VERSION 3.12)

add_executable(home_assistant_itc2
  async_entity_client_test.cpp
)

target_include_directories(home_assistant_itc2 PRIVATE .)
target_link_libraries(home_assistant_itc2
  PRIVATE
    test_support
    home_assistant
)
