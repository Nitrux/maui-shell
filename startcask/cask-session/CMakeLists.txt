add_subdirectory(cask-autostart-list)

set(cask_session_SRCS
    main.cpp
    autostart.cpp
    startup.cpp
    sessiontrack.cpp
    signalhandler.cpp
)

ecm_qt_declare_logging_category(cask_session_SRCS  HEADER debug.h IDENTIFIER CASK_SESSION CATEGORY_NAME org.cask.session)

#qt_add_dbus_adaptor( plasma_session_SRCS org.kde.Startup.xml startup.h Startup)

#set(kcminit_adaptor ${plasma-workspace_SOURCE_DIR}/startkde/kcminit/main.h)
#set(kcminit_xml ${CMAKE_CURRENT_BINARY_DIR}/org.kde.KCMinit.xml)
#qt5_generate_dbus_interface( ${kcminit_adaptor} ${kcminit_xml} )
#qt_add_dbus_interface( plasma_session_SRCS ${kcminit_xml} kcminit_interface )
#qt_add_dbus_interface( cask_session_SRCS ${KDED_DBUS_INTERFACE} kded_interface )

#qt_add_dbus_interface( plasma_session_SRCS ../../ksmserver/org.kde.KSMServerInterface.xml ksmserver_interface )

add_executable(cask_session ${cask_session_SRCS})

target_include_directories(cask_session PRIVATE ${CMAKE_SOURCE_DIR}/startcask ${CMAKE_BINARY_DIR}/startcask)
target_link_libraries(cask_session
    CaskAutostart
    startcask
    KF5::KIOCore
)

install(TARGETS cask_session ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

