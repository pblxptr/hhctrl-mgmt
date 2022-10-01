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

#TODO(pp): Refactor this
## This hack is required because mqtt_cpp in conan is delivered as mqtt_cpp whereas in CMake available on github
find_package(mqtt_cpp QUIET)
find_package(mqtt_cpp_iface QUIET)

if (mqtt_cpp_FOUND)
  set(MQTT_CPP_LIBNAME mqtt_cpp)
elseif(mqtt_cpp_iface_FOUND)
  set(MQTT_CPP_LIBNAME mqtt_cpp_iface)
endif()

if (MQTT_CPP_LIBNAME)
  message(STATUS "Found library mqttcpp with lib name: ${MQTT_CPP_LIBNAME}")
  add_compile_definitions(
    MQTT_STD_VARIANT
    MQTT_STD_OPTIONAL
    MQTT_STD_VARIANT
    MQTT_STD_STRING_VIEW
  )
else()
  message(FATAL_ERROR "mqtt_cpp: Not found!")
endif()
