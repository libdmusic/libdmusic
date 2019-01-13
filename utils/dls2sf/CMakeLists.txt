cmake_minimum_required (VERSION 3.4)
project (dls2sf VERSION 0.1)

add_executable(dls2sf ${CMAKE_CURRENT_SOURCE_DIR}/src/dls2sf.cpp)

target_include_directories(dls2sf PRIVATE ../common)
target_include_directories(dls2sf PRIVATE ../../sf2cute/include)
target_link_libraries(dls2sf PRIVATE dmusic::dmusic)
target_compile_features(dls2sf PUBLIC cxx_std_14)

add_executable(dmusic::dls2sf ALIAS dls2sf)

if(DMUSIC_STATIC)
  find_package(SndfileStatic REQUIRED)
  target_link_libraries(dls2sf PRIVATE Sndfile::SndfileStatic)
else()
  find_package(Sndfile REQUIRED)
  target_link_libraries(dls2sf PRIVATE Sndfile::Sndfile)
endif(DMUSIC_STATIC)

include(GNUInstallDirs)

install(TARGETS dls2sf
  EXPORT dmusic-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS})