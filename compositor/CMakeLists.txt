set(cask_SRCS
    main.cpp
    processlauncher.cpp
    stackableitem.cpp
    )

set(cask_HDRS
    processlauncher.h
    stackableitem.h
    )

set(cask_ASSETS
    grefsen.qrc
#    imgs.qrc
    )

add_executable(cask
    ${cask_SRCS}
    ${cask_HDRS}
    ${cask_ASSETS}
    )

target_link_libraries(cask Qt5::Qml Qt5::Quick Qt5::Svg KF5::I18n)

if(ANDROID)
    target_link_libraries(cask Qt5::AndroidExtras)
#    kde_source_files_enable_exceptions(cask pix.cpp)
endif()

install(TARGETS cask ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
#install(FILES org.kde.pix.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

#TODO: port to ecm_install_icons()
#install(FILES assets/pix.svg DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps)
