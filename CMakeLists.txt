cmake_minimum_required(VERSION 3.16)

set(CASK_VERSION 0.6.6)
project(MauiShell VERSION ${CASK_VERSION})

set(QT_MAJOR_VERSION 6)

set(REQUIRED_QT_VERSION 6.4)
set(REQUIRED_KF_VERSION 5.240.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${REQUIRED_KF_VERSION} NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(ECMSourceVersionControl)
include(ECMSetupVersion)

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

add_subdirectory(caskcore) ##plugin and the public library
add_subdirectory(plugins)
add_subdirectory(startcask)
add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
