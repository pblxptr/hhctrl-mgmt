cmake_minimum_required(VERSION 3.12)

add_executable(home_assistant_it_v2
  test_spec_setup.cpp
  async_client_test.cpp
  cover_test.cpp
  binary_sensor_test.cpp
        sensor_test.cpp
        button_test.cpp
)

target_include_directories(home_assistant_it_v2 PRIVATE .)
target_link_libraries(home_assistant_it_v2
  PRIVATE
    test_support
    home_assistant
)
