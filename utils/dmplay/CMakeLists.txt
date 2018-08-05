cmake_minimum_required (VERSION 3.4)
project (dmplay VERSION 0.1)

add_executable(dmplay "src/dmplay.cpp")

target_link_libraries(dmplay PRIVATE dmusic::dmusic)
target_compile_features(dmplay PUBLIC cxx_std_14)

add_executable(dmusic::dmplay ALIAS dmplay)

if(DMUSIC_STATIC)
  find_package(RtAudioStatic 5 REQUIRED)
  target_link_libraries(dmplay PRIVATE RtAudio::RtAudioStatic)
else()
  find_package(RtAudio 5 REQUIRED)
  target_link_libraries(dmplay PRIVATE RtAudio::RtAudio)
endif()

target_include_directories(dmplay PRIVATE ../../include)
target_include_directories(dmplay PRIVATE ../common)

include(GNUInstallDirs)

install(TARGETS dmplay
  EXPORT dmusic-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS})