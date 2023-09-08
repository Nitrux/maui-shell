# SPDX-FileCopyrightText: 2021 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(AUTOMOC_MOC_OPTIONS -Muri=org.maui.cask)

include(ECMGenerateQmlTypes)
include(ECMFindQmlModule)
include(ECMQmlModule)

project(CaskLib LANGUAGES CXX VERSION 1.0.0)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/CaskLib")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CaskLibConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CaskLibConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS CMAKE_INSTALL_PREFIX)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CaskLibConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CaskLibConfig.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel)

install(EXPORT CaskLibTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE CaskLibTargets.cmake
    NAMESPACE Cask::)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX CASKLIB
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/casklib_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/CaskLibConfigVersion.cmake"
    SOVERSION ${PROJECT_VERSION_MAJOR})

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/casklib_version.h"
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Maui/Cask
    COMPONENT Devel)

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    Core
    Quick
    Concurrent
    DBus
    Sql)

find_package(KF6 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS
    I18n
    Service
    KIO)

find_package(MauiKit4 REQUIRED COMPONENTS FileBrowsing)

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
