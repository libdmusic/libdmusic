#!/bin/sh
if [ "$#" -ne 2 ] || ! [ -d "$1" ];
then
  echo "Usage: $0 [G2 directory] [Output file]" >&2
  exit 1
fi

MUSICPATH="$1/_work/Data/Music/newworld"

./dls2sf "$MUSICPATH/DLS_Alpenhorn.dls" \
  "$MUSICPATH/DLS_Bass.dls" \
  "$MUSICPATH/DLS_Brass.dls" \
  "$MUSICPATH/DLS_Daduk.dls" \
  "$MUSICPATH/DLS_DragonIsland.dls" \
  "$MUSICPATH/DLS_Flute.dls" \
  "$MUSICPATH/DLS_Guitar.dls" \
  "$MUSICPATH/DLS_Harp.dls" \
  "$MUSICPATH/DLS_Metronom.dls" \
  "$MUSICPATH/DLS_Organ.dls" \
  "$MUSICPATH/DLS_Percussions.dls" \
  "$MUSICPATH/DLS_Piano.dls" \
  "$MUSICPATH/DLS_Rare.dls" \
  "$MUSICPATH/DLS_Strings.dls" -o "$2"