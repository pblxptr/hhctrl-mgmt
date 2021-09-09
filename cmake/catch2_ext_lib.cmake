### PATHS ###
set(CATCH2_DIR ${CMAKE_SOURCE_DIR}/lib/catch2)
set(CATCH2_INCLUDE_DIR ${CATCH2_DIR}/src)
set(CATCH2_LIBRARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/catch2/src)

### LIB NAMES ###
set(CATCH2_LIRARY -lCatch2)
set(CATCH2_MAIN_LIBRARY -lCatch2Main)

ExternalProject_Add(catch2
  GIT_REPOSITORY    "https://github.com/catchorg/Catch2.git"
  GIT_TAG           v3.0.0-preview3
  SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/catch2
  BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR}/catch2
  UPDATE_COMMAND    ""
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -DBUILD_TESTING=OFF ${CATCH2_DIR}
  BUILD_COMMAND     make # change to: make
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

include(${CATCH2_DIR}/extras/Catch.cmake)
include_directories(${CATCH2_INCLUDE_DIR})
link_directories(${CATCH2_LIBRARY_DIR})