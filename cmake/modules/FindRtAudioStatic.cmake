find_path(RtAudioStatic_INCLUDE_DIR NAMES RtAudio.h)
find_library(RtAudioStatic_LIBRARY NAMES rtaudio_static)
mark_as_advanced(RtAudioStatic_INCLUDE_DIR RtAudioStatic_LIBRARY)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(RtAudioStatic REQUIRED_VARS RtAudioStatic_LIBRARY RtAudioStatic_INCLUDE_DIR)

if(RtAudioStatic_FOUND AND NOT TARGET RtAudio::RtAudioStatic)
  add_library(RtAudio::RtAudioStatic UNKNOWN IMPORTED)
  set_target_properties(RtAudio::RtAudioStatic PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${RtAudioStatic_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${RtAudioStatic_INCLUDE_DIR}")
endif()