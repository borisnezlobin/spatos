#!/bin/bash

BUILD_ORDER=("xfce4-dev-tools-4.18.0" "libxfce4util-4.18.0" "xfconf-4.18.0" "libxfce4ui-4.18.0" "garcon-4.18.0" "exo-4.18.0" "xfce4-panel-4.18.0" "thunar-4.18.0" "xfce4-settings-4.18.0" "xfce4-session-4.18.0" "xfwm4-4.18.0" "xfdesktop-4.18.0" "xfce4-appfinder-4.18.0" "tumbler-4.18.0")

for pack in "${BUILD_ORDER[@]}"; do
    cd "/usr/xfce-source/$pack" && ./configure --prefix=/usr/local && make && make install
done

