include_directories(${PROJECT_BINARY_DIR})

# Circlepack library.
set(SRCS
  circlepack_genome.cc
  circlepack_genome_factory.cc
  circlepack_renderer.cc
)
add_library(circlepack ${SRCS})