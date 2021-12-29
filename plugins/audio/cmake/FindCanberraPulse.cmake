# - Find libcanberra's pulseaudio backend.
# This module defines the following variables:
#
#  CanberraPulse_FOUND - true if the backend was found
#
# SPDX-FileCopyrightText: 2019 Harald Sitter <sitter@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

find_package(Canberra)

find_library(CanberraPulse_LIBRARY canberra-pulse
    PATH_SUFFIXES libcanberra libcanberra-${Canberra_VERSION}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CanberraPulse
    FOUND_VAR CanberraPulse_FOUND
    REQUIRED_VARS CanberraPulse_LIBRARY
)
mark_as_advanced(CanberraPulse_LIBRARY)

# NB: CanberraPulse_LIBRARY is intentionally not documented as it serves no
#   public purpose (it's a plugin, not a library).
