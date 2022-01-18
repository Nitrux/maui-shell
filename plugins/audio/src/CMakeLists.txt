#ecm_create_qm_loader(caskaudio_QM_LOADER libcasknotificationsplugin_qt)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/code
    )

set(USE_GSETTINGS False)
set(USE_GCONF False)

configure_file(code/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(audio_SRCS
    code/card.cpp
    code/client.cpp
    code/context.cpp
    code/device.cpp
    code/maps.cpp
    code/operation.cpp
    code/port.cpp
    code/profile.cpp
    code/pulseaudio.cpp
    code/pulseobject.cpp
    code/sink.cpp
    code/sinkinput.cpp
    code/modulemanager.cpp
    code/source.cpp
    code/sourceoutput.cpp
    code/stream.cpp
    code/volumemonitor.cpp
    code/volumeobject.cpp
    code/debug.cpp
    code/server.cpp
    code/streamrestore.cpp
    code/module.cpp
    code/canberracontext.cpp
    code/speakertest.cpp
    code/qml/listitemmenu.cpp
    code/qml/plugin.cpp
    # qml/microphoneindicator.cpp
    # qml/volumeosd.cpp
    code/qml/volumefeedback.cpp

    code/model/sortfiltermodel.cpp
    )

set(
    pluginData
    controls/qmldir
    )

if(QUICK_COMPILER)
    qtquick_compiler_add_resources(caskaudio_QML_QRC resources.qrc)
else()
    qt5_add_resources(caskaudio_QML_QRC resources.qrc)
endif()

ecm_generate_qmltypes(org.cask.audio ${PROJECT_VERSION} DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/audio)

add_library(CaskAudio SHARED ${audio_SRCS} ${caskaudio_QML_QRC})

generate_export_header(CaskAudio BASE_NAME CaskAudio)

target_include_directories(CaskAudio
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/Cask/Audio>")

target_include_directories(CaskAudio PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

if(QUICK_COMPILER)
    add_definitions(-DQUICK_COMPILER)
    target_compile_definitions(CaskAudio PUBLIC QUICK_COMPILER)
endif()


target_link_libraries(CaskAudio
    Qt5::Core
    Qt5::Qml
    Qt5::Gui
    Qt5::Widgets
    Qt5::DBus
    Qt5::Quick
    Canberra::Canberra
    PkgConfig::LIBPULSE
    PkgConfig::LIBPULSE_MAINLOOP
    )


install(TARGETS CaskAudio EXPORT CaskAudioTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


install(FILES
    ${headers}
    ${CMAKE_CURRENT_BINARY_DIR}/caskaudio_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Cask/Audio
    COMPONENT Devel)

if (BUILD_SHARED_LIBS)
    add_custom_target(copy_audio_to_bin ALL
        COMMAND ${CMAKE_COMMAND} -E
        make_directory ${CMAKE_BINARY_DIR}/bin/org/cask/audio/
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/controls ${CMAKE_BINARY_DIR}/bin/org/cask/audio/
        COMMAND ${CMAKE_COMMAND} -E
        copy $<TARGET_FILE:CaskAudio> ${CMAKE_BINARY_DIR}/bin/org/cask/audio/
        )

    if(QUICK_COMPILER)
        install(FILES controls/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/audio)
    else()
        install(DIRECTORY controls/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/audio)
    endif()

    #include(ECMGeneratePriFile)
    #ecm_generate_pri_file(BASE_NAME KQuickImageEditor LIB_NAME KQuickImageEditor DEPS "core qml quick svg" FILENAME_VAR PRI_FILENAME )
    #install(FILES ${PRI_FILENAME}
    #DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

endif()

install(TARGETS CaskAudio DESTINATION ${KDE_INSTALL_QMLDIR}/org/cask/audio )
