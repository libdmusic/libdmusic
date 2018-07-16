cmake_minimum_required (VERSION 3.4)
project (dmusic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

option(DMUSIC_STATIC "Builds dmusic as a static library (generally only needed on Windows)" OFF)
option(DMUSIC_BUILD_UTILS "Build various DirectMusic utilities" ON)
option(DMUSIC_TRACE "Enable tracing messages" OFF)
option(DMUSIC_TRACE_VERBOSE "Enable verbose tracing messages" OFF)
option(DMUSIC_TSF_SUPPORT "Enable reference InstrumentPlayer based on TinySoundFont" ON)
option(DMUSIC_DLS_PLAYER "Enable DLS InstrumentPlayer (requires libsndfile)" ON)

add_library(dmusic STATIC
  "src/Articulator.cpp"
  "src/decode.cpp"
  "src/DlsPlayer.cpp"
  "src/DownloadableSound.cpp"
  "src/DummyPlayer.cpp"
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

if (DMUSIC_BUILD_UTILS)
  add_subdirectory(utils)
endif ()

if (DMUSIC_TSF_SUPPORT)
  target_compile_definitions(dmusic PRIVATE DMUSIC_TSF_SUPPORT=1)
endif ()

if (DMUSIC_DLS_PLAYER)
  if(DMUSIC_STATIC)
    find_package(SndfileStatic REQUIRED)
    target_link_libraries(dmusic PRIVATE Sndfile::SndfileStatic)
  else()
    find_package(Sndfile REQUIRED)
    target_link_libraries(dmusic PRIVATE Sndfile::Sndfile)
  endif()

  target_compile_definitions(dmusic PRIVATE DMUSIC_DLS_PLAYER=1 DMUSIC_HAS_SNDFILE=1)

  if (NOT TARGET sf2cute)
    add_subdirectory(utils/dls2sf/lib/sf2cute)
  endif ()

  target_include_directories(dmusic PRIVATE utils/dls2sf/lib/sf2cute/include)
  target_link_libraries(dmusic PRIVATE sf2cute)
endif ()

if(DMUSIC_TRACE)
  target_compile_definitions(dmusic PRIVATE DMUSIC_TRACE=1)
  if(DMUSIC_TRACE_VERBOSE)
    target_compile_definitions(dmusic PRIVATE DMUSIC_TRACE_VERBOSE=1)
  endif()
endif()

target_include_directories(dmusic PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)