cmake_minimum_required(VERSION 3.16)

set(CASK_VERSION 0.5.5)
project(cask VERSION ${CASK_VERSION})

set(REQUIRED_QT_VERSION 5.14.0)
set(REQUIRED_KF5_VERSION 5.60)

find_package(ECM ${REQUIRED_KF5_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})
set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS Qml Quick Svg QuickControls2 WaylandCompositor)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS I18n Config Service)

find_package(MauiKit)
find_package(MauiMan)
find_package(CaskServer)
find_package(MauiCore REQUIRED COMPONENTS Audio Notifications Power)

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(ECMConfiguredInstall)
include(ECMQtDeclareLoggingCategory)
include(ECMSourceVersionControl)

include(FeatureSummary)

if(${ECM_SOURCE_UNDER_VERSION_CONTROL})
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

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
    SOVERSION 1
    )

set(QML_IMPORT_PATH ${CMAKE_SOURCE_DIR}/plugins ${CMAKE_BINARY_DIR}/org CACHE STRING "" FORCE)

add_subdirectory(casklib)
add_subdirectory(plugins)
add_subdirectory(startcask)
add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
