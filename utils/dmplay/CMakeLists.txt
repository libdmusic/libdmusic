cmake_minimum_required (VERSION 2.8)
project (dmplay)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../../include)
include_directories(../common)
include_directories(lib/portaudio/include)

add_subdirectory(lib/portaudio)

file(GLOB SRC
  "src/*.cpp"
  "src/*.h"
  "src/*.hxx")

add_executable(dmplay ${SRC})
target_link_libraries(dmplay dmusic portaudio_static)