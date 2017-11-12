cmake_minimum_required (VERSION 2.6)
project (utils)

option(DMUSIC_BUILD_DLS2SF "Build DLS to SoundFont conversion utility" ON)
option(DMUSIC_BUILD_SAMPLEDUMP "Build DLS sample export utility" OFF)
option(DMUSIC_BUILD_DMRENDER "Build offline segment rendering utility" ON)
option(DMUSIC_BUILD_DMPLAY "Build realtime segment rendering utility" ON)

if (DMUSIC_BUILD_DLS2SF)
  add_subdirectory(dls2sf)
endif ()

if (DMUSIC_BUILD_SAMPLEDUMP)
  add_subdirectory(sampledump)
endif ()

if (DMUSIC_BUILD_DMRENDER)
  add_subdirectory(dmrender)
endif ()

if (DMUSIC_BUILD_DMPLAY)
  add_subdirectory(dmplay)
endif ()