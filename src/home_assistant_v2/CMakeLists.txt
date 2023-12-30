
find_package(spdlog REQUIRED)

find_package(Boost 1.82 REQUIRED COMPONENTS json)

add_library(home-assistant-v2 STATIC error.cpp)
target_include_directories(home-assistant-v2 PUBLIC include)
target_link_libraries(home-assistant-v2
  INTERFACE
    async_mqtt_iface common_utils tl::expected spdlog::spdlog Boost::json
)
