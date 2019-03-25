git config --global user.name "Anonymous"
git config --global user.email "anon@example.org"
git clone https://github.com/REGoth-project/vcpkg.git
cd vcpkg
git merge --no-edit origin/sf2cute origin/rtaudio
cd ..