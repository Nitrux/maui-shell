include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/code)

# add_library(${PROJECT_NAME} SHARED)    
ecm_add_qml_module(${PROJECT_NAME} URI "org.cask.polkit" VERSION 1.0 CLASS_NAME CaskPolkitPlugin)

ecm_target_qml_sources(${PROJECT_NAME} SOURCES
    controls/PolkitDialog.qml)

target_sources(${PROJECT_NAME}
    PRIVATE
      code/polkitagentlistener.cpp
    code/dialog.cpp
    code/agent.cpp
    code/caskpolkitplugin.cpp)

generate_export_header(${PROJECT_NAME} BASE_NAME CaskPolkit)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME "CaskPolkit")

target_include_directories(${PROJECT_NAME}
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/Cask/Polkit>")

target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

target_link_libraries(${PROJECT_NAME}
PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::DBus
    KF6::I18n

    ${POLKITQT-1_CORE_LIBRARY}
    ${POLKITQT-1_AGENT_LIBRARY}
    ${POLKIT_AGENT_LDFLAGS})

ecm_finalize_qml_module(${PROJECT_NAME} DESTINATION ${KDE_INSTALL_QMLDIR})
ecm_generate_qmltypes(org.cask.polkit 1.0 DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/polkit)

install(TARGETS ${PROJECT_NAME} EXPORT CaskPolkitTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${headers}
    ${CMAKE_CURRENT_BINARY_DIR}/caskpolkit_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Cask/Polkit
    COMPONENT Devel)



