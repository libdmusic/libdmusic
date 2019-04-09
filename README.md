<h4 align="center">
  <br>
  <img width="128" src="https://rawcdn.githack.com/frabert/libdmusic/4d70914185a8ab042bcb7cfb9d6c6379baaf4aaf/media/logo.svg">
  <br>
</h4>

<img width="256" src="https://rawcdn.githack.com/frabert/libdmusic/4d70914185a8ab042bcb7cfb9d6c6379baaf4aaf/media/logo_full.svg"> [![Build status](https://ci.appveyor.com/api/projects/status/7t7ral8wos4p7idc?svg=true)](https://ci.appveyor.com/project/frabert/libdmusic) [![Gitter chat](https://badges.gitter.im/frabert/libdmusic.png)](https://gitter.im/frabert/libdmusic)
=========

Project goals
-------------

`libdmusic` aims to provide a framework and suite of tools to enable applications and games playback of Microsoft DirectMusic audio files. The main aim of the project for now is to provide a way for the [REGoth project](https://github.com/REGoth-project/REGoth) to reproduce music files from the original Gothic and Gothic II games.

Compiling
---------

Pre-compiled binaries are available for both 32- and 64-bit windows in the [Releases](https://github.com/frabert/libdmusic/releases) section

The easiest way to build `libdmusic` is to use [vcpkg](https://github.com/Microsoft/vcpkg).
Once vcpkg is installed, you can retrieve the needed dependencies:

    ./vcpkg install args sf2cute

Then configure and build the cmake project:

    git clone https://github.com/frabert/libdmusic
    cd libdmusic
    mkdir build
    cd build
    cmake -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..
    cmake --build .

Usage
-----

You can reproduce segment files (`.sgt`) in one of two ways:

- Rendering them to wave files using [dmrender](utils/dmrender/README.md)
- Playing them in realtime using [dmplay](utils/dmplay/README.md)

It is also possible to use the provided [dls2sf](utils/dls2sf/README.md) utility to convert DLS files to SF2 files.

Acknowledgements
----------------

Many thanks to [Mirza Zulfan](https://github.com/mirzazulfan) for kindly donating the logo!

`dmrender` and `dmplay` use [TinySoundFont](https://github.com/schellingb/TinySoundFont) for SoundFont rendering and [args](https://github.com/Taywee/args).

`dls2sf` and `dmrender` use [dr_wav.h](https://github.com/mackron/dr_libs/blob/master/dr_wav.h) for wave output and conversion.

`dls2sf` uses [sf2cute](https://github.com/gocha/sf2cute) for SoundFont output.

`dmplay` uses [miniaudio](https://github.com/dr-soft/miniaudio) for realtime audio playback.
