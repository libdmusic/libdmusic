IF NOT EXIST vcpkg (
    git config --global user.name "Anonymous"
    git config --global user.email "anon@example.org"
    git clone https://github.com/REGoth-project/vcpkg.git
    cd vcpkg
    git merge --no-edit origin/sf2cute origin/rtaudio
    .\bootstrap-vcpkg.bat
    cd ..
)

IF "%LINKING%"=="static" (
    set TRIPLET=%PLATFORM%-windows-static
)

IF "%LINKING%"=="shared" (
    set TRIPLET=%PLATFORM%-windows
)

cd vcpkg
.\vcpkg.exe install --triplet %TRIPLET% args rtaudio sf2cute libsndfile
cd ..