set(CURL_PATH ${CMAKE_SOURCE_DIR}/lib/curl)
set(CURL_INCLUDE_DIR ${CURL_PATH}/include)
set(CURL_LIBRARY_DIR ${CURL_PATH}/lib/.libs)
set(CURL_LIBRARY "-lcurl")
ExternalProject_Add(curl
  GIT_REPOSITORY    "https://github.com/curl/curl.git"
  GIT_TAG           master
  SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/curl
  BUILD_IN_SOURCE   true
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND autoreconf -fi ${CURL_PATH}/configure.ac && ./configure --disable-shared --enable-static --without-ssl --without-brotli --without-zlib
  BUILD_COMMAND     "" # change to: make
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
include_directories(${CURL_INCLUDE_DIR})
link_directories(${CURL_LIBRARY_DIR})
