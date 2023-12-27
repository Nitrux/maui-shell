project(cask VERSION ${CASK_VERSION})

set(PROJECT_URI org.cask.shell)

include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(ECMGenerateHeaders)
include(ECMFindQmlModule)
include(ECMQmlModule)
include(ECMGenerateQmlTypes)

include(KDEClangFormat)
include(KDEInstallDirs)
include(KDECMakeSettings)

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS
    Qml
    Quick
    Svg
    QuickControls2
    WaylandCompositor)

find_package(KF6 ${REQUIRED_KF_VERSION} REQUIRED COMPONENTS
    I18n
    Config
    Service)

find_package(MauiKit4)
find_package(MauiMan4)
find_package(CaskServer)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

set(cask_SRCS
    main.cpp
    code/models/zpacesmodel.cpp
    code/models/surfacesmodel.cpp
    code/controllers/zpaces.cpp
    code/controllers/zpace.cpp
    code/models/tasksmodel.cpp
    code/controllers/task.cpp
    code/controllers/abstractwindow.cpp
    code/controllers/xdgwindow.cpp
    code/controllers/waylandcursorgrabber.cpp)

set(cask_ASSETS
#    qml.qrc
    assets/assets.qrc)

qt_add_executable(${PROJECT_NAME}
    ${cask_SRCS}
    ${cask_HDRS}
    ${cask_ASSETS})

qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_URI}
    VERSION 1.0
    QML_FILES
    qml/main.qml
    qml/Keyboard.qml
    qml/CaskScreen.qml
    qml/shell/statusbar/StatusBar.qml
    qml/shell/tasksbar/TaskBar.qml
    qml/shell/AppsOverview.qml
    qml/shell/statusbar/items/calendar/CalendarItem.qml
    qml/shell/tasksbar/Launcher.qml
    qml/shell/statusbar/items/sliders/SlidersItem.qml
    qml/shell/statusbar/items/player/AudioPlayerItem.qml
    qml/shell/statusbar/items/toggles/ToggleTemplate.qml
    qml/shell/statusbar/items/toggles/TogglesItem.qml
    qml/shell/statusbar/items/player/PlayerCard.qml
    qml/shell/statusbar/items/toggles/sections/network/NetworkToggle.qml
    qml/shell/statusbar/items/toggles/sections/bluetooth/BluetoothToggle.qml
    qml/shell/statusbar/items/notifications/NotificationsItem.qml
    qml/shell/statusbar/items/session/SessionItem.qml
    qml/shell/statusbar/items/toggles/sections/network/NetworkPage.qml
    qml/shell/statusbar/items/toggles/sections/network/AirplaneModeToggle.qml
    qml/shell/statusbar/items/toggles/sections/network/HotspotToggle.qml
    qml/shell/statusbar/items/toggles/sections/network/ConnectionDelegate.qml
    qml/shell/statusbar/items/toggles/sections/bluetooth/BluetoothPage.qml
    qml/shell/statusbar/items/toggles/sections/bluetooth/ConnectionDelegate.qml
    qml/shell/zpaces/WorkZpaceSwitcher.qml
    qml/shell/zpaces/WorkZpace.qml
    qml/shell/zpaces/Zpace.qml
    qml/shell/tasksbar/Dock.qml
    qml/shell/tasksbar/DockItem.qml
    qml/CursorItem.qml
    qml/shell/statusbar/items/notifications/NotificationsContainer.qml
    qml/shell/statusbar/items/notifications/NotificationItem.qml
    qml/shell/statusbar/items/session/SessionButton.qml
    qml/shell/templates/CaskDialog.qml
    qml/shell/statusbar/items/sliders/volume/VolumeSlider.qml
    qml/shell/statusbar/items/sliders/brightness/BrightnessSlider.qml
    qml/shell/statusbar/items/sliders/volume/VolumePage.qml
    qml/shell/templates/StackPage.qml
    qml/shell/statusbar/items/toggles/sections/screenshot/ScreenshotToggle.qml
    qml/shell/statusbar/items/toggles/sections/screenshot/ScreenshotPage.qml
    qml/shell/templates/CardButton.qml
    qml/shell/statusbar/items/session/BatteryToggle.qml
    qml/shell/statusbar/items/sliders/volume/DeviceItem.qml
    qml/shell/templates/SliderToggle.qml
    qml/shell/templates/ProgressToggle.qml
    qml/shell/templates/ProgressBarTemplate.qml
    qml/shell/statusbar/items/sliders/volume/MicSlider.qml
    qml/shell/statusbar/items/sliders/volume/MicPage.qml
    qml/shell/statusbar/items/sliders/brightness/BrightnessPage.qml
    qml/shell/templates/AbstractBarToggle.qml
    qml/shell/templates/SliderTemplate.qml
    qml/shell/statusbar/items/session/PowerPage.qml
    qml/shell/statusbar/items/session/BatteryBar.qml
    qml/shell/statusbar/items/toggles/sections/devices/DevicesToggle.qml
    qml/shell/statusbar/items/toggles/sections/devices/DevicesPage.qml
    qml/shell/statusbar/items/toggles/sections/bluetooth/logic.js
    qml/shell/statusbar/items/player/PlayerPage.qml
    qml/shell/ScreenshotArea.qml
    qml/OSDArea.qml
    qml/chrome/Chrome.qml
    qml/chrome/TitleBar.qml
    qml/chrome/Decoration.qml
    qml/chrome/Border.qml
    qml/chrome/PinchArea.qml)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    MauiKit4
    MauiMan4
    # CaskLib
    CaskServer::CaskServerLib
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
    KF6::Service
    Qt6::WaylandCompositor
    Qt6::DBus
    KF6::I18n
    KF6::ConfigCore)

install(TARGETS ${PROJECT_NAME} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(DIRECTORY assets/wallpapers/ DESTINATION ${KDE_INSTALL_WALLPAPERDIR}/Cask)
