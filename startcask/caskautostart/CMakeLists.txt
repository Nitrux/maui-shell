add_library(PlasmaAutostart STATIC plasmaautostart.cpp)
target_link_libraries(PlasmaAutostart KF5::CoreAddons KF5::ConfigCore)
