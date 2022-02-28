[Desktop Entry]
Name=Cask (Wayland)
Comment=Maui Shell is a convergent shell for desktops, tablets, and phones.
Exec=@CMAKE_INSTALL_FULL_LIBEXECDIR@/cask-dbus-run-session-if-needed ${CMAKE_INSTALL_FULL_BINDIR}/startcask-wayland
TryExec=${CMAKE_INSTALL_FULL_BINDIR}/startcask-wayland
Type=Application
