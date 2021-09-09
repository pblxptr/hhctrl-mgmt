set(DATE_PATH ${CMAKE_SOURCE_DIR}/lib/date)
set(DATE_INCLUDE_DIR ${DATE_PATH}/include)
set(DATE_LIB_DIR ${DATE_PATH})
set(DATE_TZ_LIBRARY -ldate-tz)
ExternalProject_Add(date
  GIT_REPOSITORY    "https://github.com/HowardHinnant/date.git"
  GIT_TAG           master
  SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/date
  BUILD_IN_SOURCE   true
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -DBUILD_TZ_LIB=ON -DUSE_SYSTEM_TZ_DB=ON ${DATE_PATH}
  BUILD_COMMAND     make
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
  DEPENDS           curl
)
include_directories(${DATE_INCLUDE_DIR})
link_directories(${DATE_LIB_DIR})
