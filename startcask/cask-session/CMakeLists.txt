add_subdirectory(plasma-autostart-list)

set(plasma_session_SRCS
    main.cpp
    autostart.cpp
    startup.cpp
    sessiontrack.cpp
    signalhandler.cpp
)

ecm_qt_declare_logging_category(plasma_session_SRCS  HEADER debug.h IDENTIFIER PLASMA_SESSION CATEGORY_NAME org.kde.plasma.session)

qt_add_dbus_adaptor( plasma_session_SRCS org.kde.Startup.xml startup.h Startup)

set(kcminit_adaptor ${plasma-workspace_SOURCE_DIR}/startkde/kcminit/main.h)
set(kcminit_xml ${CMAKE_CURRENT_BINARY_DIR}/org.kde.KCMinit.xml)
qt5_generate_dbus_interface( ${kcminit_adaptor} ${kcminit_xml} )
qt_add_dbus_interface( plasma_session_SRCS ${kcminit_xml} kcminit_interface )
qt_add_dbus_interface( plasma_session_SRCS ${KDED_DBUS_INTERFACE} kded_interface )

qt_add_dbus_interface( plasma_session_SRCS ../../ksmserver/org.kde.KSMServerInterface.xml ksmserver_interface )

add_executable(plasma_session ${plasma_session_SRCS})

target_include_directories(plasma_session PRIVATE ${CMAKE_SOURCE_DIR}/startkde ${CMAKE_BINARY_DIR}/startkde)
target_link_libraries(plasma_session
    startplasma
    KF5::KIOCore
    PlasmaAutostart
)

install(TARGETS plasma_session ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

