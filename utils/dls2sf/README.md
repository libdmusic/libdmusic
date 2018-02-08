dls2sf
======

Summary
-------
      dls2sf {OPTIONS} [input files...]
    
        dls2sf converts DLS files and DirectMusic bands to SoundFont2 files
    
      OPTIONS:
    
          -h, --help                        Display this help menu
          -s, --style                       Treats the input files as styles, and
                                            converts every band found
          -o[output], --output=[output]     Output file/path
          -i[input], --input=[input]        Input path where sounds are located
          input files...                    Input files to parse
          "--" can be used to terminate flag options and force all following
          arguments to be treated as positional options