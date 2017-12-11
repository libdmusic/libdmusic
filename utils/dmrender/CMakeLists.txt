cmake_minimum_required (VERSION 2.8)
project (dmrender)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LibSndFile REQUIRED)
get_target_property(sndfile_INCLUDE_DIRECTORIES sndfile-static INTERFACE_INCLUDE_DIRECTORIES)

include_directories(../../include)
include_directories(../common)
include_directories(${sndfile_INCLUDE_DIRECTORIES})

file(GLOB SRC
  "src/*.cpp"
  "src/*.h"
  "src/*.hxx")

add_executable(dmrender ${SRC})
target_link_libraries(dmrender dmusic)
target_compile_definitions(dmrender PRIVATE DMUSIC_HAS_SNDFILE=1 DMUSIC_DLS_PLAYER=1)
if(DMUSIC_COMPAT_GPL)
  target_link_libraries(dmrender sndfile-static)
else()
  target_link_libraries(dmrender sndfile-shared)
endif()
