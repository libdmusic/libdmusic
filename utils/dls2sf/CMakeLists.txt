cmake_minimum_required (VERSION 2.8)
project (DLS2SF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/sf2cute)

find_package(LibSndFile REQUIRED)
get_target_property(sndfile_INCLUDE_DIRECTORIES sndfile-static INTERFACE_INCLUDE_DIRECTORIES)

include_directories(../../include)
include_directories(lib/sf2cute/include)
include_directories(../common)
include_directories(${sndfile_INCLUDE_DIRECTORIES})

file(GLOB SRC
  "src/*.cpp"
  "src/*.h"
  "../../src/decode.cpp")

file(GLOB SCRIPTS "scripts/*")

file(COPY ${SCRIPTS} DESTINATION "${CMAKE_BINARY_DIR}/bin/")

add_executable(dls2sf ${SRC})
target_link_libraries(dls2sf dmusic sf2cute)
target_compile_definitions(dls2sf PRIVATE DMUSIC_HAS_SNDFILE=1)

if(DMUSIC_COMPAT_GPL)
  target_link_libraries(dls2sf sndfile-static)
else()
  target_link_libraries(dls2sf sndfile-shared)
endif()
