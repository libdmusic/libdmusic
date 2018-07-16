cmake_minimum_required (VERSION 3.4)
project (dls2sf VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/sf2cute)

add_executable(dls2sf "src/dls2sf.cpp")

target_include_directories(dls2sf PRIVATE lib/sf2cute/include)
target_include_directories(dls2sf PRIVATE ../common)

target_link_libraries(dls2sf PRIVATE dmusic sf2cute)
target_compile_definitions(dls2sf PRIVATE DMUSIC_HAS_SNDFILE=1)

if(DMUSIC_STATIC)
  find_package(SndfileStatic REQUIRED)
  target_link_libraries(dls2sf PRIVATE Sndfile::SndfileStatic)
else()
  find_package(Sndfile REQUIRED)
  target_link_libraries(dls2sf PRIVATE Sndfile::Sndfile)
endif(DMUSIC_STATIC)