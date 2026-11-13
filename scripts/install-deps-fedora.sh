#!/usr/bin/env bash
set -euo pipefail
SUDO=""; [ "$(id -u)" -ne 0 ] && SUDO="sudo"

PKGS=(
  cmake gcc-c++ make git gettext pkgconf-pkg-config extra-cmake-modules
  qt6-qtbase-devel qt6-qtbase-private-devel qt6-qtdeclarative-devel qt6-qtsvg-devel
  qt6-qtwayland-devel qt6-qtmultimedia-devel qt6-qtpositioning-devel qt6-qt5compat-devel
  qt6-qttools-devel qt6-qtshadertools-devel
  kf6-kcoreaddons-devel kf6-ki18n-devel kf6-kconfig-devel kf6-kiconthemes-devel kf6-kio-devel
  kf6-knotifications-devel kf6-kservice-devel kf6-solid-devel kf6-kidletime-devel
  kf6-kwindowsystem-devel kf6-kcrash-devel kf6-kdbusaddons-devel kf6-kded-devel
  kf6-kpackage-devel kf6-kdeclarative-devel kf6-kglobalaccel-devel kf6-karchive-devel
  kf6-kpeople-devel kf6-kdesu-devel kf6-kitemmodels-devel kf6-kcmutils-devel
  kf6-kirigami-devel kf6-kcalendarcore-devel kf6-kcontacts-devel kf6-kfilemetadata-devel
  kf6-syntax-highlighting-devel kf6-kcodecs-devel kf6-kguiaddons-devel kf6-kwidgetsaddons-devel
  kf6-kconfigwidgets-devel kf6-kjobwidgets-devel kf6-kcolorscheme-devel kf6-kauth-devel
  kf6-kbookmarks-devel kf6-kcompletion-devel kf6-kxmlgui-devel kf6-ktextwidgets-devel
  kf6-sonnet-devel kf6-kparts-devel kf6-prison-devel kf6-kunitconversion-devel
  kf6-knotifyconfig-devel kf6-kwallet-devel kf6-networkmanager-qt-devel
  phonon-qt6-devel polkit-qt6-1-devel polkit-devel libcanberra-devel pulseaudio-libs-devel
  glib2-devel fontconfig-devel wayland-devel wayland-protocols-devel libxkbcommon-devel
  libxcb-devel xcb-util-wm-devel libX11-devel
  akonadi-server akonadi-calendar akonadi-contacts grantleetheme kcalutils
  kidentitymanagement kf6-ktexttemplate
)

$SUDO dnf install -y --setopt=strict=0 "${PKGS[@]}"

missing=()
for p in "${PKGS[@]}"; do rpm -q "$p" >/dev/null 2>&1 || missing+=("$p"); done
if [ ${#missing[@]} -gt 0 ]; then
  echo; echo "NOT installed (name differs or not in your repos): ${missing[*]}"
  echo "Only matters if a configure step later says it can't find that package."
fi
