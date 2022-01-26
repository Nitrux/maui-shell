include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/code
    )

set(caskpower_SRCS
    code/powermanagementengine.cpp
    code/powermanagementjob.cpp
    code/brightnesscontrol.cpp
    code/batteryinfo.cpp
    code/powerprofile.cpp
    code/caskpowerplugin.cpp
    code/inhibitioncontrol.cpp
)

set(
    pluginData
    controls/qmldir
    )

if(QUICK_COMPILER)
    qtquick_compiler_add_resources(caskpower_QML_QRC resources.qrc)
else()
    qt5_add_resources(caskpower_QML_QRC resources.qrc)
endif()


ecm_generate_qmltypes(org.cask.power ${PROJECT_VERSION} DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/power)

add_library(CaskPower SHARED ${caskpower_SRCS} ${caskpower_QML_QRC})

generate_export_header(CaskPower BASE_NAME CaskPower)

target_include_directories(CaskPower
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/Cask/Power>")

target_include_directories(CaskPower PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

if(QUICK_COMPILER)
    add_definitions(-DQUICK_COMPILER)
    target_compile_definitions(CaskPower PUBLIC QUICK_COMPILER)
endif()

target_link_libraries(CaskPower
    Qt5::Core
    Qt5::Qml
    KF5::Solid
    KF5::IdleTime
    KF5::CoreAddons
    KF5::I18n
    KF5::Service
    Qt::DBus
)


install(TARGETS CaskPower EXPORT CaskPowerTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


install(FILES
    ${headers}
    ${CMAKE_CURRENT_BINARY_DIR}/caskpower_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Cask/Power
    COMPONENT Devel)

if (BUILD_SHARED_LIBS)
    add_custom_target(copy_power_to_bin ALL
        COMMAND ${CMAKE_COMMAND} -E
        make_directory ${CMAKE_BINARY_DIR}/bin/org/cask/power/
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/controls ${CMAKE_BINARY_DIR}/bin/org/cask/power/
        COMMAND ${CMAKE_COMMAND} -E
        copy $<TARGET_FILE:CaskPower> ${CMAKE_BINARY_DIR}/bin/org/cask/power/
        )

    if(QUICK_COMPILER)
        install(FILES controls/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/power)
    else()
        install(DIRECTORY controls/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/power)
    endif()

    #include(ECMGeneratePriFile)
    #ecm_generate_pri_file(BASE_NAME KQuickImageEditor LIB_NAME KQuickImageEditor DEPS "core qml quick svg" FILENAME_VAR PRI_FILENAME )
    #install(FILES ${PRI_FILENAME}
    #DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

endif()

install(TARGETS CaskPower DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/power )

