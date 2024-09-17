#!/bin/bash

# Assume window title is $1
# FIXME enforce that !

DISPLAY=:15
WINNAME=""

shopt -s extglob

while [ ! -z "$1" ]
do
    case "$1" in
        :*)     DISPLAY="$1"
                shift
                ;;


        *)      WINNAME="$1"
                shift
                ;;
    esac
done

[ -z "${WINNAME}" ] && {
    echo "Missing window name Aborting." >&2
    exit 1
}

declare -A WINPROPS

while IFS=": " read key value ; do
    if [ -n "$key" ]; then
        # echo "key=|$key| value=|$value|" >&2
        WINPROPS["$key"]="$value"
    fi
done < <(xwininfo -display "${DISPLAY}" -name "$WINNAME")

printf "%sx%s%s\n" \
    "${WINPROPS[Width]}" \
    "${WINPROPS[Height]}" \
    "${WINPROPS[Corners]%% *}"
