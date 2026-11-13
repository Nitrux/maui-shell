#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/_common.sh"
build_component mauikit-filebrowsing https://invent.kde.org/maui/mauikit-filebrowsing.git "${BRANCH_MAUIKIT_FILEBROWSING:-master}"
