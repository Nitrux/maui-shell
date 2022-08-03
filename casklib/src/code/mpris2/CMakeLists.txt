liri_add_qml_plugin(MprisQmlPlugin
    MODULE_PATH
        "Liri/Mpris"
    VERSION
        "1.0"
    OUTPUT_NAME
        mpris2plugin
    SOURCES
        mpris2engine.cpp
        mpris2engine.h
        mpris2player.cpp
        mpris2player.h
        plugin.cpp
    QML_FILES
        qmldir
    DBUS_INTERFACE_SOURCES
        org.freedesktop.DBus.Properties.xml
        org.mpris.MediaPlayer2.Player.xml
        org.mpris.MediaPlayer2.xml
    DBUS_INTERFACE_FLAGS "-N"
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES

)

liri_finalize_qml_plugin(MprisQmlPlugin)


# SPDX-FileCopyrightText: 2021 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

set(REQUIRED_QT_VERSION 5.15.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(AUTOMOC_MOC_OPTIONS -Muri=org.maui.cask)

project(MprisPlugin LANGUAGES CXX VERSION 1.0.0)

find_package(ECM ${KF5_VERSION} NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

option(QUICK_COMPILER "Use QtQuick compiler to improve performance" TRUE)

include(GenerateExportHeader)
include(GNUInstallDirs)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)
include(ECMPoQmTools)
include(ECMSetupVersion)
include(ECMGenerateQmlTypes)
include(CMakePackageConfigHelpers)

find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Core DBus)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS I18n Service KIO )
find_package(MauiKit REQUIRED)

if(QUICK_COMPILER)
    find_package(Qt5QuickCompiler)
    set_package_properties(Qt5QuickCompiler PROPERTIES
        DESCRIPTION "Compile QML at build time"
        TYPE OPTIONAL
        )
endif()

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
