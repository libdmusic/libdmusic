<h4 align="center">
  <br>
  <img width="128" src="https://cdn.rawgit.com/frabert/libdmusic/3ea57d5/media/logo.svg">
  <br>
</h4>

<img width="256" src="https://cdn.rawgit.com/frabert/libdmusic/3ea57d5/media/logo_full.svg"> [![Linux build Status](https://travis-ci.org/frabert/libdmusic.svg?branch=master)](https://travis-ci.org/frabert/libdmusic) [![Windows build status](https://ci.appveyor.com/api/projects/status/7t7ral8wos4p7idc?svg=true)](https://ci.appveyor.com/project/frabert/libdmusic) [![Gitter chat](https://badges.gitter.im/frabert/libdmusic.png)](https://gitter.im/frabert/libdmusic)
=========

Project goals
-------------

`libdmusic` aims to provide a framework and suite of tools to enable applications and games playback of Microsoft DirectMusic audio files. The main aim of the project for now is to provide a way for the [REGoth project](https://github.com/REGoth-project/REGoth) to reproduce music files from the original Gothic and Gothic II games.

Compiling
---------

Pre-compiled binaries are available for both 32- and 64-bit windows in the [Releases](https://github.com/frabert/libdmusic/releases) section

Currently, `libsndfile` needs to be present on the system at the moment of compilation (also `rtaudio` if you want to build `dmplay`):
this can be ensured on Linux and macOS by installing the correct packages (e.g. `libsndfile1-dev` and `librtaudio-dev` on Debian/Ubuntu),
while on Windows it needs to be compiled manually:

````batch
REM Building libsndfile
$ git clone https://github.com/erikd/libsndfile
$ cd libsndfile
$ mkdir build
$ cd build
$ cmake -D CMAKE_INSTALL_PREFIX=%HOMEPATH%/libs ..
$ cmake --build .. --target INSTALL
$ cd ..

REM Building RtAudio (optional)
$ git clone https://github.com/thestk/rtaudio
$ cd rtaudio
$ mkdir build
$ cd build
$ cmake -D BUILD_TESTING=OFF -D CMAKE_INSTALL_PREFIX=%HOMEPATH%/libs ..
$ cmake --build .. --target INSTALL
$ cd ..

REM Building libdmusic
REM Add -D DMUSIC_BUILD_UTILS=OFF to disable building utilities
REM Disabling the utilities removes the need for RtAudio
$ git clone --recursive https://github.com/frabert/libdmusic
$ cmake -D CMAKE_PREFIX_PATH=%HOMEPATH/libs .
$ cmake --build .
````

On *nix systems, building is simpler (once you have installed libsndfile and rtaudio):

````sh
$ git clone --recursive https://github.com/frabert/libdmusic
$ cd libdmusic
$ cmake . # Add -D DMUSIC_BUILD_UTILS=OFF to disable building utilities and remove the dependency on RtAudio
$ make
````

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

`dls2sf` and `dmrender` use [libsndfile](http://www.mega-nerd.com/libsndfile/) for wave output and conversion.

`dls2sf` uses [sf2cute](https://github.com/gocha/sf2cute) for SoundFont output.

`dmplay` uses [RtAudio](http://www.music.mcgill.ca/~gary/rtaudio/) for realtime audio playback.
