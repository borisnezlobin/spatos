#!/bin/bash

function usage() {
    nroff -mandoc <<-EOF | less
.TH $(basename $0) 1
$(basename $0) - open a nested X terminal
.SH SYNOPSIS
$(basename $0) [displayid] [displaysize | --fullscreen] [userlogin] [homedir]

$(basename $0) \-\-help

.SH OPTIONS

.TP
.BR \fIdisplayid\fR
Virtual display to create
Default is :15
.TP
.BR \fIdisplaysize\fR
Size of the virtual display
Default is 1280x720
.TP
.BR \fIuserlogin\fR
Open a session with the given identity.
Default is the current user. Opening a session as a different user require sudo access.
.TP
.BR \fIhomedir\fR
Set the \fIHOME\fR directory for the nested session.
Useful to run xfce4 in a clean and distinct environment 
from yours.  The nested session owner should have 
read/write access to the directory.

.SH EXAMPLE
  ./nested-xfce4-session.sh 
      Start a new X11 server and run xfce4-session

  ./nested-xfce4-session.sh :12
      Start a new X11 server as DISPLAY :12 and run xfce4-session

  ./nested-xfce4-session.sh :12 640x480
      Start a new X11 server as DISPLAY :12 
      with a screen size of 640x480 and run xfce4-session
      (eventually screen size will be resized according to
      your host xfce4 configuration)

  ./nested-xfce4-session.sh /tmp/temp.dir
      Start a new X11 server and run xfce4-session with the
      home directory set to /tmp/temp.dir.
EOF
}

SCREEN="1280x720"
USER=$(whoami)
GUEST_DISPLAY=":15"
EXTRA=( -dpi 96 )
REMOTE=

shopt -s extglob
declare -A OPTS

while [ ! -z "$1" ]
do
    case "$1" in
        -?(-)help)
                usage
                exit 0
                ;;

        +([0-9])x+([0-9]))
                SCREEN="$1"
                shift
                ;;

        --fullscreen)
                EXTRA+=( -fullscreen )
                shift
                ;;

        :+([0-9]))
                GUEST_DISPLAY="$1"
                shift
                ;;
        
	*@*)	USER="$1"
		REMOTE=true
		shift
		;;

        *)      if id -u "$1" > /dev/null 2>&1 ; then
                    # valid user name on that system
                    USER="$1"
                    shift
                elif [ -d "$1" ] ; then
                    # valid directory
                    OPTS[HOME]="HOME=\"$1\""
                    shift
                else
                    echo Not a known user or directory : "$1" >&2
                    exit 1
                fi
                ;;
    esac
done

# Prepare the client to run when server will be ready
LAUNCHER="/bin/bash"

#if [ "${USER}" != $(whoami) ]
#    LAUNCHER="sudo -u ${USER} -b -i"
#then
#else
#    LAUNCHER="env -i PATH=${PATH} 
#                     DISPLAY=${GUEST_DISPLAY} 
#                     HOME=${HOME} 
#                     LOGNAME=${LOGNAME}
#                     USER=${USER} bash -c"
#fi

TEMPDIR=$(mktemp -d)
FIFO="${TEMPDIR}/fifo"
mkfifo "${FIFO}"
chmod a+r "${FIFO}"
chmod a+x "${TEMPDIR}"
exec 3<>"${FIFO}"

APP=xfce4-session
#APP=xterm
if [ -n "$REMOTE" ]
then
    DISPLAY=${GUEST_DISPLAY} ssh -fCY ${USER} "sleep 5 && echo \$DISPLAY && unset XAUTHORITY && "${OPTS[@]}" ${APP}"
else
    LAUNCHER="sudo -u ${USER} -b -i bash -c"
    DISPLAY=${GUEST_DISPLAY} ${LAUNCHER} "read DISPLAY < '${FIFO}' && \
                    unset XAUTHORITY && \
                    "${OPTS[@]}" DISPLAY=\":\${DISPLAY}\" $APP"
fi

Xephyr -displayfd 3 \
       -sw-cursor -reset -terminate \
       "${GUEST_DISPLAY}" -screen "${SCREEN}" "${EXTRA[@]}"

rm -rf "${TEMPDIR}"

wait
