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
option(DMUSIC_TSF_SUPPORT "Enable reference InstrumentPlayer based on TinySoundFont" ON)
option(DMUSIC_COMPAT_GPL "Enable this to link LGPL modules statically instead of dynamically" OFF)
option(DMUSIC_DLS_PLAYER "Enable DLS InstrumentPlayer (requires libsndfile)" ON)

include_directories(include)

set(LIB_SRC
  "src/Articulator.cpp"
  "src/decode.cpp"
  "src/DlsPlayer.cpp"
  "src/DownloadableSound.cpp"
  "src/Exceptions.cpp"
  "src/Instrument.cpp"
  "src/MusicMessages.cpp"
  "src/PlayingContext.cpp"
  "src/Region.cpp"
  "src/Riff.cpp"
  "src/SoundFontPlayer.cpp"
  "src/Wave.cpp"
  "src/Forms/Band.cpp"
  "src/Forms/Chordmap.cpp"
  "src/Forms/ReferenceList.cpp"
  "src/Forms/Segment.cpp"
  "src/Forms/Style.cpp"
  "src/Forms/Tracks.cpp")

add_library(dmusic STATIC ${LIB_SRC})

if (DMUSIC_BUILD_UTILS)
  add_subdirectory(utils)
endif ()

if (DMUSIC_TSF_SUPPORT)
  target_compile_definitions(dmusic PRIVATE DMUSIC_TSF_SUPPORT=1)
endif ()

if (DMUSIC_DLS_PLAYER)
  find_package(LibSndFile REQUIRED)
  get_target_property(sndfile_INCLUDE_DIRECTORIES sndfile-static INTERFACE_INCLUDE_DIRECTORIES)

  include_directories(${sndfile_INCLUDE_DIRECTORIES})
  target_compile_definitions(dmusic PRIVATE DMUSIC_DLS_PLAYER=1 DMUSIC_HAS_SNDFILE=1)

  if (NOT TARGET sf2cute)
    add_subdirectory(utils/dls2sf/sf2cute)
  endif ()

  include_directories(utils/dls2sf/lib/sf2cute/include)
  target_link_libraries(dmusic sf2cute)
  
  if (DMUSIC_COMPAT_GPL)
    target_link_libraries(dmusic sndfile-static)
  else ()
    target_link_libraries(dmusic sndfile-shared)
  endif ()
endif ()

if(DMUSIC_TRACE)
  target_compile_definitions(dmusic PRIVATE DMUSIC_TRACE=1)
  if(DMUSIC_TRACE_VERBOSE)
    target_compile_definitions(dmusic PRIVATE DMUSIC_TRACE_VERBOSE=1)
  endif()
endif()
