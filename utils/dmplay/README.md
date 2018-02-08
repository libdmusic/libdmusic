dmplay
======

Summary
-------

    dmplay {OPTIONS} [segment]

      dmplay plays DirectMusic segments in real time

    OPTIONS:

        -h, --help                        Display this help menu
        -s[sampling rate],
        --sample=[sampling rate]          The sampling rate to use
        -c[channels],
        --channels=[channels]             The number of channels to use
        segment                           The segment to render
        "--" can be used to terminate flag options and force all following
        arguments to be treated as positional options

Remarks
-------

The program tries to resolve all external file references in the current working
directory, so if the segment file needs - for example - style files, they have to
be in the working directory, generally the one where the binary resides.

The default settings are: stereo audio at 44.1kHz.