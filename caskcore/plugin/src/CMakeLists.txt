

# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

    # add_library(${PROJECT_NAME})
ecm_add_qml_module(${PROJECT_NAME}
URI "org.maui.cask" 
VERSION 1.0
CLASS_NAME CaskCorePlugin)

###MPRIS
set(mpris2_SRCS
    code/mpris2/mpris2engine.cpp
    code/mpris2/mpris2engine.h
    code/mpris2/mpris2player.cpp
    code/mpris2/mpris2player.h
    code/mpris2/playersmodel.cpp)

ecm_target_qml_sources(${PROJECT_NAME} SOURCES
    controls/IconLabel.qml
    controls/Dashboard.qml)

ecm_target_qml_sources(${PROJECT_NAME} PATH templates SOURCES
    controls/templates/Panel.qml
    controls/templates/PanelItem.qml
    controls/templates/PanelSection.qml
    controls/templates/PanelPopup.qml
    controls/templates/PanelCard.qml)

set_source_files_properties(code/mpris2/org.freedesktop.DBus.Properties.xml PROPERTIES NO_NAMESPACE true)
qt_add_dbus_interface(mpris2_SRCS code/mpris2/org.freedesktop.DBus.Properties.xml properties_interface)

set_source_files_properties(code/mpris2/org.mpris.MediaPlayer2.Player.xml PROPERTIES NO_NAMESPACE true)
qt_add_dbus_interface(mpris2_SRCS code/mpris2/org.mpris.MediaPlayer2.Player.xml player_interface)

set_source_files_properties(code/mpris2/org.mpris.MediaPlayer2.xml PROPERTIES NO_NAMESPACE true)
qt_add_dbus_interface(mpris2_SRCS code/mpris2/org.mpris.MediaPlayer2.xml mediaplayer2_interface)

###POWER MANAGEMENT
set(power_SRCS
    code/power/powermanager.cpp)

set(power_HDRS
    code/power/powermanager.h)

set_source_files_properties(code/power/org.freedesktop.login1.Manager.xml
    code/power/org.freedesktop.login1.Seat.xml
    code/power/org.freedesktop.login1.Session.xml
    PROPERTIES INCLUDE "code/power/loginddbustypes.h")

qt_add_dbus_interface(power_SRCS code/power/org.freedesktop.login1.Manager.xml login1_manager_interface)
# The autogenerated cpp file hardcodes the interface name given to the QDBusAbstractAdaptor
# This "hack" swaps the implementation for one that can switch between login1 and consolekit2
#list(REMOVE_ITEM power_SRCS "${CMAKE_CURRENT_BINARY_DIR}/login1_manager_interface.cpp")
#list(APPEND power_SRCS code/power/login1_manager_interface.cpp)

get_property(output SOURCE "${CMAKE_CURRENT_BINARY_DIR}/login1_manager_interface.cpp" PROPERTY OBJECT_DEPENDS)
set_source_files_properties(code/power/login1_manager_interface.cpp
    PROPERTIES
    OBJECT_DEPENDS ${output}
    SKIP_AUTOMOC TRUE
    SKIP_AUTOUIC TRUE)

qt_add_dbus_interface(power_SRCS code/power/org.freedesktop.UPower.xml upower_interface)


target_sources(${PROJECT_NAME}
    PRIVATE
    resources.qrc
    
    code/caskplugin.cpp
    code/models/appsmodel.cpp
    code/models/recentfilesmodel.cpp
    code/models/recentappsmodel.cpp
    code/stackableitem.cpp
    code/enviroment.cpp
    code/processlauncher.cpp
    code/controllers/panelsection.cpp
    code/controllers/appsdb.cpp
    code/controllers/db.cpp
    code/controllers/mauimaninterface.cpp
    code/server/caskserver.cpp
    
    code/caskplugin.h
    code/models/appsmodel.h
    code/models/recentfilesmodel.h
    code/models/recentappsmodel.h
    code/stackableitem.h
    code/enviroment.h
    code/processlauncher.h
    code/controllers/panelsection.h
    code/controllers/appsdb.h
    code/controllers/db.h
    code/controllers/mauimaninterface.h
    code/server/caskserver.h
    
    ${mpris2_SRCS}
    ${power_SRCS}
    ${dbus_SRCS})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    MauiMan4::MauiMan4
    CaskServer::CaskServerLib
    MauiCore::Audio
    MauiCore::Notifications
    MauiKit4
    MauiKit4::FileBrowsing
    KF6::I18n
    KF6::Service
    KF6::ConfigCore
    KF6::KIOCore
    KF6::KIOFileWidgets
    Qt6::Core
    Qt6::Qml
    Qt6::DBus
    Qt6::Sql
    Qt6::Quick)

ecm_finalize_qml_module(${PROJECT_NAME} DESTINATION ${KDE_INSTALL_QMLDIR})
ecm_generate_qmltypes(org.maui.cask 1.0 DESTINATION ${KDE_INSTALL_QMLDIR}/org/maui/cask)
