# - Find sound-theme-freedesktop via XDG_DATA_DIRS
# This module defines the following variables:
#
#  SoundThemeFreeDesktop_FOUND - true if the sound theme is found
#  SoundThemeFreeDesktop_PATH - path to the index.theme file
#
# SPDX-FileCopyrightText: 2019 Harald Sitter <sitter@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

find_file(SoundThemeFreeDesktop_PATH "sounds/freedesktop/index.theme"
    PATHS ENV XDG_DATA_DIRS /usr/local/share/ /usr/share/
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SoundThemeFreeDesktop
    FOUND_VAR SoundThemeFreeDesktop_FOUND
    REQUIRED_VARS SoundThemeFreeDesktop_PATH
)
mark_as_advanced(SoundThemeFreeDesktop_PATH)