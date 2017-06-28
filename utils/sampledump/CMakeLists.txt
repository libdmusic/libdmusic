cmake_minimum_required (VERSION 2.8)
project (sampledump)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../../include)

file(GLOB SRC
  "src/*.cpp"
  "src/*.h")

add_executable(sampledump ${SRC})
target_link_libraries(sampledump dmusic)
