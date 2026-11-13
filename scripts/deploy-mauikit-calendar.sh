#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/_common.sh"
ensure_checkout mauikit-calendar https://invent.kde.org/maui/mauikit-calendar.git "${BRANCH_MAUIKIT_CALENDAR:-master}"
apply_patch mauikit-calendar "$REPO_ROOT/scripts/patches/mauikit-calendar-generate-plugin.patch"
configure_build_install mauikit-calendar "$WORKDIR/mauikit-calendar" -DBUNDLE_MAUI_STYLE=ON
