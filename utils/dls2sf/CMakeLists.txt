cmake_minimum_required (VERSION 2.8)
project (DLS2SF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/sf2cute)

find_package(SndFile REQUIRED)

include_directories(../../include)
include_directories(lib/sf2cute/include)
include_directories(../common)
include_directories(${SNDFILE_INCLUDE_DIRS})

file(GLOB SRC
  "src/*.cpp"
  "src/*.h"
  "../../src/decode.cpp")

file(GLOB SCRIPTS "scripts/*")

file(COPY ${SCRIPTS} DESTINATION "${CMAKE_BINARY_DIR}/bin/")

add_executable(dls2sf ${SRC})
target_link_libraries(dls2sf dmusic sf2cute)
target_compile_definitions(dls2sf PRIVATE DMUSIC_HAS_SNDFILE=1)

target_link_libraries(dls2sf ${SNDFILE_LIBRARIES})
