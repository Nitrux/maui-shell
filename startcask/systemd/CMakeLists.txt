ecm_install_configured_files(INPUT plasma-ksplash-ready.service.in @ONLY
                                                   DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR})

install(FILES plasma-core.target DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR})
install(FILES plasma-workspace.target DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR})
install(FILES plasma-workspace-wayland.target DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR})
install(FILES plasma-workspace-x11.target DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR})

add_executable(kde-systemd-start-condition kde-systemd-start-condition.cpp)
target_link_libraries(kde-systemd-start-condition PUBLIC KF6::ConfigCore KF6::Service PlasmaAutostart)
install(TARGETS kde-systemd-start-condition DESTINATION ${KDE_INSTALL_BINDIR})
