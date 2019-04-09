cmake_minimum_required (VERSION 3.10)
project (dmusic VERSION 0.1.6 LANGUAGES CXX)

option(DMUSIC_BUILD_UTILS "Build various DirectMusic utilities" ON)
option(DMUSIC_TRACE "Enable tracing messages" OFF)
option(DMUSIC_TRACE_VERBOSE "Enable verbose tracing messages" OFF)

option(DMUSIC_FORCE_STATIC_CRT "Force the use of static runtime on Windows" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(DMUSIC_FORCE_STATIC_CRT)
  set(CompilerFlags
          CMAKE_CXX_FLAGS
          CMAKE_CXX_FLAGS_DEBUG
          CMAKE_CXX_FLAGS_RELEASE
          CMAKE_C_FLAGS
          CMAKE_C_FLAGS_DEBUG
          CMAKE_C_FLAGS_RELEASE
          )
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  endforeach()
endif()

find_package(sf2cute CONFIG REQUIRED)

add_library(dmusic "")
target_sources(dmusic
  PRIVATE
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Forms/Tracks.cpp)

target_compile_features(dmusic PUBLIC cxx_std_14)
target_link_libraries(dmusic PRIVATE sf2cute::sf2cute)

set_target_properties(dmusic PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(DMUSIC_TRACE)
  target_compile_definitions(dmusic PRIVATE DMUSIC_TRACE=1)
  if(DMUSIC_TRACE_VERBOSE)
    target_compile_definitions(dmusic PRIVATE DMUSIC_TRACE_VERBOSE=1)
  endif()
endif()

target_include_directories(dmusic
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(dmusic::dmusic ALIAS dmusic)

include(GNUInstallDirs)

set(UTILS_DESTINATION "bin" CACHE "PATH" "Utilities install directory")

# install library files needed for linking
install(
  TARGETS dmusic
  EXPORT dmusic-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# install the public header files
install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if (DMUSIC_BUILD_UTILS)
  add_subdirectory(utils)
endif ()

# installs the *cmake files in share directory
install(
  EXPORT dmusic-targets
  FILE dmusic-targets.cmake
  NAMESPACE dmusic::
  DESTINATION share/dmusic
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${CMAKE_BINARY_DIR}/cmake/dmusic-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/dmusic-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/dmusic-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/dmusic
)

install(
  FILES
    ${CMAKE_BINARY_DIR}/cmake/dmusic-config.cmake
    ${CMAKE_BINARY_DIR}/cmake/dmusic-config-version.cmake
  DESTINATION share/dmusic
)