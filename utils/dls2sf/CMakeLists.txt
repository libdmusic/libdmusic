cmake_minimum_required (VERSION 2.8)
project (DLS2SF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/sf2cute)
add_subdirectory(lib/libsndfile)

include_directories(../../include)
include_directories(lib/sf2cute/include)
include_directories(lib/libsndfile/src)

file(GLOB SRC
  "src/*.cpp"
  "src/*.h")

add_executable(dls2sf ${SRC})
target_link_libraries(dls2sf dmusic sf2cute sndfile-shared)
