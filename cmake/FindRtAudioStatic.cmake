find_path(RtAudioStatic_INCLUDE_DIR NAMES RtAudio.h PATH_SUFFIXES rtaudio)
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

  if (RtAudioStatic_INCLUDE_DIR AND EXISTS "${RtAudioStatic_INCLUDE_DIR}/RtAudio.h")
    file(STRINGS "${RtAudioStatic_INCLUDE_DIR}/RtAudio.h" rtaudio_version_str REGEX "^#define[ \t]+RTAUDIO_VERSION[ \t]+\".+\"")

    string(REGEX REPLACE "^#define[ \t]+RTAUDIO_VERSION[ \t]+\"([^\"]+)\".*" "\\1" RtAudioStatic_VERSION_STRING "${rtaudio_version_str}")
    unset(rtaudio_version_str)
  endif ()
endif()