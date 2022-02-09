
find_package(Protobuf REQUIRED)
find_package(icon)


set (Protobuf_IMPORT_DIRS ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
  bci/bci.proto
  eci/eci_hc.proto
)

add_library(iconnect STATIC ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(iconnect icon)