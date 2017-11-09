cmake_minimum_required (VERSION 2.8)
project (DLS2SF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/sf2cute)

set(BUILD_PROGRAMS OFF CACHE STRING "" FORCE)
set(BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(BUILD_TESTING OFF CACHE STRING "" FORCE)
set(BUILD_STATIC_LIBS OFF CACHE STRING "" FORCE)
set(BUILD_SHARED_LIBS ON CACHE STRING "" FORCE)
if (NOT TARGET sndfile-shared)
    add_subdirectory(lib/libsndfile)
endif()

include_directories(../../include)
include_directories(lib/sf2cute/include)
include_directories(lib/libsndfile/src)

file(GLOB SRC
  "src/*.cpp"
  "src/*.h")

file(GLOB SCRIPTS "scripts/*")

file(COPY ${SCRIPTS} DESTINATION "${CMAKE_BINARY_DIR}/bin/")

add_executable(dls2sf ${SRC})
target_link_libraries(dls2sf dmusic sf2cute sndfile-shared)
