find_path(ARGS_HXX args.hxx)

add_executable(dmplay "src/dmplay.cpp")

target_link_libraries(dmplay PRIVATE dmusic::dmusic)

if(UNIX AND NOT APPLE)
  find_package(Threads REQUIRED)
  target_link_libraries(dmplay PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
endif()

target_compile_features(dmplay PUBLIC cxx_std_14)

target_include_directories(dmplay PRIVATE ../../include ../common ${ARGS_HXX})

if(NOT DISABLE_INSTALL_TOOLS)
  install(
    TARGETS dmplay
    RUNTIME DESTINATION ${UTILS_DESTINATION}
  )
endif()