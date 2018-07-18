IF NOT EXIST C:\build_libsndfile (
  git clone https://github.com/erikd/libsndfile C:/libsndfile
  mkdir C:\build_libsndfile
  cd C:\build_libsndfile
  cmake -D CMAKE_INSTALL_PREFIX=C:/install_prefix C:/libsndfile
  cmake --build . --target INSTALL --config Release
  cd C:\projects\libdmusic
)