FROM debian:bullseye

RUN apt-get update && apt-mark hold iptables && \
    env DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      dbus-x11 \
      psmisc \
      xdg-utils \
      xorg \
      xorg-dev \
      x11-xserver-utils \
      x11-utils \
      gtk+3.0 \
      libgtk-3-dev \
      libwnck-dev \
      libwnck-3-0 \
      libwnck-common \
      libwnck-3-dev \
      libwnck-3-common \
      wmctrl

# I HATE dependencies
RUN apt install build-essential intltool dbus dbus-system-bus-common \
    libglib2.0-dev libgtk2.0-dev libxfce4util-dev \
    at-spi2-core libxi-dev xfce4-settings libxfce4panel-2.0-dev \
    libxi6 libxi-dev libxinerama1 libxinerama-dev libgtk-3-bin -y

# install xfce from ./xfce-source
# from https://docs.xfce.org/xfce/building, ${PREFIX}=$HOME/local
COPY ./xfce-source /usr/xfce-source
COPY ./run-config.sh /usr/run-config.sh

ENV PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"
ENV export CFLAGS="-02 -pipe"

# RUN apt install sudo -y
# RUN adduser --disabled-password --gecos '' docker
# RUN adduser docker sudo
# RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# USER docker
RUN /usr/run-config.sh

RUN env DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      libpulse0 \
      mousepad \
      xfce4-notifyd \
      xfce4-taskmanager \
      xfce4-terminal && \
    env DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      xfce4-battery-plugin \
      xfce4-clipman-plugin \
      xfce4-cpufreq-plugin \
      xfce4-cpugraph-plugin \
      xfce4-diskperf-plugin \
      xfce4-datetime-plugin \
      xfce4-fsguard-plugin \
      xfce4-genmon-plugin \
      xfce4-indicator-plugin \
      xfce4-netload-plugin \
      xfce4-places-plugin \
      xfce4-sensors-plugin \
      xfce4-smartbookmark-plugin \
      xfce4-systemload-plugin \
      xfce4-timer-plugin \
      xfce4-verve-plugin \
      xfce4-weather-plugin \
      xfce4-whiskermenu-plugin && \
    env DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      libxv1 \
      mesa-utils \
      mesa-utils-extra && \
    touch /etc/xdg/xfce4/xfconf/xfce-perchannel-xml/xsettings.xml && \
    sed -i 's%<property name="ThemeName" type="string" value="Xfce"/>%<property name="ThemeName" type="string" value="Raleigh"/>%' /etc/xdg/xfce4/xfconf/xfce-perchannel-xml/xsettings.xml


# disable xfwm4 compositing if X extension COMPOSITE is missing and no config file exists
RUN Configfile="~/.config/xfce4/xfconf/xfce-perchannel-xml/xfwm4.xml" && \
echo "#! /bin/bash\n\
xdpyinfo | grep -q -i COMPOSITE || {\n\
  echo 'x11docker/xfce: X extension COMPOSITE is missing.\n\
Window manager compositing will not work.\n\
If you run x11docker with option --nxagent,\n\
you might want to add option --composite.' >&2\n\
  [ -e $Configfile ] || {\n\
    mkdir -p $(dirname $Configfile)\n\
    echo '<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<channel name=\"xfwm4\" version=\"1.0\">\n\
\n\
  <property name=\"general\" type=\"empty\">\n\
    <property name=\"use_compositing\" type=\"bool\" value=\"false\"/>\n\
  </property>\n\
</channel>\n\
' > $Configfile\n\
  }\n\
}\n\
startxfce4\n\
" > /usr/local/bin/start && \
chmod +x /usr/local/bin/start

CMD start
