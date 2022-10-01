cmake_minimum_required(VERSION 3.12)

add_compile_options(-Werror -Wextra -Wall -save-temps)

add_subdirectory(common)
add_subdirectory(event)
add_subdirectory(device)
add_subdirectory(poller)
add_subdirectory(main_board)
add_subdirectory(home_assistant)
add_subdirectory(app)


