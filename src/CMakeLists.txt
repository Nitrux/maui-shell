set(cask_SRCS
    main.cpp
    models/zpacemodel.cpp
    models/appsmodel.cpp
    models/zpaceitem.cpp
    models/zpace.cpp
    models/appitem.cpp
    controllers/cask.cpp
    )

set(cask_ASSETS
    qml.qrc
    assets/assets.qrc
    )

include_directories(controllers)
include_directories(models)

add_subdirectory(plugins)

add_executable(cask
    ${cask_SRCS}
    ${cask_HDRS}
    ${cask_ASSETS}
    )

target_link_libraries(cask caskplugin plugins Qt5::Qml Qt5::Quick Qt5::Svg Qt5::WaylandCompositor KF5::I18n)

if(ANDROID)
    target_link_libraries(cask Qt5::AndroidExtras)
#    kde_source_files_enable_exceptions(cask pix.cpp)
endif()

install(TARGETS cask ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
#install(FILES org.kde.pix.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

#TODO: port to ecm_install_icons()
#install(FILES assets/pix.svg DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps)


add_executable(
    startcask

    startcask.cpp
)

target_link_libraries(
    startcask

    Qt5::Core
)

install(TARGETS startcask DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
