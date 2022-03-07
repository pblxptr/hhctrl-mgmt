
### PATHS ###
set(iconnect_INCLUDE_DIR ${CMAKE_BINARY_DIR}/iconnect)
set(iconnect_LIBRARY_DIR ${CMAKE_BINARY_DIR}/iconnect)
set(iconnect_LIBRARY     iconnect)
set(iconnect_INCLUDES    ${CMAKE_BINARY_DIR}/iconnect)

link_directories(${iconnect_LIBRARY_DIR})

add_subdirectory(${CMAKE_SOURCE_DIR}/iconnect)