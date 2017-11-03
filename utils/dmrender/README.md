dmrender
========

Summary
-------
      dmrender {OPTIONS} [segment] [output]
    
        dmrender renders DirectMusic segments into audio files
    
      OPTIONS:
    
          -h, --help                        Display this help menu
          -l[length], --length=[length]     The length in seconds of the audio to
                                            render
          -s[sampling rate],
          --sample=[sampling rate]          The sampling rate to use
          -c[channels],
          --channels=[channels]             The number of channels to use
          -f[soundfont],
          --soundfont=[soundfont]           The SoundFont file to use during
                                            rendering
          -O, --ogg                         The output file is going to be an
                                            Ogg/Vorbis file instead of an
                                            uncompressed Microsoft WAVE file
          segment                           The segment to render
          output                            The output file
          "--" can be used to terminate flag options and force all following
          arguments to be treated as positional options


Remarks
-----
The program tries to resolve all external file references in the current working
directory, so if the segment file needs - for example - style files, they have to
be in the working directory, generally the one where the binary resides.

Also the directory must contain all the instruments as SF2 files, which can
be obtained by converting the DLS files using the `dls2sf` utility; more than one
DLS can be converted and merged into a single SoundFont, which can be specified using
the `-f` flag.

The default settings are: 60 seconds of mono audio, encoded at 44.1kHz.