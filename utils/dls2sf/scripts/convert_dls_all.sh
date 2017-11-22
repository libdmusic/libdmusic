#!/usr/bin/env bash

if [ "$#" -ne 1 ];
then
    echo "Usage: $0 [G2 directory]" >&2
    exit 1
fi

MUSICPATH="$1/_work/Data/Music"
DLS2SF="./dls2sf"

if [ ! -d "$1" ]; then
    >&2 echo "error: specified gothic directory does not exist: $1"
    exit 1
fi

if [ ! -d "$MUSICPATH" ]; then
    >&2 echo "error: the Music directory does not exist under: $MUSICPATH"
    exit 1
fi

if [ ! -f "$DLS2SF" ]; then
    >&2 echo "error: the dls2sf converter executable was not found: $DLS2SF"
    exit 1
fi

for worldDirectory in $MUSICPATH/*/; do
    worldName=$(basename $worldDirectory)
    echo "Converting *.dls files in directory $worldName"
    mkdir -p "$worldName"
    "$DLS2SF" "$worldDirectory"/*.dls -o "$worldName/Orchestra.sf2"
done
