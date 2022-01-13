add_definitions(-DTRANSLATION_DOMAIN=\"plasma_engine_powermanagement\")

set(powermanagement_engine_SRCS
    powermanagementengine.cpp
    powermanagementjob.cpp
    powermanagementservice.cpp
)

set(krunner_xml ${plasma-workspace_SOURCE_DIR}/krunner/dbus/org.kde.krunner.App.xml)
qt_add_dbus_interface(powermanagement_engine_SRCS ${krunner_xml} krunner_interface)

kcoreaddons_add_plugin(plasma_engine_powermanagement SOURCES ${powermanagement_engine_SRCS} INSTALL_NAMESPACE plasma/dataengine)

target_link_libraries(plasma_engine_powermanagement
    KF5::Solid
    KF5::Plasma
    KF5::IdleTime
    KF5::CoreAddons
    KF5::I18n
    KF5::Service
    Qt::DBus
    PW::KWorkspace
)

install(FILES powermanagementservice.operations DESTINATION ${PLASMA_DATA_INSTALL_DIR}/services)
