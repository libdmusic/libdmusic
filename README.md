<h1 align="center">
  <br>
  <img width="192" src="media/logo.svg">
  <br>
  <br>
  <br>
</h1>

Project goals
=============

`libdmusic` aims to provide a framework and suite of tools to enable applications and games playback of Microsoft DirectMusic audio files.

Roadmap
-------
- DLS to SoundFont conversion
- Realtime SoundFont playback
- Realtime DLS level 2 playback
- `.sgt` and `.sty` files loading

For now, the `dls2sf` utility does not support DLS articulators, so the envelopes will sound weird. Converting articulators is difficult because DLS level 1 (and especially level 2) specify many modulating sources and destinations which are not supported by the SoundFont 2 specification. In the long run, I hope to be able to implement a custom renderer that doesn't require DLS to SF conversion.

Compiling
---------

````
$ git clone --recursive https://github.com/frabert/libdmusic
$ cd libdmusic
$ cmake .
$ make
````
