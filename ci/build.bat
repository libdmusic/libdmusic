IF %PLATFORM%==x86 (
    set GENERATOR="Visual Studio 15 2017"
)

IF %PLATFORM%==x64 (
    set GENERATOR="Visual Studio 15 2017 Win64"
)

IF "%LINKING%"=="static" (
    set TRIPLET=%PLATFORM%-windows-static
)

IF "%LINKING%"=="shared" (
    set TRIPLET=%PLATFORM%-windows
)

mkdir build
cd build
cmake -DVCPKG_TRIPLET=%TRIPLET% -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake .. -G %GENERATOR%
cmake --build . --config %CONFIGURATION%