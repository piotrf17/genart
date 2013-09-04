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
#find_package(Boost COMPONENTS thread REQUIRED)

# OpenCV.
find_package(OpenCV REQUIRED)

set(APP_LIBRARIES
  poly
  image
  util
  gflags
  pthread
  GL
  GLU
  X11
  ${PROTOBUF_LIBRARIES}
#  ${Boost_THREAD_LIBRARY}
  ${OpenCV_LIBS})

############################################################
# Build targets

# Sub libraries
add_subdirectory(image)
add_subdirectory(poly)
add_subdirectory(util)

# gen_pic
include_directories(${PROJECT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS poly/params.proto)

set(GENPIC_SRCS
  comparison_window.cc
  gen_pic.cc)
add_executable(genpic ${GENPIC_SRCS})
target_link_libraries(genpic ${APP_LIBRARIES})

# slideshow
add_executable(slideshow slideshow.cc)
target_link_libraries(slideshow ${APP_LIBRARIES})

# gen_movie
set(GENMOVIE_SRCS
  comparison_window.cc
  gen_movie.cc)
add_executable(genmovie ${GENMOVIE_SRCS})
target_link_libraries(genmovie ${APP_LIBRARIES})

# gen_convert
add_executable(genconvert gen_convert.cc)
target_link_libraries(genconvert ${APP_LIBRARIES})

# movie player
# TODO(piotrf): make a generic player/view for polygon images and movies.
# It should also be able to convert from them to actual images/movies.
add_executable(movie_player movie_player.cc)
target_link_libraries(movie_player ${APP_LIBRARIES})
