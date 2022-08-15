#include(ExternalProject)
#ExternalProject_Add(mqtt_cpp
#  LOG_DOWNLOAD ON
#  SOURCE_DIR ${PROJECT_SOURCE_DIR}/lib/mqtt_cpp
#  GIT_REPOSITORY https://github.com/redboltz/mqtt_cpp
#  GIT_TAG        master
#  STEP_TARGETS   build
#  CMAKE_ARGS
#    -DMQTT_BUILD_TESTS=OFF
#    -DMQTT_BUILD_EXAMPLES=OFF
#    -DMQTT_USE_LOG=ON
#    -DMQTT_STD_VARIANT=ON
#    -DMQTT_STD_OPTIONAL=ON
#    -DMQTT_STD_STRING_VIEW=ON
#    -DMQTT_STD_ANY=ON
#    -DMQTT_STD_SHARED_PTR_ARRAY=ON
#  INSTALL_COMMAND sudo make install
#)

find_package(mqtt_cpp REQUIRED)

if (mqtt_cpp_FOUND)
  message(INFO "mqtt_cpp: Found")
  add_compile_definitions(
    MQTT_STD_VARIANT
    MQTT_STD_OPTIONAL
    MQTT_STD_VARIANT
    MQTT_STD_STRING_VIEW
  )
else()
  message(FATAL_ERROR "mqtt_cpp: Not found!")
endif()
