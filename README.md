# Maui Shell 
![](https://mauikit.org/wp-content/uploads/2018/12/maui_project_logo.png)

[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0) [![Awesome](https://awesome.re/badge.svg)](https://awesome.re) [![Generic badge](https://img.shields.io/badge/OS-Linux-blue.svg)](https://shields.io/)

_Maui Shell is a convergent shell for desktops, tablets, and phones._

Maui Shell is composed of two parts:

- Cask is the shell container and elements templates, such as panels, popups, cards etc.
- Zpace is the composer, which is the layout and places the windows or surfaces into the Cask container.

# Screenshots

![a screenshot](screenshots/desktop/screenshot.png "screenshot")

# Build

### Requirements

#### Debian/Ubuntu
##### Maui Shell needs ECM > 5.70
##### Maui Shell needs MauiKit > 2.1.0

```
libkf5config-dev
libkf5coreaddons-dev
libkf5i18n-dev
libkf5kio-dev
libkf5notifications-dev
libkf5service-dev
libqt5svg5-dev
libqt5waylandcompositor5-dev
mauikit
qtbase5-dev
qtdeclarative5-dev
qtquickcontrols2-5-dev
```

### Compile source
 1. `git clone --depth 1 --branch master https://github.com/Nitrux/maui-shell.git` 
 2. `mkdir -p maui-shell/build && cd maui-shell/build`
 4. `cmake-DCMAKE_INSTALL_PREFIX=/usr -DENABLE_BSYMBOLICFUNCTIONS=OFF -DQUICK_COMPILER=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_SYSCONFDIR=/etc -DCMAKE_INSTALL_LOCALSTATEDIR=/var -DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON -DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON -DCMAKE_INSTALL_RUNSTATEDIR=/run "-GUnix Makefiles" -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_INSTALL_LIBDIR=lib/x86_64-linux-gnu ..`
 5. `make`

 ### Install
 1. `make install`

# Running

It can run as a window inside an X11 session.

If you want to run it on the Linux console without an X11 session
(which is much more efficient), you can use a startup script like this:

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

# Issues
If you find problems with the contents of this repository please create an issue.

©2021 Nitrux Latinoamericana S.C.
