dmrender
========

Summary
-------
`dmrender` renders a DirectMusic Segment file (`.sgt`) into a Microsoft WAVE file.

Usage
-----
````
dmrender <inputfile> <outputfile>
````
The program tries to resolve all external file references in the current working
directory, so if the segment file needs - for example - style files, they have to
be in the working directory, generally the one where the binary resides.
