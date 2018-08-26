find_path(Sndfile_INCLUDE_DIR NAMES sndfile.h)
find_library(Sndfile_LIBRARY NAMES sndfile libsndfile sndfile-1 libsndfile-1)
mark_as_advanced(Sndfile_INCLUDE_DIR Sndfile_LIBRARY)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Sndfile REQUIRED_VARS Sndfile_LIBRARY Sndfile_INCLUDE_DIR)

if(Sndfile_FOUND)
  if(NOT TARGET Sndfile::Sndfile)
    add_library(Sndfile::Sndfile UNKNOWN IMPORTED)
    set_target_properties(Sndfile::Sndfile PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${Sndfile_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${Sndfile_INCLUDE_DIR}")
  endif()
endif()