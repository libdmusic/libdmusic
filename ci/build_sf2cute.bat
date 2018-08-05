IF NOT EXIST C:\build_sf2cute_x86 (
  IF NOT EXIST C:\sf2cute (
    git clone https://github.com/frabert/sf2cute C:/sf2cute
  )

  mkdir C:\build_sf2cute_x86
  cd C:\build_sf2cute_x86
  cmake -D CMAKE_INSTALL_PREFIX=C:/install_prefix_x86 -D BUILD_TESTING=OFF C:/sf2cute
  cmake --build . --target INSTALL --config Release
  cd C:\projects\libdmusic
)

IF NOT EXIST C:\build_sf2cute_x86-64 (
  IF NOT EXIST C:\sf2cute (
    git clone https://github.com/frabert/sf2cute C:/sf2cute
  )

  mkdir C:\build_sf2cute_x86-64
  cd C:\build_sf2cute_x86-64
  cmake -D CMAKE_INSTALL_PREFIX=C:/install_prefix_x86-64 -D BUILD_TESTING=OFF C:/sf2cute
  cmake --build . --target INSTALL --config Release
  cd C:\projects\libdmusic
)