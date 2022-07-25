#!/bin/bash

if [ "$EUID" -ne 0 ]; then
    APT_COMMAND="sudo apt-get"
else
    APT_COMMAND="apt-get"
fi

$APT_COMMAND update -q
$APT_COMMAND install -qy --no-install-recommends \
    cmake \
    git \
    build-essential \
    extra-cmake-modules \
    gtk-update-icon-cache \
    qtbase5-dev \
    qtdeclarative5-dev \
    qtmultimedia5-dev \
    qtquickcontrols2-5-dev \
    libkf5coreaddons-dev \
    libqt5svg5-dev \
    libkf5config-dev \
    libkf5i18n-dev \
    libkf5bookmarks-dev \
    libkf5widgetsaddons-dev \
    libkf5completion-dev \
    libkf5itemviews-dev \
    libkf5jobwidgets-dev \
    libkf5solid-dev \
    libkf5xmlgui-dev \
    libkf5configwidgets-dev \
    libkf5auth-dev \
    libkf5codecs-dev \
    libkf5windowsystem-dev \
    libkf5parts-dev \
    libkf5pty-dev \
    libkf5textwidgets-dev \
    libkf5sonnet-dev \
    libkf5service-dev \
    gettext \
    libkf5notifications-dev \
    qtpositioning5-dev \
    libqt5x11extras5-dev \
    libxcb-shape0-dev \
    libxcb-icccm4-dev \
    libkf5doctools-dev \
    libkf5runner-dev \
    libkf5notifyconfig-dev \
    libkf5su-dev \
    libkf5wallet-dev \
    libkf5idletime-dev \
    libkf5declarative-dev \
    libkf5crash-dev \
    libkf5globalaccel-dev \
    libkf5wayland-dev \
    libkf5people-dev \
    libkf5activitiesstats-dev \
    libkf5kio-dev \
    libkf5prison-dev \
    libkf5package-dev \
    libkf5archive-dev \
    libkf5iconthemes-dev \
    libkf5unitconversion-dev \
    libkf5itemmodels-dev \
    libkf5texteditor-dev \
    libkf5syntaxhighlighting-dev \
    libkf5activities-dev \
    kinit-dev \
    libqt5waylandcompositor5-dev \
    kded5-dev \
    libpolkit-agent-1-dev \
    libpolkit-qt5-1-dev \
    libfontconfig1-dev \
    libphonon4qt5-dev \
    libkf5akonadi-dev \
    libkf5akonadicontact-dev \
    libkf5akonadicontact-dev \
    libkf5eventviews-dev