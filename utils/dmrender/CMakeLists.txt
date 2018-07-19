cmake_minimum_required (VERSION 3.4)
project (dmrender VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dmrender "src/dmrender.cpp")
target_link_libraries(dmrender PRIVATE dmusic)
target_compile_definitions(dmrender PRIVATE DMUSIC_HAS_SNDFILE=1 DMUSIC_DLS_PLAYER=1)

target_include_directories(dmrender PRIVATE ../../include)
target_include_directories(dmrender PRIVATE ../common)

if(DMUSIC_STATIC)
  find_package(SndfileStatic REQUIRED)
  target_link_libraries(dmrender PRIVATE Sndfile::SndfileStatic)
else()
  find_package(Sndfile REQUIRED)
  target_link_libraries(dmrender PRIVATE Sndfile::Sndfile)
endif(DMUSIC_STATIC)