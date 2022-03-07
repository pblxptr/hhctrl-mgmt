
find_package(Protobuf REQUIRED)
find_package(icon REQUIRED)

if (icon_INCLUDES)
  configure_file(${icon_INCLUDES}/metadata/metadata.proto ${CMAKE_BINARY_DIR}/icon/metadata/metadata.proto COPYONLY)
  set(Protobuf_IMPORT_DIRS ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
else()
  set(Protobuf_IMPORT_DIRS ${CMAKE_BINARY_DIR} ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set(PROTOBUF_GENERATE_CPP_APPEND_PATH FALSE)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
  bci/bci.proto
  # pdci/pdci_hatch.proto
)

add_library(iconnect STATIC ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(iconnect PUBLIC
  ${iconnect_INCLUDES}
  ${protobuf_INCLUDES}
)