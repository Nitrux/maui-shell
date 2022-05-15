# Maui Shell 
![](https://mauikit.org/wp-content/uploads/2018/12/maui_project_logo.png)

[![Maui Shell AppImage](https://github.com/Nitrux/maui-shell/actions/workflows/build-appimage-amd64.yml/badge.svg)](https://github.com/Nitrux/maui-shell/actions/workflows/build-appimage-amd64.yml) [![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0) [![Awesome](https://awesome.re/badge.svg)](https://awesome.re) [![Generic badge](https://img.shields.io/badge/OS-Linux-blue.svg)](https://shields.io/)

_Maui Shell is a convergent shell for desktops, tablets, and phones._

Maui Shell is composed of two parts:

- Cask is the shell container and elements templates, such as panels, popups, cards etc.
- Zpace is the composer, which is the layout and places the windows or surfaces into the Cask container.

# Screenshots

![screenshot_1](https://nxos.org/wp-content/uploads/2021/12/promo-0-scaled.jpg "Desktop Mode")
![screenshot_2](https://nxos.org/wp-content/uploads/2021/12/promo-8-scaled.jpg "Tablet Mode")
![screenshot_3](https://nxos.org/wp-content/uploads/2021/12/promo-6.jpg "Phone Mode")

# Download

 Maui Shell is available as an AppImage. Click the button below to download it or head over to the _Releases_ tab.

 [![appimage-button](https://github.com/UriHerrera/appimage-buttons/blob/main/buttons/appimage-btn-dark.svg)](https://github.com/Nitrux/maui-shell/releases)

# Build

Please check the [Wiki](https://github.com/Nitrux/maui-shell/wiki) for build instructions.

# Running

Maui Shell can run as a window inside an X11 session, open a terminal and run `cask`. There is a Wayland session  too if you’d like to test the Maui Shell using a Wayland sesson.

To run un Cask in window mode, you can launch ‘cask -w,’ which will allow you to test different sizes to emulate tablet or phone form factors.

Using a different TTY is also possible to test Maui Shell; in this case, you would need to use the program ‘startcask-wayland’.

If you try it within another session, you can launch `cask -l /tmp/cask.log` to generate a log file; this is useful if you want to report an issue later.

To ensure the input works correctly, you need to add your user to the `input` group.

Eventually, Cask can run Weston and GTK3 apps.

Cask does not include an embedded X server yet, but it might be possible.

# Contributing
If you'd like to contribute to this project, please make a PR. You can also ask for help at the [KDE Maui project Telegram](https://t.me/mauiproject) channel.

# Issues
If you find problems with the contents of this repository please create an issue.

©2022 Nitrux Latinoamericana S.C.
