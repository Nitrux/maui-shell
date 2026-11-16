#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/_common.sh"
ensure_checkout mauiman https://invent.kde.org/maui/mauiman.git "${BRANCH_MAUIMAN:-master}"
apply_patch mauiman "$REPO_ROOT/scripts/patches/mauiman-service-name.patch"
configure_build_install mauiman "$WORKDIR/mauiman" -DBUNDLE_MAUI_STYLE=ON

# Remove the old misnamed service file from previous installs.
$SUDO rm -f "$PREFIX"/share/dbus-1/services/org.mauiman.Manager[0-9]*.service
