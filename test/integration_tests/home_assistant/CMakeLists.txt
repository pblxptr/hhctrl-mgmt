cmake_minimum_required(VERSION 3.12)

add_executable(home_assistant_tests
        async_entity_client_tests.cpp test_context.cpp
)

target_include_directories(home_assistant_tests PRIVATE .)
target_link_libraries(home_assistant_tests
  PRIVATE
    test_support
    home_assistant
)
