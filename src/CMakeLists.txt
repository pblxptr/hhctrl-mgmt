cmake_minimum_required(VERSION 3.12)

add_compile_options(-Werror -Wextra -Wall -Wno-unused-parameter) #TODO(bielpa) Remove this unused once the async_mqtt has fixed unused variable

add_subdirectory(common)
add_subdirectory(event)
add_subdirectory(device)
add_subdirectory(poller)
add_subdirectory(main_board)
add_subdirectory(home_assistant_v2)
add_subdirectory(home_assistant_adapter)
add_subdirectory(app)
