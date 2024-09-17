#!/bin/bash

DISPLAY=:15
X=0
Y=0
X11GRAB_OPTS=( -show_region 1 )

shopt -s extglob

while [ ! -z "$1" ]
do
    case "$1" in
        :*)     DISPLAY="$1"
                shift
                ;;

        +([0-9])x+([0-9]))
                read W H < <(tr -c "[0-9]" " " <<< "$1")
                shift
                ;;

        +([0-9])x+([0-9])++([0-9])++([0-9]))
                read W H X Y < <(tr -c "[0-9]" " " <<< "$1")
                shift
                ;;

        --nomouse)
                X11GRAB_OPTS+=( -draw_mouse 0 )
                shift
                ;;

        *)      OUTFILE="$1"
                shift
                ;;
    esac
done

[ -f "${OUTFILE}" ] && {
    echo "${OUTFILE} already exists. Aborting." >&2
    exit 1
}

if which play > /dev/null 2>&1 ; then
    (
        sleep 1.0
        play  -n -c1 synth .2 sin 440 gain -12

        if which espeak > /dev/null 2>&1 ; then
            sleep 0.25
            echo "$(basename "${OUTFILE%.*}")" | espeak
        fi
    
    ) &
fi


while IFS=": " read key value ; do
    echo "key=|$key| value=|$value|"
    case "$key" in
        Width) DWIDTH=$value
                echo W
                ;;
        Height) DHEIGHT=$value
                ;;
    esac
done < <(xwininfo -display "${DISPLAY}" -root)

[ -z "${W}" ] && W="${DWIDTH}"
[ -z "${H}" ] && H="${DHEIGHT}"

ffmpeg -y \
  -f x11grab -thread_queue_size 512 -vsync cfr -framerate 25 \
  -video_size "${W}x${H}" \
  "${X11GRAB_OPTS[@]}" \
  -i "${DISPLAY}.0+$X,$Y" \
  -f alsa -thread_queue_size 2048 -acodec pcm_s24le -sample_rate 44100 -ac 1 -i pulse \
  -c:a pcm_s16le \
  -c:v libx264 -pix_fmt yuv420p -preset ultrafast -tune animation -qp 1 \
  -map 0:0,0:0 -map 1:0,0:0 \
  -timecode 00:00:00:00 \
  "$OUTFILE"

