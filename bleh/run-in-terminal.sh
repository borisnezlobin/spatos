#!/bin/bash

GEOMETRY="80x24"
USER=$(whoami)
GUEST_DISPLAY=":15"
WINNAME=$(uuidgen || echo $RANDOM)
EXTRA=( --disable-server --title "$WINNAME" )
OUTFILE="out.mov"

shopt -s extglob
declare -A OPTS
OPTS[HOME]=""

while [ ! -z "$1" ]
do
    case "$1" in
        -?(-)help)
                usage
                exit 0
                ;;

        +([0-9])x+([0-9])*)
                GEOMETRY="$1"
                shift
                ;;

        --fullscreen)
                EXTRA+=( --fullscreen )
                shift
                ;;

        :+([0-9]))
                GUEST_DISPLAY="$1"
                shift
                ;;

        *.mov)  OUTFILE="$1"
                shift
                ;;

        *)      if id -u "$1" > /dev/null 2>&1 ; then
                    # valid user name on that system
                    USER="$1"
                    shift
                elif [ -d "$1" ] ; then
                    # valid directory
                    OPTS[HOME]="$1"
                    shift
                else
                    echo Not a known user or directory : "$1" >&2
                    exit 1
                fi
                ;;
    esac
done

TEMPDIR=$(mktemp -d)
COMP="${TEMPDIR}/comp"
CTRL="${TEMPDIR}/ctrl"
FIFO="${TEMPDIR}/fifo"
mkfifo "${COMP}"
mkfifo "${CTRL}"
mkfifo "${FIFO}"
chmod a+r "${COMP}"
chmod a+r "${FIFO}"
chmod a+x "${TEMPDIR}"
#exec 3<>"${FIFO}"

typeit() {
  while IFS='' read -rn1 C; do
    if [ -z "${C}" ] ; then
        echo
    else
        echo -n "$C"
    fi
    sleep .$((1+$RANDOM/15000))
    [ -z "${C}" ] && sleep 1.5 # extra delay after end-of-line
  done
}


sudo -u "${USER}" -b -i xfce4-terminal --geometry="${GEOMETRY}" \
                                       --display="${GUEST_DISPLAY}" \
                                       --hide-menubar \
                                       --hide-toolbar \
                                       --hide-scrollbar \
                                       "${EXTRA[@]}" -x bash -c \
                                       'cat < '"${FIFO}"' ; read < '"${COMP}"

sleep 1
# At this point, the terminal window is blocked.
# Start capture
./capture.sh --nomouse "${GUEST_DISPLAY}" \
    $(./window-geometry.sh "${GUEST_DISPLAY}" "${WINNAME}") \
    "${OUTFILE}" < "${CTRL}" &

CAPTUREPID="$!"
echo "${CAPTUREPID}"
echo  > "${CTRL}"

# For obscure reasons, the following line will put the process to
# the background when reading from stdin ?!?
# Maybe `stty -tostop` would FIX that ?
# See http://www.linusakesson.net/programming/tty/

# This require a PTS, not a Pipe so 'bash -i' will be totally
# "fooled" in thinking it is connected to a terminal
exec 3<>/dev/ptmx
sudo -u "${USER}" echo -n # <- hack to force authentication here
PTS=$(./pts <&3)
echo $?
# sudo -u "${USER}" ./grantpt <&3
# echo $?
# ./unlockpt <&3
# echo $?
# PTS=$(./ptsname <&3)
# echo $?

echo PTS is ${PTS}

# stty -F "${PTS}" cooked echo iutf8 rows 22 columns 80
# stty -F "${PTS}" --all

(
    sleep 3
    typeit
    sleep 1

    # Take a screenshot after completion
    xwd -silent -display "${GUEST_DISPLAY}" -name "${WINNAME}" | \
        convert xwd:- "${OUTFILE%.*}.png"

    echo exit
) >&3 < /dev/stdin &

sudo -nSu "${USER}" \
bash -c "
    cd \"${OPTS[HOME]:-\$HOME}\"
    bash -i
" <"${PTS}" 2>&1 | tee "${OUTFILE%.*}.log" >"${FIFO}" 


exec 3>&-

sleep 1
echo 'q' > "${CTRL}"
wait

# Exit terminal
echo 'done' > "${COMP}"

#kill -SIGTERM "${CAPTUREPID}"
rm -rf "${TEMPDIR}"
