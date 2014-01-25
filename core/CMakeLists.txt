# Core genetic algorithms library.
include_directories(${PROJECT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PARAMS_PROTO_SRCS PARAMS_PROTO_HDRS params.proto)

set(SRCS
  fitness_l2_with_focii.cc
  genetic_effect.cc
  ${PARAMS_PROTO_SRCS})
add_library(core ${SRCS})

# Benchmarks.
#add_executable(fitness_benchmark fitness_benchmark.cc)
#target_link_libraries(fitness_benchmark image poly pthread X11 ${OpenCV_LIBS})
