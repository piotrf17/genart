# Build proto files.
include_directories(${PROJECT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PARAMS_PROTO_SRCS PARAMS_PROTO_HDRS params.proto)
PROTOBUF_GENERATE_CPP(IMAGE_PROTO_SRCS IMAGE_PROTO_HDRS polygon_image.proto)

# Poly library.
set(SRCS
  animated_polygon_image.cc
  fitness_l2_with_focii.cc
  genome.cc
  polygon.cc
  polygon_effect.cc
  polygon_mutator.cc
  polygon_render.cc
  util.cc
  ${PARAMS_PROTO_SRCS}
  ${IMAGE_PROTO_SRCS})
add_library(poly ${SRCS})

# Testing.
if(BUILD_TESTING)
  add_executable(polygon_mutator_test polygon_mutator_test.cc)
  target_link_libraries(polygon_mutator_test gtest_main poly util)
  add_test(PolygonMutatorTest ${PROJECT_BINARY_DIR}/poly/polygon_mutator_test)
endif(BUILD_TESTING)

# Benchmarks.
add_executable(fitness_benchmark fitness_benchmark.cc)
target_link_libraries(fitness_benchmark image poly pthread X11)
