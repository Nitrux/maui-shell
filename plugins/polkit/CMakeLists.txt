
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

set(AUTOMOC_MOC_OPTIONS -Muri=org.cask.polkit)

project(CaskPolkit LANGUAGES CXX VERSION 0.5.0)

find_package(ECM ${KF5_VERSION} NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

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

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/CaskPolkit")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CaskPolkitConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CaskPolkitConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS KF5_INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CaskPolkitConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CaskPolkitConfig.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT CaskPolkitTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE CaskPolkit.cmake
    NAMESPACE Cask::)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX ${PROJECT_NAME}
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/CaskPolkit_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/CaskPolkitConfigVersion.cmake"
    SOVERSION ${PROJECT_VERSION_MAJOR}
    )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/CaskPolkit_version.h"
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Cask/Power
    COMPONENT Devel)

option(QUICK_COMPILER "Use QtQuick compiler to improve performance" TRUE)

find_package(PkgConfig)
pkg_check_modules(POLKIT_AGENT REQUIRED polkit-agent-1)
message(STATUS "polkit agent: ${POLKIT_AGENT_INCLUDE_DIRS} ${POLKIT_AGENT_LIBRARIES}")

find_package(PolkitQt5-1 REQUIRED)

include_directories(
    ${POLKIT_AGENT_INCLUDE_DIRS}
    "${POLKITQT-1_INCLUDE_DIR}"
)

find_package(Qt5 ${REQUIRED_QT_VERSION} COMPONENTS Core Qml Quick DBus Gui REQUIRED)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS I18n Solid IdleTime CoreAddons Service)

if(QUICK_COMPILER)
        find_package(Qt5QuickCompiler)
        set_package_properties(Qt5QuickCompiler PROPERTIES
                DESCRIPTION "Compile QML at build time"
                TYPE OPTIONAL
        )
endif()

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)


