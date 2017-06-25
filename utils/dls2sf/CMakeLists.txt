cmake_minimum_required (VERSION 2.6)
project (DLS2SF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/sf2cute)

include_directories(../../include)
include_directories(lib/sf2cute/include)

file(GLOB SRC
  "src/*.cpp"
  "src/*.h")

add_executable(dls2sf ${SRC})
target_link_libraries(dls2sf dmusic sf2cute)
