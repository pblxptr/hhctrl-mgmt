cmake_minimum_required(VERSION 3.12)
project(hhctrl)

# add_definitions(-DCURL_STATICLIB=ON)
# set(CURL_LIBRARY ${CMAKE_SOURCE_DIR}/lib/curl)
# set(CURL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/curl/include)
# set(CURL_LINK_DIR ${CMAKE_BINARY_DIR}/lib/curl/lib)
# include_directories(${CURL_INCLUDE_DIR})
# link_directories( ${CURL_LINK_DIR} )

# add_subdirectory(lib/curl)

### CMAKE MODULES ###
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(CURL_PATH ${CMAKE_SOURCE_DIR}/lib/curl)
set(CURL_INCLUDE ${CURL_PATH}/include)
set(CURL_LIB ${CURL_PATH}/lib/.libs)
ExternalProject_Add(curl
  GIT_REPOSITORY    "https://github.com/curl/curl.git"
  GIT_TAG           master
  SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/curl
  BUILD_IN_SOURCE   true
  CONFIGURE_COMMAND autoreconf -fi ${CURL_PATH}/configure.ac && ./configure --disable-shared --enable-static --without-ssl --without-brotli
  BUILD_COMMAND     "make"
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
include_directories(${CURL_INCLUDE})
link_directories(${CURL_LIB})


add_subdirectory(src)