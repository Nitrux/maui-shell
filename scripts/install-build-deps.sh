#!/usr/bin/env bash
set -euo pipefail

if [ "$EUID" -ne 0 ]; then
    APT_COMMAND="sudo apt-get"
else
    APT_COMMAND="apt-get"
fi

$APT_COMMAND update -q
$APT_COMMAND install -qy --no-install-recommends \
    appstream \
    build-essential \
    cmake \
    curl \
    extra-cmake-modules \
    fontconfig \
    gettext \
    git \
    gnupg2 \
    gtk-update-icon-cache \
    libcanberra-dev \
    libfontconfig-dev \
    libpolkit-agent-1-dev \
    libpolkit-qt6-1-dev \
    libphonon4qt6-dev \
    libpulse-dev \
    libwayland-dev \
    libxcb-icccm4-dev \
    libxcb-shape0-dev \
    libxkbcommon-dev \
    pkg-config \
    qt6-5compat-dev \
    qt6-base-dev \
    qt6-base-private-dev \
    qt6-declarative-dev \
    qt6-declarative-private-dev \
    qt6-multimedia-dev \
    qt6-positioning-dev \
    qt6-shadertools-dev \
    libqt6svg6-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    qt6-wayland-dev \
    qt6-wayland-dev-tools \
    qt6-wayland-private-dev \
    libkf6archive-dev \
    libkf6bookmarks-dev \
    libkf6codecs-dev \
    libkf6completion-dev \
    libkf6config-dev \
    libkf6configwidgets-dev \
    libkf6coreaddons-dev \
    libkf6crash-dev \
    libkf6dbusaddons-dev \
    libkf6declarative-dev \
    libkf6globalaccel-dev \
    libkf6i18n-dev \
    libkf6iconthemes-dev \
    libkf6idletime-dev \
    libkf6itemmodels-dev \
    libkf6itemviews-dev \
    libkf6jobwidgets-dev \
    libkf6kcmutils-dev \
    libkf6kio-dev \
    libkf6notifications-dev \
    libkf6package-dev \
    libkf6parts-dev \
    libkf6people-dev \
    libkf6prison-dev \
    libkf6service-dev \
    libkf6solid-dev \
    libkf6sonnet-dev \
    libkf6su-dev \
    libkf6syntaxhighlighting-dev \
    libkf6textwidgets-dev \
    libkf6unitconversion-dev \
    libkf6widgetsaddons-dev \
    libkf6windowsystem-dev \
    libkf6xmlgui-dev \
    libkf6kcalendarcore-dev \
    libkf6kcontacts-dev
