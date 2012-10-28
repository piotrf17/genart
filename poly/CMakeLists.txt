# Build proto files.
include_directories(${PROJECT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS params.proto)

# Poly library.
set(SRCS
  fitness_l2_with_focii.cc
  genome.cc
  polygon.cc
  polygon_effect.cc
  polygon_mutator.cc
  polygon_render.cc
  ${PROTO_SRCS}
  ${PROTO_HDRS})
add_library(poly ${SRCS})

# Testing.
if(BUILD_TESTING)
  add_executable(polygon_mutator_test polygon_mutator_test.cc)
  target_link_libraries(polygon_mutator_test gtest_main poly util)
  add_test(PolygonMutatorTest ${PROJECT_BINARY_DIR}/poly/polygon_mutator_test)
endif(BUILD_TESTING)
