add_library(CaskAutostart STATIC caskautostart.cpp)
target_link_libraries(CaskAutostart KF6::CoreAddons KF6::ConfigCore)
