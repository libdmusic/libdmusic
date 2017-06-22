cmake_minimum_required (VERSION 2.6)
project (dmusic)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DMUSIC_BUILD_TESTBED "Build testbed" ON)

include_directories(include)

file(GLOB LIB_SRC
  "src/*.cpp"
  "src/*.h"
  "include/dmusic/*.h"
  "include/dmusic/dls/*.h")

add_library(dmusic STATIC ${LIB_SRC})

if (DMUSIC_BUILD_TESTBED)
  file(GLOB TESTBED_SRC
    "testbed/*.cpp"
    "testbed/*.h")

  add_executable(testbed ${TESTBED_SRC})
  target_link_libraries (testbed dmusic)
endif (DMUSIC_BUILD_TESTBED)
