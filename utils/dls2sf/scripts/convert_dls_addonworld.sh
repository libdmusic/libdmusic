#!/bin/sh
if [ "$#" -ne 2 ] || ! [ -d "$1" ];
then
  echo "Usage: $0 [G2 directory] [Output file]" >&2
  exit 1
fi

MUSICPATH="$1/_work/Data/Music/AddonWorld"

./dls2sf "$MUSICPATH/__DLS_Accordion.dls" \
  "$MUSICPATH/__DLS_Bass.dls" \
  "$MUSICPATH/__DLS_Celli.dls" \
  "$MUSICPATH/__DLS_Fiddle.dls" \
  "$MUSICPATH/__DLS_Guitar.dls" \
  "$MUSICPATH/__DLS_Harp.dls" \
  "$MUSICPATH/__DLS_Horn.dls" \
  "$MUSICPATH/__DLS_MonoPerc.dls" \
  "$MUSICPATH/__DLS_RareIngame.dls" \
  "$MUSICPATH/__DLS_Violins.dls" -o "$2"