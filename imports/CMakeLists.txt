
find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS Qml Quick Sql Svg QuickControls2)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS I18n Notifications Config KIO Attica)

find_package(ECM ${KF5_MIN_VER} REQUIRED NO_MODULE)
find_package(ECM 0.0.11 REQUIRED NO_MODULE)
find_package(MauiKit REQUIRED)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMQMLModules)
include(CheckIncludeFiles)
include(KDECMakeSettings)
include(KDEInstallDirs)

set(cask_SRCS
    Cask/plugin.cpp
    Cask/models/appsmodel.cpp
)
add_library(caskplugin SHARED ${cask_SRCS})

target_link_libraries(caskplugin
                      MauiKit
                      KF5::Attica
                      Qt5::Core
                      Qt5::DBus
                      Qt5::Quick)

install(TARGETS caskplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/maui/cask)
install(DIRECTORY Cask/qml/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/maui/cask)
install(FILES Cask/qml/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/maui/cask)
