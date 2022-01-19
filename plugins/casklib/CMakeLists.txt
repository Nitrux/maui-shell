# SPDX-FileCopyrightText: 2021 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

set(KF5_VERSION "5.70.0")
set(REQUIRED_QT_VERSION 5.15.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(AUTOMOC_MOC_OPTIONS -Muri=org.maui.cask)

project(CaskLib LANGUAGES CXX VERSION 1.0.0)

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
include(KDEPackageAppTemplates)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/CaskLib")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CaskLibConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CaskLibConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS KF5_INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CaskLibConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CaskLibConfig.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT CaskLibTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE CaskLibTargets.cmake
    NAMESPACE Cask::)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX CASKLIB
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/casklib_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/CaskLibConfigVersion.cmake"
    SOVERSION 1
    )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/casklib_version.h"
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Maui/Cask
    COMPONENT Devel)

find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Core Quick Concurrent DBus)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS I18n Service KIO )
find_package(MauiKit REQUIRED)
find_package(MauiKit REQUIRED COMPONENTS FileBrowsing)

if(QUICK_COMPILER)
    find_package(Qt5QuickCompiler)
    set_package_properties(Qt5QuickCompiler PROPERTIES
        DESCRIPTION "Compile QML at build time"
        TYPE OPTIONAL
        )
endif()

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
