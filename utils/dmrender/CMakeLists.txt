cmake_minimum_required (VERSION 2.8)
project (dmrender)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../../include)
include_directories(../common)
include_directories(../dls2sf/lib/libsndfile/src)

set(BUILD_PROGRAMS OFF CACHE STRING "" FORCE)
set(BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(BUILD_TESTING OFF CACHE STRING "" FORCE)
set(BUILD_STATIC_LIBS OFF CACHE STRING "" FORCE)
set(BUILD_SHARED_LIBS ON CACHE STRING "" FORCE)

if (NOT TARGET sndfile-shared)
    add_subdirectory(../dls2sf/lib/libsndfile ${CMAKE_BINARY_DIR}/bin2)
endif()

file(GLOB SRC
  "src/*.cpp"
  "src/*.h"
  "src/*.hxx")

add_executable(dmrender ${SRC})
target_link_libraries(dmrender dmusic sndfile-shared)
