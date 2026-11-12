#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/_common.sh"
build_component mauikit https://invent.kde.org/maui/mauikit.git "${BRANCH_MAUIKIT:-master}" -DBUNDLE_MAUI_STYLE=ON
