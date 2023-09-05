
add_library(home-assistant-adapter STATIC
    event/main_board_event_handler.cpp
    event/hatch_event_handler.cpp
    event/temp_sensor_event_handler.cpp
    device/main_board.cpp

    hatch_handler.cpp
#    main_board_event_handler.cpp
#    main_board_handler.cpp
#    temp_sensor_event_handler.cpp
#    temp_sensor_handler.cpp
#    main_board.cpp
)
target_include_directories(home-assistant-adapter PUBLIC include)

target_link_libraries(home-assistant-adapter
    PRIVATE
        main_board device event common_utils
    PUBLIC
        home-assistant-v2 home_assistant common_coro common_logger
)
