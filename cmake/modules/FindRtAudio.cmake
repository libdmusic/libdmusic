find_path(RtAudio_INCLUDE_DIR NAMES RtAudio.h PATH_SUFFIXES rtaudio)
find_library(RtAudio_LIBRARY NAMES rtaudio)
mark_as_advanced(RtAudio_INCLUDE_DIR RtAudio_LIBRARY)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(RtAudio REQUIRED_VARS RtAudio_LIBRARY RtAudio_INCLUDE_DIR)

if(RtAudio_FOUND AND NOT TARGET RtAudio::RtAudio)
  add_library(RtAudio::RtAudio UNKNOWN IMPORTED)
  set_target_properties(RtAudio::RtAudio PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${RtAudio_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${RtAudio_INCLUDE_DIR}")
endif()