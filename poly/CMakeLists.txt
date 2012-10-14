# Build proto files.
include_directories(${PROJECT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS params.proto)

# Poly library.
set(SRCS
  fitness_l2_with_focii.cc
  genome.cc
  polygon.cc
  polygon_effect.cc
  polygon_render.cc
  ${PROTO_SRCS}
  ${PROTO_HDRS})
add_library(poly ${SRCS})