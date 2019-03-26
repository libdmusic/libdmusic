mkdir build_static
cd build_static
IF %PLATFORM%==x86 (
   cmake -DVCPKG_TRIPLET=x86-windows-static -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake ..
)
IF %PLATFORM%==x64 (
   cmake -DVCPKG_TRIPLET=x64-windows-static -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 15 2017 Win64" ..
)
cmake --build . --config Release
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cd ..

mkdir build_shared
cd build_shared
IF %PLATFORM%==x86 (
   cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TRIPLET=x86-windows -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake ..
)
IF %PLATFORM%==x64 (
   cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 15 2017 Win64" ..
)
cmake --build . --config Release
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo