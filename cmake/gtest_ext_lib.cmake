### PATHS ###
set(GTEST_DIR         ${CMAKE_SOURCE_DIR}/lib/googletest)
set(GTEST_INCLUDE_DIR ${GTEST_DIR}/googletest/include)
set(GMOCK_INLCUDE_DIR ${GTEST_DIR}/googlemock/include)
set(GTEST_LIBRARY_DIR ${GTEST_DIR}/lib)

### LIB NAMES ###
set(GTEST_LIBRARY      -lgtest)
set(GTEST_MAIN_LIBRARY -lgtest_main)
set(GMOCK_LIBRARY      -lgmock)

ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG           master
  SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/googletest
  BUILD_IN_SOURCE   true
  CONFIGURE_COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/lib/googletest
  BUILD_COMMAND     make
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
include_directories(${GTEST_INCLUDE_DIR})
include_directories(${GMOCK_INLCUDE_DIR})
link_directories(${GTEST_LIBRARY_DIR})
