IF NOT EXIST C:\build_rtaudio (
  git clone https://github.com/thestk/rtaudio C:/rtaudio
  mkdir C:\build_rtaudio
  cd C:\build_rtaudio
  cmake -D CMAKE_INSTALL_PREFIX=C:/install_prefix -D BUILD_TESTING=OFF C:/rtaudio
  cmake --build . --target INSTALL --config Release
  cd C:\projects\libdmusic
)