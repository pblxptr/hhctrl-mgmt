
### PATHS ###
set(ICONNECT_INCLUDE_DIR ${CMAKE_BINARY_DIR}/iconnect)
set(ICONNECT_LIBRARY_DIR ${CMAKE_BINARY_DIR}/iconnect)
set(ICONNECT_LIBRARY     iconnect)

add_subdirectory(${CMAKE_SOURCE_DIR}/iconnect)

include_directories(${ICONNECT_INCLUDE_DIR})
link_directories(${ICONNECT_LIBRARY_DIR})