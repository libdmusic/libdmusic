cmake_minimum_required (VERSION 3.4)
project (dmplay VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dmplay "src/dmplay.cpp")

target_link_libraries(dmplay PRIVATE dmusic)

if(DMUSIC_STATIC)
  find_package(RtAudioStatic 5 REQUIRED)
  target_link_libraries(dmplay PRIVATE RtAudio::RtAudioStatic)
else()
  find_package(RtAudio 5 REQUIRED)
  target_link_libraries(dmplay PRIVATE RtAudio::RtAudio)
endif()

target_include_directories(dmplay PRIVATE ../../include)
target_include_directories(dmplay PRIVATE ../common)