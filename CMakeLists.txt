cmake_minimum_required(VERSION 3.16)

set(CASK_VERSION 0.6.6)
project(MauiShell VERSION ${CASK_VERSION})

set(REQUIRED_QT_VERSION 6.4)
set(REQUIRED_KF_VERSION 5.109.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(ECM ${REQUIRED_KF_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(ECMConfiguredInstall)
include(ECMQtDeclareLoggingCategory)
include(ECMSourceVersionControl)
include(ECMGenerateHeaders)

include(FeatureSummary)
include(GenerateExportHeader)
include(FeatureSummary)

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS
    Qml
    Quick
    Svg
    QuickControls2
    WaylandCompositor)

find_package(KF6 ${REQUIRED_KF_VERSION} REQUIRED COMPONENTS
    I18n
    Config
    Service)

find_package(MauiKit4)
find_package(MauiMan4)
find_package(CaskServer)
find_package(MauiCore REQUIRED COMPONENTS Audio Notifications Power)

if(${ECM_SOURCE_UNDER_VERSION_CONTROL})
    execute_process(COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    # Get the latest abbreviated commit hash of the working branch
    execute_process(COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
    add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")

else()
    add_definitions(-DGIT_COMMIT_HASH="${CASK_VERSION}")
    add_definitions(-DGIT_BRANCH="Stable")
endif()

ecm_setup_version(${CASK_VERSION}
    VARIABLE_PREFIX CASK
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/cask_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/CaskConfigVersion.cmake"
    SOVERSION ${PROJECT_VERSION_MAJOR})

#set(QML_IMPORT_PATH ${CMAKE_SOURCE_DIR}/plugins ${CMAKE_BINARY_DIR}/org CACHE STRING "" FORCE)

add_subdirectory(casklib)
add_subdirectory(plugins)
#add_subdirectory(startcask)
add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
