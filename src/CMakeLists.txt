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
    code/controllers/waylandcursorgrabber.cpp
    )

set(cask_ASSETS
    qml.qrc
    assets/assets.qrc
    )

add_executable(${PROJECT_NAME}
    ${cask_SRCS}
    ${cask_HDRS}
    ${cask_ASSETS}
    )

target_link_libraries(${PROJECT_NAME} CaskLib Qt5::Qml Qt5::Quick Qt5::QuickControls2 Qt5::Svg KF5::Service Qt5::WaylandCompositor KF5::I18n KF5::ConfigCore)

install(TARGETS ${PROJECT_NAME} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


#install(FILES org.kde.pix.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

#TODO: port to ecm_install_icons()
#install(FILES assets/pix.svg DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps)
