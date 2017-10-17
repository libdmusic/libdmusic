dmrender
========

Summary
-------
`dmrender` renders a DirectMusic Segment file (`.sgt`) into a Microsoft WAVE file.

Usage
-----
````
dmrender <inputfile> <outputfile> [rendering length in seconds] 
````
The program tries to resolve all external file references in the current working
directory, so if the segment file needs - for example - style files, they have to
be in the working directory, generally the one where the binary resides.

Also the directory must contain all the instruments as SF2 files, which can
be obtained by converting the DLS files using the `dls2sf` utility.

If no length is specified, the utility will render 60 seconds of audio.