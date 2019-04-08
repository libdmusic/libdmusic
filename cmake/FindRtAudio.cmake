find_package(RtAudio CONFIG)

if(NOT RTAUDIO_FOUND)
    find_path(RTAUDIO_INCLUDE_DIRS RtAudio.h)
    find_library(RTAUDIO_LIBRARIES rtaudio librtaudio)

    if((NOT RTAUDIO_INCLUDE_DIRS STREQUAL "RTAUDIO_INCLUDE_DIRS-NOTFOUND") AND (NOT RTAUDIO_LIBRARIES STREQUAL "RTAUDIO_LIBRARIES-NOTFOUND"))
        file(READ "${RTAUDIO_INCLUDE_DIRS}/RtAudio.h" _RTAUDIO_HEADER_FILE)
        if(_RTAUDIO_HEADER_FILE MATCHES "#define\\s+RTAUDIO_VERSION\\s+\"(\\d+)\\.(\\d+)\.(\\d+)\"")
            set(RTAUDIO_VERSION_MAJOR ${CMAKE_MATCH_1})
            set(RTAUDIO_VERSION_MINOR ${CMAKE_MATCH_2})
            set(RTAUDIO_VERSION_PATCH ${CMAKE_MATCH_3})
            set(RTAUDIO_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
        endif()

        add_library(RtAudio::rtaudio UNKNOWN IMPORTED)
        set_target_properties(RtAudio::rtaudio PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${RTAUDIO_INCLUDE_DIRS}"
            IMPORTED_LOCATION "${RTAUDIO_LIBRARIES}")

        set(RTAUDIO_FOUND TRUE)
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RtAudio
                                  REQUIRED_VARS RTAUDIO_LIBRARIES RTAUDIO_INCLUDE_DIRS
                                  VERSION_VAR RTAUDIO_VERSION)