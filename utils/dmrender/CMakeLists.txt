cmake_minimum_required (VERSION 3.4)
project (dmrender VERSION 0.1)

add_executable(dmrender ${CMAKE_CURRENT_SOURCE_DIR}/src/dmrender.cpp)
target_link_libraries(dmrender PRIVATE dmusic::dmusic)
target_compile_features(dmrender PUBLIC cxx_std_14)

add_executable(dmusic::dmrender ALIAS dmrender)

target_include_directories(dmrender PRIVATE ../../include)
target_include_directories(dmrender PRIVATE ../common)

if(DMUSIC_STATIC)
  find_package(SndfileStatic REQUIRED)
  target_link_libraries(dmrender PRIVATE Sndfile::SndfileStatic)
else()
  find_package(Sndfile REQUIRED)
  target_link_libraries(dmrender PRIVATE Sndfile::Sndfile)
endif(DMUSIC_STATIC)


include(GNUInstallDirs)

install(TARGETS dmrender
  EXPORT dmusic-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS})