#!/usr/bin/env bash

if [ "$#" -ne 1 ];
then
    echo "Usage: $0 [Gothic directory]" >&2
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

mkdir -p "$1/soundfonts"
if [ ! -d "$1/soundfonts" ]; then
    >&2 echo "error: cannot create directory $1/soundfonts"
    exit 1
fi

for worldDirectory in $MUSICPATH/*/; do
    worldName=$(basename "$worldDirectory")
    echo "Converting *.sty files in directory $worldName"
    "$DLS2SF" -s "$worldDirectory/"*".sty" -o "$1/soundfonts" -i "$worldDirectory/"
done
