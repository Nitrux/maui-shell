# SPDX-FileCopyrightText: 2021 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

project(CaskCoreLib LANGUAGES CXX VERSION ${CASK_VERSION})

#DEFAULT COMPONENTS DEFINITIONS
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

include(KDEClangFormat)
include(KDEInstallDirs)
include(KDECMakeSettings)

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    Core
    Quick
    Qml
    Concurrent
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
    
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/CaskCoreLib")

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX CaskCoreLib
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/caskcorelib_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/CaskCoreLibConfigVersion.cmake"
    SOVERSION ${PROJECT_VERSION_MAJOR})

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CaskCoreLibConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CaskCoreLibConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS CMAKE_INSTALL_PREFIX)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CaskCoreLibConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CaskCoreLibConfig.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel)

install(EXPORT CaskCoreLibTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE CaskLibTargets.cmake
    NAMESPACE Cask::)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/caskcorelib_version.h"
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Maui/Cask
    COMPONENT Devel)

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
