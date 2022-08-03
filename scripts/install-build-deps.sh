#!/bin/bash

if [ "$EUID" -ne 0 ]; then
    APT_COMMAND="sudo apt-get"
else
    APT_COMMAND="apt-get"
fi

$APT_COMMAND update -q
$APT_COMMAND install -qy --no-install-recommends \
    appstream \
    automake \
    autotools-dev \
    build-essential \
    checkinstall \
    cmake \
    curl \
    devscripts \
    equivs \
    extra-cmake-modules \
    gettext \
    git \
    gnupg2 \
    gtk-update-icon-cache \
    kded5-dev \
    kinit-dev \
    libfontconfig-dev \
    libfontconfig1-dev \
    libkf5activities-dev \
    libkf5activitiesstats-dev \
    libkf5akonadi-dev \
    libkf5akonadicontact-dev \
    libkf5archive-dev \
    libkf5auth-dev \
    libkf5bookmarks-dev \
    libkf5codecs-dev \
    libkf5completion-dev \
    libkf5config-dev \
    libkf5configwidgets-dev \
    libkf5coreaddons-dev \
    libkf5crash-dev \
    libkf5declarative-dev \
    libkf5doctools-dev \
    libkf5eventviews-dev \
    libkf5globalaccel-dev \
    libkf5i18n-dev \
    libkf5iconthemes-dev \
    libkf5idletime-dev \
    libkf5itemmodels-dev \
    libkf5itemviews-dev \
    libkf5jobwidgets-dev \
    libkf5kio-dev \
    libkf5notifications-dev \
    libkf5notifyconfig-dev \
    libkf5package-dev \
    libkf5parts-dev \
    libkf5people-dev \
    libkf5prison-dev \
    libkf5pty-dev \
    libkf5runner-dev \
    libkf5service-dev \
    libkf5solid-dev \
    libkf5sonnet-dev \
    libkf5su-dev \
    libkf5syntaxhighlighting-dev \
    libkf5texteditor-dev \
    libkf5textwidgets-dev \
    libkf5unitconversion-dev \
    libkf5wallet-dev \
    libkf5wayland-dev \
    libkf5widgetsaddons-dev \
    libkf5windowsystem-dev \
    libkf5xmlgui-dev \
    libphonon4qt5-dev \
    libpolkit-agent-1-dev \
    libpolkit-qt5-1-dev \
    libqt5svg5-dev \
    libqt5waylandcompositor5-dev \
    libqt5x11extras5-dev \
    libwayland-dev \
    libxcb-icccm4-dev \
    libxcb-shape0-dev \
    lintian \
    qtbase5-dev \
    qtdeclarative5-dev \
    qtmultimedia5-dev \
    qtpositioning5-dev \
    qtquickcontrols2-5-dev \
    qtwayland5 \
    qtwayland5-dev-tools \
    qtwayland5-private-dev \
    qtwayland5-private-devppstream
