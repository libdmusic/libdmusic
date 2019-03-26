IF NOT EXIST vcpkg (
    git config --global user.name "Anonymous"
    git config --global user.email "anon@example.org"
    git clone https://github.com/REGoth-project/vcpkg.git
    cd vcpkg
    git merge --no-edit origin/sf2cute origin/rtaudio
    .\bootstrap-vcpkg.bat
    cd ..
)
cd vcpkg
IF %PLATFORM%==x86 (
    .\vcpkg.exe install --triplet x86-windows args rtaudio sf2cute libsndfile
    .\vcpkg.exe install --triplet x86-windows-static args rtaudio sf2cute libsndfile
)
IF %PLATFORM%==x64 (
    .\vcpkg.exe install --triplet x64-windows args rtaudio sf2cute libsndfile
    .\vcpkg.exe install --triplet x64-windows-static args rtaudio sf2cute libsndfile
)
cd ..