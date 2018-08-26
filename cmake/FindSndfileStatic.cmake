find_path(SndfileStatic_INCLUDE_DIR NAMES sndfile.h)
find_library(SndfileStatic_LIBRARY NAMES sndfile libsndfile)
mark_as_advanced(SndfileStatic_INCLUDE_DIR SndfileStatic_LIBRARY)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SndfileStatic REQUIRED_VARS SndfileStatic_LIBRARY SndfileStatic_INCLUDE_DIR)

if(SndfileStatic_FOUND)
  if(NOT TARGET Sndfile::SndfileStatic)
    add_library(Sndfile::SndfileStatic UNKNOWN IMPORTED)
    set_target_properties(Sndfile::SndfileStatic PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${SndfileStatic_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${SndfileStatic_INCLUDE_DIR}")
  endif()
endif()