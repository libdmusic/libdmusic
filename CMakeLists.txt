cmake_minimum_required (VERSION 2.8)
project (dmusic)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(DMUSIC_BUILD_UTILS "Build various DirectMusic utilities" ON)
option(DMUSIC_TRACE "Enable tracing messages" OFF)
option(DMUSIC_TRACE_VERBOSE "Enable verbose tracing messages" OFF)

include_directories(include)

file(GLOB LIB_SRC
  "src/*.cpp"
  "src/Forms/*.cpp"
  "src/*.h"
  "include/dmusic/*.h"
  "include/dmusic/dls/*.h")

add_library(dmusic STATIC ${LIB_SRC})

if (DMUSIC_BUILD_UTILS)
  add_subdirectory(utils)
endif ()

if(DMUSIC_TRACE)
  target_compile_definitions(dmusic PRIVATE DMUSIC_TRACE=1)
  if(DMUSIC_TRACE_VERBOSE)
    target_compile_definitions(dmusic PRIVATE DMUSIC_TRACE_VERBOSE=1)
  endif()
endif()
