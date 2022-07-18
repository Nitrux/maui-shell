find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    DocTools Runner Notifications NotifyConfig Su Wallet IdleTime
    Declarative I18n Crash GlobalAccel DBusAddons Wayland
    CoreAddons People ActivitiesStats Activities KIO Prison Package
    Archive IconThemes UnitConversion ItemModels Init TextEditor)

find_package(KDED CONFIG REQUIRED)
find_package(CaskServer REQUIRED)

find_package(Fontconfig)
set_package_properties(Fontconfig PROPERTIES DESCRIPTION "Font access configuration library"
    URL "https://www.freedesktop.org/wiki/Software/fontconfig"
    TYPE OPTIONAL
    PURPOSE "Needed to build font configuration and installation tools"
    )

if(FONTCONFIG_FOUND)
    # kfontinst
    find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS PrintSupport)
endif()

find_package(Phonon4Qt5 4.6.60 REQUIRED NO_MODULE)
set_package_properties(Phonon4Qt5 PROPERTIES
    DESCRIPTION "Qt-based audio library"
    TYPE REQUIRED)

add_subdirectory(caskautostart)
#add_subdirectory(kcminit)
#add_subdirectory(waitforname)

#if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
#    add_subdirectory(systemd)
#endif()

#add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
#add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)

#qt_add_dbus_interface(
#startplasma_SRCS
#${CMAKE_SOURCE_DIR}/ksplash/ksplashqml/org.kde.KSplash.xml
#ksplashinterface
#)

set(startcask_SRCS
    autostartscriptdesktopfile.cpp
    updatelaunchenvjob.cpp
    )

ecm_qt_declare_logging_category(startcask_SRCS HEADER debug.h IDENTIFIER CASK_STARTUP CATEGORY_NAME org.cask.startup)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

add_library(startcask OBJECT startcask.cpp ${startcask_SRCS})
target_link_libraries(startcask PUBLIC
    Qt::Core
    Qt::DBus
    CaskServer::CaskServerLib
    KF5::ConfigCore
    KF5::Notifications
    KF5::Package
    ${PHONON_LIBRARIES}
    )

add_executable(startcask-wayland ${START_CASK_COMMON_SRCS} startcask-wayland.cpp)

target_link_libraries(startcask-wayland PRIVATE
    startcask
    )
message( "CASKSTARTUP" ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(cask-session)
#add_subdirectory(plasma-shutdown)

#FIXME: reconsider, looks fishy
if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    set_property(SOURCE startcask.cpp APPEND PROPERTY COMPILE_DEFINITIONS
        XCURSOR_PATH="${KDE_INSTALL_FULL_DATAROOTDIR}/icons:$XCURSOR_PATH:~/.icons:/usr/share/icons:/usr/share/pixmaps:/usr/X11R6/lib/X11/icons")
endif()

   set(CASK_WAYLAND_DESKTOP_FILENAME cask-session.desktop CACHE INTERNAL "Filename for Cask Wayland session")

configure_file(config-startcask.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-startcask.h)
configure_file(cask-session.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/${CASK_WAYLAND_DESKTOP_FILENAME})

install(TARGETS startcask-wayland ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS cask-sourceenv.sh DESTINATION ${KDE_INSTALL_LIBEXECDIR})
install(PROGRAMS cask-dbus-run-session-if-needed DESTINATION ${KDE_INSTALL_LIBEXECDIR})

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${CASK_WAYLAND_DESKTOP_FILENAME}
        DESTINATION ${KDE_INSTALL_DATADIR}/wayland-sessions
)

install(FILES cask.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES cask.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR})

