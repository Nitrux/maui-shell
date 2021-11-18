cmake_minimum_required(VERSION 3.16)

set(CASK_VERSION 0.0.5)
project(cask VERSION ${CASK_VERSION})

set(REQUIRED_QT_VERSION 5.14.0)
set(REQUIRED_KF5_VERSION 5.60)

find_package(ECM ${REQUIRED_KF5_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})
set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS Qml Quick Svg QuickControls2)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS I18n)

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(FeatureSummary)

ecm_setup_version(${CASK_VERSION}
    VARIABLE_PREFIX CASK
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/cask_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/CaskConfigVersion.cmake"
    SOVERSION 1
    )


set(QML_IMPORT_PATH ${CMAKE_SOURCE_DIR}/plugins ${CMAKE_BINARY_DIR}/org CACHE STRING "" FORCE)

add_subdirectory(plugins)
add_subdirectory(src)


#install(FILES org.kde.pix.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
