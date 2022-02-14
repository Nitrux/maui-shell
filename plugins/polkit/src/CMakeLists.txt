include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/code
    )

set(caskpolkit_SRCS
    code/polkitagentlistener.cpp
    code/dialog.cpp
    code/agent.cpp
    code/caskpolkitplugin.cpp
)

set(
    pluginData
    controls/qmldir
    )

if(QUICK_COMPILER)
    qtquick_compiler_add_resources(caskpolkit_QML_QRC resources.qrc)
else()
    qt5_add_resources(caskpolkit_QML_QRC resources.qrc)
endif()


ecm_generate_qmltypes(org.cask.polkit ${PROJECT_VERSION} DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/polkit)

add_library(CaskPolkit SHARED ${caskpolkit_SRCS} ${caskpolkit_QML_QRC})

generate_export_header(CaskPolkit BASE_NAME CaskPolkit)

target_include_directories(CaskPolkit
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/Cask/Polkit>")

target_include_directories(CaskPolkit PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

if(QUICK_COMPILER)
    add_definitions(-DQUICK_COMPILER)
    target_compile_definitions(CaskPolkit PUBLIC QUICK_COMPILER)
endif()

target_link_libraries(CaskPolkit
    Qt5::Core
    Qt5::Qml
    KF5::I18n
    Qt::DBus


    ${POLKITQT-1_CORE_LIBRARY}
    ${POLKITQT-1_AGENT_LIBRARY}
    ${POLKIT_AGENT_LDFLAGS}
)


install(TARGETS CaskPolkit EXPORT CaskPolkitTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


install(FILES
    ${headers}
    ${CMAKE_CURRENT_BINARY_DIR}/caskpolkit_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Cask/Polkit
    COMPONENT Devel)

if (BUILD_SHARED_LIBS)
    add_custom_target(copy_polkit_to_bin ALL
        COMMAND ${CMAKE_COMMAND} -E
        make_directory ${CMAKE_BINARY_DIR}/bin/org/cask/polkit/
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/controls ${CMAKE_BINARY_DIR}/bin/org/cask/polkit/
        COMMAND ${CMAKE_COMMAND} -E
        copy $<TARGET_FILE:CaskPolkit> ${CMAKE_BINARY_DIR}/bin/org/cask/polkit/
        )

    if(QUICK_COMPILER)
        install(FILES controls/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/polkit)
    else()
        install(DIRECTORY controls/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/polkit)
    endif()

    #include(ECMGeneratePriFile)
    #ecm_generate_pri_file(BASE_NAME KQuickImageEditor LIB_NAME KQuickImageEditor DEPS "core qml quick svg" FILENAME_VAR PRI_FILENAME )
    #install(FILES ${PRI_FILENAME}
    #DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

endif()

install(TARGETS CaskPolkit DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/polkit )

