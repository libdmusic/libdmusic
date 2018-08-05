IF NOT EXIST C:\build_libsndfile_x86 (
  IF NOT EXIST C:\libsndfile (
    git clone https://github.com/erikd/libsndfile C:/libsndfile
  )

  mkdir C:\build_libsndfile_x86
  cd C:\build_libsndfile_x86
  cmake -D CMAKE_INSTALL_PREFIX=C:/install_prefix_x86 C:/libsndfile
  cmake --build . --target INSTALL --config Release
  cd C:\projects\libdmusic
)

IF NOT EXIST C:\build_libsndfile_x86-64 (
  IF NOT EXIST C:\libsndfile (
    git clone https://github.com/erikd/libsndfile C:/libsndfile
  )

  mkdir C:\build_libsndfile_x86-64
  cd C:\build_libsndfile_x86-64
  cmake -G "Visual Studio 15 2017 Win64" -D CMAKE_INSTALL_PREFIX=C:/install_prefix_x86-64 C:/libsndfile
  cmake --build . --target INSTALL --config Release
  cd C:\projects\libdmusic
)