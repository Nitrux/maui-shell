
find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS Qml Quick)

set(plugins_SRCS
    processlauncher.cpp
    stackableitem.cpp
    enviroment.cpp
    )

set(plugins_HDRS
    processlauncher.h
    stackableitem.h
    enviroment.h
    )

add_subdirectory(cask)
add_library(plugins STATIC ${plugins_SRCS} ${plugins_HDRS})
target_link_libraries(plugins caskplugin Qt5::Qml Qt5::Quick)
