add_executable(sampledump ${CMAKE_CURRENT_SOURCE_DIR}/src/sampledump.cpp)
target_include_directories(sampledump PRIVATE ../../include)
target_link_libraries(sampledump PRIVATE dmusic::dmusic)

target_compile_features(sampledump PUBLIC cxx_std_14)

if(NOT DISABLE_INSTALL_TOOLS)
  install(
    TARGETS sampledump
    RUNTIME DESTINATION ${UTILS_DESTINATION}
  )
endif()
