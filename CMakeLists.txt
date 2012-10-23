cmake_minimum_required(VERSION 2.6)
project(GENART)

# All header files are specified relative to base dir.
include_directories(${PROJECT_SOURCE_DIR})

# Set some basic build flags.
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Werror -std=c++0x")

############################################################
# Configure required libraries

# gtest
if(BUILD_TESTING)
  add_subdirectory("/usr/src/gtest/" "${CMAKE_BINARY_DIR}/gtest")
endif(BUILD_TESTING)
include(CTest)

# Protobuf library
include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

# Boost
find_package(Boost COMPONENTS thread REQUIRED)

set(APP_LIBRARIES
  image
  poly
  util
  gflags
  pthread
  GL
  GLU
  X11
  ${PROTOBUF_LIBRARIES}
  ${Boost_THREAD_LIBRARY})

############################################################
# Build targets

# Sub libraries
add_subdirectory(image)
add_subdirectory(poly)
add_subdirectory(util)

# gen_pic
include_directories(${PROJECT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS poly/params.proto)

set(SRCS
  gen_pic.cc)
add_executable(genpic ${SRCS})
target_link_libraries(genpic ${APP_LIBRARIES})