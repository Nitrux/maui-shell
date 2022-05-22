# SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
#
# SPDX-License-Identifier: BSD-2-Clause

ecm_create_qm_loader(casknotifications_QM_LOADER libcasknotificationsplugin_qt)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/code
    )

set(
    SRCS
    code/notificationimagesstorage.cpp
    code/notificationserver.cpp
    code/notifications.cpp
    code/notificationsimageutils.cpp
    code/notificationsimageprovider.cpp
    code/plugin.cpp
    code/notification.cpp
    code/notificationsmodel.cpp
    code/historymodel.cpp
    code/datehelper.cpp


    ${casknotifications_QM_LOADER}
    )

set(
    headers
    code/notificationimagesstorage.h
    code/notificationserver.h
#    code/notifications_plugin.h

    code/notifications.h
    code/notificationsimageutils.h
    code/notificationsimageprovider.h
    code/notification.h
    code/notificationsmodel.h
    code/historymodel.h
    code/datehelper.h
    )

set(
    pluginData
    controls/qmldir
    )

qt_add_dbus_interface(SRCS code/org.freedesktop.Notifications.xml notifications_interface )

qt_add_dbus_adaptor(SRCS code/org.freedesktop.Notifications.xml code/notificationserver.h NotificationServer notifications_adaptor)

if(QUICK_COMPILER)
    qtquick_compiler_add_resources(casknotifications_QML_QRC resources.qrc)
else()
    qt5_add_resources(casknotifications_QML_QRC resources.qrc)
endif()

ecm_generate_qmltypes(org.cask.notifications ${PROJECT_VERSION} DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/notifications)

add_library(
    CaskNotifications
    SHARED
    ${SRCS}
    ${headers}
    )

generate_export_header(CaskNotifications BASE_NAME CaskNotifications)
#set_target_properties(CaskNotifications PROPERTIES
#    VERSION ${CaskNotifications_VERSION_STRING}
#    SOVERSION ${CaskNotifications_SOVERSION}
#    EXPORT_NAME CaskNotifications
#    )

target_include_directories(CaskNotifications
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/Cask/Notifications>")

target_include_directories(CaskNotifications PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

if(QUICK_COMPILER)
    add_definitions(-DQUICK_COMPILER)
    target_compile_definitions(CaskNotifications PUBLIC QUICK_COMPILER)
endif()

target_link_libraries(
    CaskNotifications
    PRIVATE
    Qt5::Core
    Qt5::Quick
    Qt5::DBus
    Qt5::Gui
    KF5::I18n
    )

install(TARGETS CaskNotifications EXPORT CaskNotificationsTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${headers}
    ${CMAKE_CURRENT_BINARY_DIR}/casknotifications_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Cask/Notifications
    COMPONENT Devel)

if (BUILD_SHARED_LIBS)
    add_custom_target(copy_notifications_to_bin ALL
        COMMAND ${CMAKE_COMMAND} -E
        make_directory ${CMAKE_BINARY_DIR}/bin/org/cask/notifications/
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/controls ${CMAKE_BINARY_DIR}/bin/org/cask/notifications/
        COMMAND ${CMAKE_COMMAND} -E
        copy $<TARGET_FILE:CaskNotifications> ${CMAKE_BINARY_DIR}/bin/org/cask/notifications/
        )

    if(QUICK_COMPILER)
        install(FILES controls/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/notifications)
    else()
        install(DIRECTORY controls/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/notifications)
    endif()

    #include(ECMGeneratePriFile)
    #ecm_generate_pri_file(BASE_NAME KQuickImageEditor LIB_NAME KQuickImageEditor DEPS "core qml quick svg" FILENAME_VAR PRI_FILENAME )
    #install(FILES ${PRI_FILENAME}
    #DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

endif()

install(TARGETS CaskNotifications DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/notifications )
