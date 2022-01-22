# Maui Shell 
![](https://mauikit.org/wp-content/uploads/2018/12/maui_project_logo.png)

[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0) [![Awesome](https://awesome.re/badge.svg)](https://awesome.re) [![Generic badge](https://img.shields.io/badge/OS-Linux-blue.svg)](https://shields.io/)

_Maui Shell is a convergent shell for desktops, tablets, and phones._

Maui Shell is composed of two parts:

- Cask is the shell container and elements templates, such as panels, popups, cards etc.
- Zpace is the composer, which is the layout and places the windows or surfaces into the Cask container.

# Screenshots

![screenshot_1](https://nxos.org/wp-content/uploads/2021/12/promo-0-scaled.jpg "Desktop Mode")
![screenshot_2](https://nxos.org/wp-content/uploads/2021/12/promo-8-scaled.jpg "Tablet Mode")
![screenshot_3](https://nxos.org/wp-content/uploads/2021/12/promo-6.jpg "Phone Mode")

# Build

Please check the [Wiki](https://github.com/Nitrux/maui-shell/wiki) for build instructions.

# Running

Maui Shell can run as a window inside an X11 session, open a terminal and run `cask`.

If you want to run it on the Linux console without an X11 session, you can use a startup script like this:

```
#!/bin/sh
export QT_QPA_PLATFORM=eglfs

# don't enlarge stuff on "high-res" displays
export QT_AUTO_SCREEN_SCALE_FACTOR=0 

# in case it's not detected
# or you wish to override
#
# export QT_QPA_EGLFS_PHYSICAL_WIDTH=480 
# export QT_QPA_EGLFS_PHYSICAL_HEIGHT=270 

# try to restart if it crashes; write a log file
cask -r -l /tmp/cask.log
```

If you are on the console and have a problem with the keyboard, mouse, etc.
don't work (which should be fixed in Qt 5.6 and above, theoretically) you can
try various input plugins (after rebooting via ssh, or the power button ;-) by adding

```
-plugin EvdevTouch -plugin EvdevMouse -plugin EvdevTablet -plugin EvdevKeyboard
```
or
```
-plugin libinput
```

Cask can run Weston and GTK3 apps too eventually;
that's mainly a matter of QtWayland having the XDG shell support finished.

Cask does not include an embedded X server yet, but it might be possible.

# Contributing
If you'd like to contribute to this project, please make a PR. You can also ask for help at the [KDE Maui project Telegram](https://t.me/mauiproject) channel.

# Issues
If you find problems with the contents of this repository please create an issue.

©2022 Nitrux Latinoamericana S.C.
