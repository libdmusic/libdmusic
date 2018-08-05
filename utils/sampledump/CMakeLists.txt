cmake_minimum_required (VERSION 3.4)
project (sampledump VERSION 0.1)

add_executable(sampledump ${CMAKE_CURRENT_SOURCE_DIR}/src/sampledump.cpp)
target_include_directories(sampledump PRIVATE ../../include)
target_link_libraries(sampledump PRIVATE dmusic::dmusic)

target_compile_features(sampledump PUBLIC cxx_std_14)

add_executable(dmusic::sampledump ALIAS sampledump

include(GNUInstallDirs)

install(TARGETS sampledump
  EXPORT dmusic-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS}))
