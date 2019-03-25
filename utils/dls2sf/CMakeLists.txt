find_path(LIBSNDFILE_H sndfile.h)
find_library(LIBSNDFILE_LIB NAMES libsndfile-1 libsndfile)

find_path(ARGS_HXX args.hxx)

find_package(sf2cute REQUIRED)

add_executable(dls2sf ${CMAKE_CURRENT_SOURCE_DIR}/src/dls2sf.cpp)

target_include_directories(dls2sf PRIVATE ../common ${ARGS_HXX} ${LIBSNDFILE_H})
target_link_libraries(dls2sf PRIVATE dmusic::dmusic ${LIBSNDFILE_LIB} sf2cute::sf2cute)
target_compile_features(dls2sf PUBLIC cxx_std_14)

if(NOT DISABLE_INSTALL_TOOLS)
  install(
    TARGETS dls2sf
    RUNTIME DESTINATION ${UTILS_DESTINATION}
  )
endif()