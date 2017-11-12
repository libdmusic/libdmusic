<h2 align="center">
  <br>
  <img width="256" src="https://cdn.rawgit.com/frabert/libdmusic/f7aeed1b/media/logo.svg">
  <br>
</h2>

libdmusic [![Linux build Status](https://travis-ci.org/frabert/libdmusic.svg?branch=master)](https://travis-ci.org/frabert/libdmusic) [![Windows build status](https://ci.appveyor.com/api/projects/status/7t7ral8wos4p7idc?svg=true)](https://ci.appveyor.com/project/frabert/libdmusic) [![Gitter chat](https://badges.gitter.im/frabert/libdmusic.png)](https://gitter.im/frabert/libdmusic)
=========

Project goals
-------------

`libdmusic` aims to provide a framework and suite of tools to enable applications and games playback of Microsoft DirectMusic audio files. The main aim of the project for now is to provide a way for the [REGoth project](https://github.com/REGoth-project/REGoth) to reproduce music files from the original Gothic and Gothic II games.

Roadmap
-------
- DLS to SoundFont conversion
- Realtime SoundFont playback
- Realtime DLS level 2 playback
- `.sgt` and `.sty` files loading

For now, the `dls2sf` utility does not support DLS articulators, so the envelopes will sound weird. Converting articulators is difficult because DLS level 1 (and especially level 2) specify many modulating sources and destinations which are not supported by the SoundFont 2 specification. In the long run, I hope to be able to implement a custom renderer that doesn't require DLS to SF conversion.

The `dmrender` utility is able to render an arbitrary amount of audio data from a segment. It requires that all the DLS instruments are converted into SoundFont2 files via the aforementioned `dls2sf` utility. It is still extremely experimental and is being heavily worked on.

Compiling
---------

````sh
$ git clone --recursive https://github.com/frabert/libdmusic
$ cd libdmusic
$ cmake .
$ make
````

Usage
-----

Currently, in order to render segments you need to convert DLS files to SF2 manually.
Unix shell scripts are shipped to make it easier to convert the Gothic2 files:

````sh
$ ./convert_dls_newworld.sh /path/to/G2 Orchestra.sf2
$ ./dmplay /path/to/G2/_work/Data/Music/newworld/LOB_DayStd.sgt -f Orchestra.sf2
````

If you want to playback G1 music files, you need to invoke `dls2sf` manually:
````sh
$ ./dls2sf /path/to/G1/music/files/Orchestra.dls Orchestra.sf2
$ ./dmplay /path/to/G1/music/files/OC_Day_Std.sgt -f Orchestra.sf2
````

Acknowledgements
----------------

`dmrender` and `dmplay` use [TinySoundFont](https://github.com/schellingb/TinySoundFont) for SoundFont rendering and [args](https://github.com/Taywee/args).

`dls2sf` and `dmrender` use [libsndfile](http://www.mega-nerd.com/libsndfile/) for wave output and conversion.

`dls2sf` uses [sf2cute](https://github.com/gocha/sf2cute) for SoundFont output.

`dmplay` uses [portaudio](http://www.portaudio.com/) for realtime audio playback.