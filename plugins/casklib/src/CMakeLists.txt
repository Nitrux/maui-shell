include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

set(caskplugin_SRCS
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
    )

set(caskplugin_HDRS
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
    )

set(mpris2_SRCS
    code/mpris2/mpris2engine.cpp
    code/mpris2/mpris2engine.h
    code/mpris2/mpris2player.cpp
    code/mpris2/mpris2player.h
    code/mpris2/playersmodel.cpp
    )

set_source_files_properties(code/mpris2/org.freedesktop.DBus.Properties.xml PROPERTIES NO_NAMESPACE true)
qt_add_dbus_interface(mpris2_SRCS code/mpris2/org.freedesktop.DBus.Properties.xml properties_interface)

set_source_files_properties(code/mpris2/org.mpris.MediaPlayer2.Player.xml PROPERTIES NO_NAMESPACE true)
qt_add_dbus_interface(mpris2_SRCS code/mpris2/org.mpris.MediaPlayer2.Player.xml player_interface)

set_source_files_properties(code/mpris2/org.mpris.MediaPlayer2.xml PROPERTIES NO_NAMESPACE true)
qt_add_dbus_interface(mpris2_SRCS code/mpris2/org.mpris.MediaPlayer2.xml mediaplayer2_interface)

set(caskplugin_ASSETS
    )

if(QUICK_COMPILER)
    qtquick_compiler_add_resources(caskplugin_QML_QRC resources.qrc
        )
else()
    qt5_add_resources(caskplugin_QML_QRC resources.qrc
        )
endif()

add_library(${PROJECT_NAME}
    SHARED
    ${caskplugin_SRCS}
    ${caskplugin_HDRS}
    ${caskplugin_ASSETS}
    ${mpris2_SRCS}
    ${caskplugin_QML_QRC}

    ${dbus_SRCS}
    )

generate_export_header(${PROJECT_NAME} BASE_NAME CaskLib)
#set_target_properties(${PROJECT_NAME} PROPERTIES
#    VERSION ${CASKLIB_VERSION_STRING}
#    SOVERSION ${CASKLIB_SOVERSION}
#    EXPORT_NAME CaskLib
#    )

target_include_directories(${PROJECT_NAME}
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/Maui/Cask>")

target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    MauiMan::MauiMan
    CaskServer::CaskServerLib
    MauiKit
    MauiKit::FileBrowsing
    KF5::Service
    KF5::ConfigCore
    KF5::KIOCore
    KF5::KIOFileWidgets
    Qt5::Core
    Qt5::DBus
    Qt5::Sql
    Qt5::Quick)

install(TARGETS ${PROJECT_NAME} EXPORT CaskLibTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${caskplugin_HDRS}
    ${CMAKE_CURRENT_BINARY_DIR}/casklib_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Maui/Cask
    COMPONENT Devel)

if (BUILD_SHARED_LIBS)
    add_custom_target(copy_to_bin ALL
        COMMAND ${CMAKE_COMMAND} -E
        make_directory ${CMAKE_BINARY_DIR}/bin/org/maui/cask/
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/controls ${CMAKE_BINARY_DIR}/bin/org/maui/cask/
        COMMAND ${CMAKE_COMMAND} -E
        copy $<TARGET_FILE:CaskLib> ${CMAKE_BINARY_DIR}/bin/org/maui/cask/
        )

    if(QUICK_COMPILER)
        install(FILES controls/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/maui/cask)
    else()
        install(DIRECTORY controls/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/maui/cask)
    endif()

endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${KDE_INSTALL_QMLDIR}/org/maui/cask)
