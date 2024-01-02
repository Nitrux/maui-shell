# SPDX-FileCopyrightText: 2021 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(AUTOMOC_MOC_OPTIONS -Muri=org.maui.cask)

project(CaskCorePlugin LANGUAGES CXX VERSION 1.0.0)

include(ECMFindQmlModule)
include(ECMQmlModule)
include(ECMGenerateQmlTypes)

include(KDEClangFormat)
include(KDEInstallDirs)
include(KDECMakeSettings)

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    Core
    Quick
    Qml
    Concurrent
    QuickControls2
    DBus
    Sql)

find_package(KF6 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS
    I18n
    Service
    KIO)

find_package(CaskServer)
find_package(MauiCore REQUIRED COMPONENTS Audio Notifications Power)
find_package(MauiKit4 REQUIRED COMPONENTS FileBrowsing)

find_package(MauiMan4 REQUIRED)
set_package_properties(MauiMan${MAUI_MAJOR_VERSION} PROPERTIES
    TYPE REQUIRED
    DESCRIPTION "MauiMan is a server and API for syncing global system setting preferences."
    URL "https://invent.kde.org/maui/mauiman"
    PURPOSE "Sync to system setting preferences.")

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
