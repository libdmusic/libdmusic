cmake_minimum_required (VERSION 3.4)
project (sampledump VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sampledump "src/sampledump.cpp")
target_include_directories(sampledump PRIVATE ../../include)
target_link_libraries(sampledump PRIVATE dmusic)
