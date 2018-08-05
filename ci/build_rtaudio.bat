IF NOT EXIST C:\build_rtaudio_x86 (
  IF NOT EXIST C:\rtaudio (
    git clone https://github.com/thestk/rtaudio C:/rtaudio
  )

  mkdir C:\build_rtaudio_x86
  cd C:\build_rtaudio_x86
  cmake -D CMAKE_INSTALL_PREFIX=C:/install_prefix_x86 -D BUILD_TESTING=OFF C:/rtaudio
  cmake --build . --target INSTALL --config Release
  cd C:\projects\libdmusic
)

IF NOT EXIST C:\build_rtaudio_x86-64 (
  IF NOT EXIST C:\rtaudio (
    git clone https://github.com/thestk/rtaudio C:/rtaudio
  )
  
  mkdir C:\build_rtaudio_x86-64
  cd C:\build_rtaudio_x86-64
  cmake -D CMAKE_INSTALL_PREFIX=C:/install_prefix_x86-64 -D BUILD_TESTING=OFF C:/rtaudio
  cmake --build . --target INSTALL --config Release
  cd C:\projects\libdmusic
)