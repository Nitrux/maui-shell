
find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS Qml Quick Sql Svg QuickControls2)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS I18n Notifications Config KIO Service)

find_package(ECM ${KF5_MIN_VER} REQUIRED NO_MODULE)
find_package(ECM 0.0.11 REQUIRED NO_MODULE)
find_package(MauiKit REQUIRED FileBrowsing)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMQMLModules)
include(CheckIncludeFiles)
include(KDECMakeSettings)
include(KDEInstallDirs)

set(caskplugin_SRCS
    caskplugin.cpp
    models/appsmodel.cpp
)

set(caskplugin_HDRS
    models/appsmodel.h
    )

set(caskplugin_ASSETS
   caskplugin.qrc
    )

add_library(caskplugin SHARED ${caskplugin_SRCS} ${caskplugin_HDRS} ${caskplugin_ASSETS})
target_link_libraries(caskplugin
                      MauiKit
                      MauiKit::FileBrowsing
                      KF5::Service
                      KF5::ConfigCore
                      KF5::KIOCore
                      KF5::KIOFileWidgets
                      Qt5::Core
                      Qt5::DBus
                      Qt5::Quick)

install(TARGETS caskplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/maui/cask)
install(DIRECTORY qml/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/maui/cask)
install(FILES qml/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/maui/cask)
