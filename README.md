<h4 align="center">
  <br>
  <img width="256" src="https://cdn.rawgit.com/frabert/libdmusic/f7aeed1b/media/logo.svg">
  <br>
</h4>

libdmusic [![Linux build Status](https://travis-ci.org/frabert/libdmusic.svg?branch=master)](https://travis-ci.org/frabert/libdmusic) [![Windows build status](https://ci.appveyor.com/api/projects/status/7t7ral8wos4p7idc?svg=true)](https://ci.appveyor.com/project/frabert/libdmusic) [![Gitter chat](https://badges.gitter.im/frabert/libdmusic.png)](https://gitter.im/frabert/libdmusic)
=========

Project goals
-------------

`libdmusic` aims to provide a framework and suite of tools to enable applications and games playback of Microsoft DirectMusic audio files. The main aim of the project for now is to provide a way for the [REGoth project](https://github.com/REGoth-project/REGoth) to reproduce music files from the original Gothic and Gothic II games.

Compiling
---------

Currently, `libsndfile` needs to be present on the system at the moment of compilation:
this can be ensured on Linux and macOS by installing the correct packages (e.g. `libsndfile1-dev` on Debian/Ubuntu),
while on Windows it needs to be compiled manually:

````batch
$ git clone --recursive https://github.com/frabert/libdmusic
$ mkdir build-sndfile
$ cd build-sndfile
$ cmake -D CMAKE_INSTALL_PREFIX=%HOMEPATH%/libsndfile libdmusic/utils/dls2sf/lib/libsndfile
$ cmake --build . --target install
$ cd ../libdmusic
$ cmake -D CMAKE_PREFIX_PATH=%HOMEPATH/libsndfile .
$ cmake --build .
````

On *nix systems, building is simpler (once you have installed libsndfile):

````sh
$ git clone --recursive https://github.com/frabert/libdmusic
$ cd libdmusic
$ cmake .
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

`dmrender` and `dmplay` use [TinySoundFont](https://github.com/schellingb/TinySoundFont) for SoundFont rendering and [args](https://github.com/Taywee/args).

`dls2sf` and `dmrender` use [libsndfile](http://www.mega-nerd.com/libsndfile/) for wave output and conversion.

`dls2sf` uses [sf2cute](https://github.com/gocha/sf2cute) for SoundFont output.

`dmplay` uses [portaudio](http://www.portaudio.com/) for realtime audio playback.
