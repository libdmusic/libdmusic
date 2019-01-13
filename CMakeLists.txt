cmake_minimum_required (VERSION 3.4)
project (dmusic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(DMUSIC_STATIC "Builds dmusic as a static library (generally only needed on Windows)" OFF)
option(DMUSIC_BUILD_UTILS "Build various DirectMusic utilities" ON)
option(DMUSIC_TRACE "Enable tracing messages" OFF)
option(DMUSIC_TRACE_VERBOSE "Enable verbose tracing messages" OFF)

add_library(dmusic STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Articulator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/decode.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DlsPlayer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DownloadableSound.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DummyPlayer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Exceptions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Instrument.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MusicMessages.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PlayingContext.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Region.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Riff.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SoundFontPlayer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Wave.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Forms/Band.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Forms/Chordmap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Forms/ReferenceList.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Forms/Segment.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Forms/Style.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Forms/Tracks.cpp
  
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/file.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/file_writer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/generator_item.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/instrument.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/instrument_zone.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/modulator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/modulator_key.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/modulator_item.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/preset.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/preset_zone.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff_ibag_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff_igen_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff_imod_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff_inst_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff_pbag_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff_pgen_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff_phdr_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff_pmod_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff_shdr_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/riff_smpl_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/sample.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sf2cute/src/sf2cute/zone.cpp)

target_include_directories(dmusic PRIVATE sf2cute/include)

add_library(dmusic::dmusic ALIAS dmusic)
target_compile_features(dmusic PUBLIC cxx_std_14)

set_target_properties(dmusic PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if (DMUSIC_BUILD_UTILS)
  add_subdirectory(utils)
endif ()

if(DMUSIC_STATIC)
  find_package(SndfileStatic REQUIRED)
  target_link_libraries(dmusic PRIVATE Sndfile::SndfileStatic)
  set(SndfileConfig FindSndfileStatic)
else()
  find_package(Sndfile REQUIRED)
  target_link_libraries(dmusic PRIVATE Sndfile::Sndfile)
  set(SndfileConfig FindSndfile)
endif()

if(DMUSIC_TRACE)
  target_compile_definitions(dmusic PRIVATE DMUSIC_TRACE=1)
  if(DMUSIC_TRACE_VERBOSE)
    target_compile_definitions(dmusic PRIVATE DMUSIC_TRACE_VERBOSE=1)
  endif()
endif()

target_include_directories(dmusic PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>)

include(GNUInstallDirs)

install(TARGETS dmusic
  EXPORT dmusic-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/../
)

install(EXPORT dmusic-targets
  FILE dmusic-targets.cmake
  NAMESPACE dmusic::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dmusic
)

include(CMakePackageConfigHelpers)

if(DMUSIC_STATIC)
  configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/dmusic-config_static.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/dmusic-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dmusic
  )
else()
  configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/dmusic-config.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/dmusic-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dmusic
  )
endif()

write_basic_package_version_file(
  ${CMAKE_BINARY_DIR}/cmake/dmusic-config-version.cmake
  VERSION ${DMUSIC_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(
  FILES
    ${CMAKE_BINARY_DIR}/cmake/dmusic-config.cmake
    ${CMAKE_BINARY_DIR}/cmake/dmusic-config-version.cmake
    ${CMAKE_CURRENT_LIST_DIR}/cmake/${SndfileConfig}.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dmusic
)