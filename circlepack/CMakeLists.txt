include_directories(${PROJECT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(
  PROTO_SRCS
  PROTO_HDRS
  ../core/params.proto  # TODO(piotrf): fix this ugliness.
  circlepack_params.proto
)

# Circlepack library.
set(SRCS
  circlepack_genome.cc
  circlepack_genome_factory.cc
  circlepack_renderer.cc
  ${PROTO_SRCS}
  ${PROTO_HDRS}
)
add_library(circlepack ${SRCS})