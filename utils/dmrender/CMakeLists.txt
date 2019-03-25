find_path(LIBSNDFILE_H sndfile.h)
find_library(LIBSNDFILE_LIB NAMES sndfile sndfile-1 libsndfile-1 libsndfile)
find_path(ARGS_HXX args.hxx)

add_executable(dmrender ${CMAKE_CURRENT_SOURCE_DIR}/src/dmrender.cpp)
target_link_libraries(dmrender PRIVATE dmusic::dmusic ${LIBSNDFILE_LIB})
target_compile_features(dmrender PUBLIC cxx_std_14)

target_include_directories(dmrender PRIVATE ../../include ../common ${LIBSNDFILE_H} ${ARGS_HXX})

if(NOT DISABLE_INSTALL_TOOLS)
  install(
    TARGETS dmrender
    RUNTIME DESTINATION ${UTILS_DESTINATION}
  )
endif()