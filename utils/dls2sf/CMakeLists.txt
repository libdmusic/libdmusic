find_path(ARGS_HXX args.hxx)

find_package(sf2cute CONFIG REQUIRED)

add_executable(dls2sf ${CMAKE_CURRENT_SOURCE_DIR}/src/dls2sf.cpp)

target_include_directories(dls2sf PRIVATE ../common ${ARGS_HXX})
target_link_libraries(dls2sf PRIVATE dmusic::dmusic sf2cute::sf2cute)
target_compile_features(dls2sf PUBLIC cxx_std_14)

if(NOT DISABLE_INSTALL_TOOLS)
  install(
    TARGETS dls2sf
    RUNTIME DESTINATION ${UTILS_DESTINATION}
  )
endif()