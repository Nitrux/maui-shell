add_definitions(-DTRANSLATION_DOMAIN=\"cutefish_pulseaudio\")

set(USE_GSETTINGS False)
set(USE_GCONF False)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(audio_SRCS
    card.cpp
    client.cpp
    context.cpp
    device.cpp
    maps.cpp
    operation.cpp
    port.cpp
    profile.cpp
    pulseaudio.cpp
    pulseobject.cpp
    sink.cpp
    sinkinput.cpp
    modulemanager.cpp
    source.cpp
    sourceoutput.cpp
    stream.cpp
    volumemonitor.cpp
    volumeobject.cpp
    debug.cpp
    server.cpp
    streamrestore.cpp
    module.cpp
    canberracontext.cpp
    speakertest.cpp
    qml/listitemmenu.cpp
    qml/plugin.cpp
    # qml/microphoneindicator.cpp
    # qml/volumeosd.cpp
    qml/volumefeedback.cpp

    model/sortfiltermodel.cpp
)

set(qml_SRCS
    qml/qmldir
    qml/PulseObjectFilterModel.qml
)

find_package(PkgConfig)
find_package(Canberra REQUIRED)

find_package(CanberraPulse)
set_package_properties(CanberraPulse PROPERTIES
    DESCRIPTION "Pulseaudio backend for libcanberra"
    PURPOSE "Required for volume feedback sounds"
    TYPE RUNTIME
)

find_package(SoundThemeFreedesktop)
set_package_properties(SoundThemeFreedesktop PROPERTIES
    DESCRIPTION "The standard freedesktop sound theme"
    PURPOSE "Required for volume feedback sounds"
    URL "https://www.freedesktop.org/wiki/Specifications/sound-theme-spec/"
    TYPE RUNTIME
)

pkg_check_modules(LIBPULSE libpulse REQUIRED IMPORTED_TARGET)
pkg_check_modules(LIBPULSE_MAINLOOP libpulse-mainloop-glib REQUIRED IMPORTED_TARGET)

add_library(cutefishaudio_qmlplugins SHARED ${audio_SRCS})

target_link_libraries(cutefishaudio_qmlplugins
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

install(TARGETS cutefishaudio_qmlplugins DESTINATION ${INSTALL_QMLDIR}/Cutefish/Audio)
install(FILES ${qml_SRCS} DESTINATION ${INSTALL_QMLDIR}/Cutefish/Audio)