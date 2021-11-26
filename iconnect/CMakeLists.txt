
find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
  bci/bci.proto
  eci/eci_hc.proto
)

add_library(iconnect STATIC ${PROTO_SRCS} ${PROTO_HDRS})