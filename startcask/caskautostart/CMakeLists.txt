add_library(CaskAutostart STATIC caskautostart.cpp)
target_link_libraries(CaskAutostart KF5::CoreAddons KF5::ConfigCore)
