Maui Shell is a convergent shell for desktop, tablets and phones.

Cask is the shell container and elements templates, such as panels, popups, cards etc.

Zpace is the composer, which is the layout and places the windows /surfaces into the Cask container.


![a screenshot](screenshots/desktop/screenshot.png "screenshot")

# Requirements

* Qt 5.15 (if you need 5.x, use the 5.x branch)
  - qtbase, qtdeclarative, qtwayland, dbus, concurrent
  - QtQuick.Controls 2
  - QtQuick.Controls 1, only for the calendar popover
* KConfig
* KService
* KI18n
* KCoreAddons
* BlueDevil
* Plasma Core
* Plasma-nm
* KIO
* MauiKit

# Building

```
cmake .. _DCMAKE_INSTALL_PREFIX=/usr
make
```

# Installation

```
sudo make install
```

# Running

It can run as a window inside an X11 session.

If you want to run it on the Linux console without an X11 session
(which is much more efficient), you can use a startup script like this:

```
#!/bin/sh
export QT_QPA_PLATFORM=eglfs
export QT_AUTO_SCREEN_SCALE_FACTOR=0 # don't embiggen stuff on "high-res" displays
#export QT_QPA_EGLFS_PHYSICAL_WIDTH=480 # in case it's not detected
#export QT_QPA_EGLFS_PHYSICAL_HEIGHT=270 # or you wish to override
# try to restart if it crashes; write a log file
cask -r -l /tmp/cask.log
```

If you are on the console and have the problem that the keyboard, mouse etc.
don't work (which should be fixed in Qt 5.6 and above, theoretically) you can
try various input plugins (after rebooting via ssh, or the power button ;-) by adding
```-plugin EvdevTouch -plugin EvdevMouse -plugin EvdevTablet -plugin EvdevKeyboard```
or
```-plugin libinput```

The set of applications you can run inside is mostly limited to those
that are built with Qt 5, so far.  That includes a lot of KDE applications.
It is intended to eventually be able to run weston and GTK3 apps too;
that's mainly a matter of qtwayland having the XDG shell support finished.
Cask does not include an embedded X server yet, but it might be possible.
