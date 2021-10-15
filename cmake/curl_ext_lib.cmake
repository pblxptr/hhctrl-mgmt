find_package(CURL)

if (NOT CURL_FOUND)
  message("-- Adding ExternalProject_Add(curl)")
  set(CURL_DIR ${CMAKE_SOURCE_DIR}/lib/curl)
  set(CURL_INCLUDE_DIRS ${CURL_DIR}/include)
  set(CURL_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/curl/lib/.libs)

  ExternalProject_Add(curl
    GIT_REPOSITORY    "https://github.com/curl/curl.git"
    GIT_TAG           master
    SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/curl
    BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR}/curl
    UPDATE_COMMAND    ""
    CONFIGURE_COMMAND autoreconf -fi ${CURL_DIR}/configure.ac && ${CURL_DIR}/configure --disable-shared --enable-static --without-ssl --without-brotli --without-zlib
    BUILD_COMMAND     make
    INSTALL_COMMAND   ""
    TEST_COMMAND      ""
  )
endif(NOT CURL_FOUND)

### LIB NAMES ###
set(CURL_LIBRARY "-lcurl")

### Add to paths ###
include_directories(${CURL_INCLUDE_DIRS})
link_directories(${CURL_LIBRARIES})
