cmake_minimum_required (VERSION 2.8)
project (dmrender)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../../include)

file(GLOB SRC
  "src/*.cpp"
  "src/*.h")

add_executable(dmrender ${SRC})
target_link_libraries(dmrender dmusic)
