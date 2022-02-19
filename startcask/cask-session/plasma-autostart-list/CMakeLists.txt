add_executable(plasma-autostart-list main.cpp ../autostart.cpp)
target_link_libraries(plasma-autostart-list KF5::Service PlasmaAutostart)
